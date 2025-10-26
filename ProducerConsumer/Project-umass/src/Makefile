# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11 -pthread

# Target executable
TARGET = producer_consumer

# Source files
SOURCES = main.cpp BoundedBuffer.cpp ProducerConsumer.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Header files
HEADERS = BoundedBuffer.h ProducerConsumer.h

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Build successful! Run with: ./$(TARGET)"

# Compile source files to object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) output.txt
	@echo "Cleaned build artifacts and output files"

# Clean only object files (keep executable)
clean-obj:
	rm -f $(OBJECTS)
	@echo "Cleaned object files"

# Clean only output file
clean-output:
	rm -f output.txt
	@echo "Cleaned output.txt"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Rebuild everything
rebuild: clean all

# Display help information
help:
	@echo "Available targets:"
	@echo "  make          - Build the project"
	@echo "  make all      - Build the project (same as make)"
	@echo "  make clean    - Remove all build artifacts and output.txt"
	@echo "  make clean-obj    - Remove only object files"
	@echo "  make clean-output - Remove only output.txt"
	@echo "  make run      - Build and run the program"
	@echo "  make rebuild  - Clean and rebuild everything"
	@echo "  make help     - Display this help message"

# Phony targets (not actual files)
.PHONY: all clean clean-obj clean-output run rebuild help