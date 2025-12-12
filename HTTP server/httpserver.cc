#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <time.h>
#include <arpa/inet.h> 
#include <sys/wait.h>
const char* LOG_FILE = "server.log";

#define PORT 8080
#define PREFIX "httpdocs/"
using namespace std;

//stats part
time_t server_time;
clock_t start_req, end_req;
double cpu_time_used;
int request_count = 0;

double min_service_time = 1e9;
double max_service_time = 0;

string min_service_url = "";
string max_service_url = "";

//string expected = "Aladdin:open sesame";
string expected = "QWxhZGRpbjpvcGVuIHNlc2FtZQ==";
char *docType;
const char * error ="\nCould not find the specified URL.\n";

void processRequest(int socket,struct sockaddr_in clientAddr);
void writeRequest(int socket , string path);

void readRequest(int socket){
    char buffer[2048];
    int n = read(socket,buffer,sizeof(buffer)-1);
    buffer[n] = '\0';
    //cout<<"Request \n%s\n," << buffer;

}
void writeRequest(int socket , string path){
    // Frame the appropriate response header depending on whether the URL requested is found on the server or not.
    // Write the requested document (initially, you will respond with index.html that is found in htdocs/index.html) to the TCP connection.
    int fd = open(path.c_str(),O_RDONLY);
    char fileBuf[4096];
    int bytes;
    //docType = strdup("text/html");
    write(socket, "HTTP/1.1 ", strlen("HTTP/1.1 "));
    if(fd > 0 ){
        write(socket, "200 ", strlen("200 "));
		write(socket, "Content-type: ", strlen("Content-type: "));
        write(socket, docType, strlen(docType));
		write(socket, "\015\012", strlen("\015\012"));
		write(socket, "\015\012", strlen("\015\012"));
        while((bytes = read(fd,fileBuf,sizeof(fileBuf))) > 0){
            write(socket, fileBuf,bytes);
        }
        close(fd);
    }
    else {
        // if file not found  -> write into the response header
		//Case 404
		write(socket, "404 File Not Found", strlen("404 File Not Found"));
		write(socket, "\015\012", strlen("\015\012"));
		write(socket, "Content-type: ", strlen("Content-type: "));
		write(socket, "text/html", strlen("text/html"));
		write(socket, "\015\012", strlen("\015\012"));
		write(socket, "\015\012", strlen("\015\012"));

		//Case 404 : error = couldn't find specific URL
		write(socket, error, strlen(error));
    }
}


void Type(const string& filePath) {
    size_t dot = filePath.find_last_of('.');
    string fileExt;

    if (dot == string::npos)
        fileExt = "";
    else
        fileExt = filePath.substr(dot + 1);  // e.g., "html"

    //cout << "extension: " << fileExt << endl;

    if (fileExt == "html" || fileExt == "htm") {
        docType = strdup("text/html");
    }
    else if (fileExt == "gif") {
        docType = strdup("image/gif");
    }
    else if (fileExt == "jpeg" || fileExt == "jpg") {
        docType = strdup("image/jpeg");
    }
    else if (fileExt == "png") {
        docType = strdup("image/png");
    }
    else if (fileExt == "svg") {
        docType = strdup("image/svg+xml");
    }
    else {
        docType = strdup("text/plain");  // safer fallback
    }
}
void getAuthorization(int socket){
    // called when it doesn't exist
    const char* response =
        "HTTP/1.1 401 Unauthorized\r\n"
        "WWW-Authenticate: Basic realm=\"Secure Area\"\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html><body><h1>401 Unauthorized</h1>"
        "<p>Authorization required.</p></body></html>";
    
    
    write(socket,response,strlen(response));
    
    // check if the response has write password
    //check if the password is right
    // return  1 if success  

}

int getContentLength(const char* buffer) {
    int content_length = 0;
    const char* cl = strstr(buffer, "Content-Length:");
    if (cl) {
        sscanf(cl, "Content-Length: %d", &content_length);
    }
    return content_length;
}

void cgibin(int socket, const char* buffer, const std::string& method,
            const std::string& path, const std::string& query) 
{
    pid_t child = fork();
    if (child < 0) {
        perror("Fork failed");
        return;
    }

    if (child == 0) {
        dup2(socket, 1);   // redirect stdout → socket

        setenv("REQUEST_METHOD", method.c_str(), 1);

        if (method == "GET") {
            setenv("QUERY_STRING", query.c_str(), 1);
        }

        if (method == "POST") {
            int cl = getContentLength(buffer);

            char lenStr[32];
            snprintf(lenStr, sizeof(lenStr), "%d", cl);
            setenv("CONTENT_LENGTH", lenStr, 1);

            // POST body = after the blank line
            const char* body = strstr(buffer, "\r\n\r\n");
            if (body) body += 4;
            else body = "";

            setenv("QUERY_STRING", body, 1);
        }

        // Send header
        const char* header =
            "HTTP/1.1 200 OK\r\n"
            "Server: Cs252 CGI Server\r\n"
            "\r\n";

        write(socket, header, strlen(header));

        // exec CGI script
        char* args[] = {(char*)path.c_str(), NULL};
        execvp(args[0], args);

        perror("execvp failed");
        exit(1);
    }

    // parent
    waitpid(child, NULL, 0);
}



void sendStats(int socket){
    const char* response = "HTTP/1.1 200 OK \r\nContent-Type: text/html\r\n\r\n";
    write(socket, response,strlen(response));
    time_t now = time(NULL);
    double uptime = difftime(now,server_time);
    char res[2048];
    snprintf(res,sizeof(res),
        "<html><body>"
        "<h1>Server Statistics</h1>"
        "<p><b>Developer:</b> Hingis Martin </p>"
        "<p><b>Server Uptime:</b> %.2f seconds</p>"
        "<p><b>Total Requests:</b> %d</p>"
        "<p><b>Minimum Service Time:</b> %.6f seconds<br>"
        "<b>URL:</b> %s</p>"
        "<p><b>Maximum Service Time:</b> %.6f seconds<br>"
        "<b>URL:</b> %s</p>"
        "</body></html>", uptime, request_count,min_service_time,min_service_url.c_str(),max_service_time,max_service_url.c_str());
    
    // to write into stats.html
    
    string path = string(PREFIX) + "stats.html";
    FILE* file = fopen(path.c_str(),"w");
    if(file){
        fwrite(res, 1, strlen(res), file);
        fclose(file);
    }
    write(socket,res,strlen(res));
}

void logs(const char* client_ip, string path){
    string new_path = string(PREFIX) +LOG_FILE ;
    FILE* file = fopen(new_path.c_str(),"a");
    cout<<"logs "<<client_ip  << " " << path<<endl;
    if(file){
        
        fprintf(file, "%s %s\n", client_ip, path.c_str());
        cout<<"File " <<client_ip  << " " << path << endl;
        fclose(file);
    }
}

void streamLive(int socket){
    cout<<"Streaming";
    const char* header =
        "HTTP/1.1 200 OK\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: close\r\n"
        "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
    
    write(socket,header,strlen(header));
    int i = 0;
    while(1){
        i++;
        string filename = string(PREFIX) + "stream/frame" + to_string(i) + ".jpg";
        cout << "filename "<<filename << endl;
        int fd = open(filename.c_str(),O_RDONLY);
        if(fd<0) {  
            cout<<"break";
            break;
        }
        char buffer[65536];
        int bytes = read(fd,buffer,sizeof(buffer));
        close(fd);
        cout <<"bytes"<<bytes <<endl; 
        if(bytes <=0 ) break;
        write(socket, "--frame\r\n", strlen("--frame\r\n"));
        char jpgHeader[256];
        int hdrlen = snprintf(jpgHeader, sizeof(jpgHeader),
            "Content-Type: image/jpeg\r\n"
            "Content-Length: %d\r\n\r\n",
            bytes);
        
        write(socket,jpgHeader,hdrlen);
        write(socket,buffer,bytes);
        write(socket,"\r\n",2);
        usleep(1000);

    }
    
}
void processRequest(int socket,struct sockaddr_in clientAddr) {
    //The time the server has been up
    start_req = clock();
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, client_ip, INET_ADDRSTRLEN);
    // read the request header
    char buffer[1024];
    int n = read(socket,buffer,sizeof(buffer)-1);
    buffer[n] = '\0';
    //cout<<"Request \n" << buffer << endl;
    
    char method[16] , path[256], version[16] ;
    // request line looks like - GET /index.html HTTP/1.1 
    sscanf(buffer,"%s %s %s", method,path,version) ;  // read each char until next white space
    Type(path);
    //cout <<" Method : " << method<< " PATH : "<<path<<" Version : "<< version <<" Doctype : " <<docType<<endl;
    // manipulate the path 
    string new_path;
    char * authHeader = strstr(buffer,"Authorization");
    if(authHeader == NULL) {
        getAuthorization(socket);
        return;
    }
    //cout<<"Autho" <<authHeader;
    authHeader += strlen("Authorization: Basic ");
    char* end_head = strstr(authHeader,"\r\n");
    string encoded(authHeader, end_head - authHeader);
    //cout<< encoded <<endl;
    if(strcmp(encoded.c_str(),expected.c_str()) !=0){
        getAuthorization(socket);
        return;
    }
    if(strcmp(path,  "/")==0) {
        new_path = string(PREFIX) + "index.html" ;    
    }
    else if(strcmp(path,"/stats") == 0){
        sendStats(socket);
        return;
    }
    else if(strcmp(path,"/logs") == 0){
        new_path = string(PREFIX) +LOG_FILE ;
        cout << new_path;
    }
    else if(strcmp(path,"/stream/play")==0){
        streamLive(socket);
    }
    else if(strcmp(path,"/cgi-bin")==0){
        string query = "";
        char * q = strstr(path,'?');
        if(q) {
            query = q+1;
        }
        cout << query;
        cgibin(socket,buffer,method,query);
    }

    else {
        new_path = string(PREFIX) + path ;
    }
    logs(client_ip,string(new_path));
    cout << client_ip << " " << new_path <<endl;
    cout.flush();
    //cout << new_path <<endl;
    //open that directory and read the file into socket
    writeRequest(socket,new_path);
    end_req = clock();
    cpu_time_used = ((double) (end_req - start_req)) / CLOCKS_PER_SEC;
    //The number of requests since the server started
    request_count++;
    //The minimum service time and the URL request that took this time.
    if(cpu_time_used < min_service_time){
        min_service_time = cpu_time_used;
        min_service_url = path;
    }
    //The maximum service time and the URL request that took this time.
    if(cpu_time_used > max_service_time){
        max_service_time = cpu_time_used;
        max_service_url = path;
    }
        
    close(socket);
}


int main(int argc , char const * argv[]){
    server_time = time(NULL);
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    // have to wait about 2 minutes before reusing the sae port number
    int opt = 1;
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080 # Allows multiple processes or threads to bind the same address + port simultaneously.
    //“Let me use the port again right now.” SO_REUSEADDR
    if (setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address))< 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    cout << "listening" <<endl;
    // forever loop 
    while(1) {
        //Accept new TCP connection
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address,&addrlen))< 0) {
            perror("accept");
            //exit(EXIT_FAILURE);
            continue ; 
        }
        
        //Read request from TCP connection and parse it.
        processRequest(new_socket,address);
        // Close TCP connection
        
        close(new_socket);
        
        
    
    }
    
    // closing the connected socket
    close(server_fd);
    return 0;
}