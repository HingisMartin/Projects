# Producer-Consumer Problem with Bounded Buffer

## Overview
This project implements the classic **Producer-Consumer synchronization problem** using POSIX threads (pthreads) in C++. Multiple producer threads generate items and place them into a shared bounded buffer, while multiple consumer threads remove and process items from the buffer.

## Features
- Multi-threaded producer-consumer implementation
- Thread-safe bounded buffer with synchronization
- Configurable number of producers and consumers
- Adjustable sleep times for producers and consumers
- Mutex-based synchronization to prevent race conditions
- Output logging to both console and file

## Files
- **`main.cpp`** - Main driver program
- **`ProducerConsumer.cpp`** - Producer and consumer thread implementations
- **`ProducerConsumer.h`** - Header file with function declarations
- **`BoundedBuffer.cpp`** - Bounded buffer implementation
- **`BoundedBuffer.h`** - Bounded buffer class definition

## Requirements
- C++ compiler with C++11 support (g++)
- POSIX threads library (pthread)
- Linux/Unix environment (or WSL on Windows)

## Compilation
```bash
g++ main.cpp BoundedBuffer.cpp ProducerConsumer.cpp -o producer_consumer -pthread
```

## Usage
```bash
./producer_consumer
```

The program will:
1. Create specified number of producer and consumer threads
2. Producers generate random items and add them to the buffer
3. Consumers remove items from the buffer
4. All operations are logged to `output.txt` and console
5. Program terminates when all items are produced and consumed

## Configuration
Modify parameters in `main.cpp` or `InitProducerConsumer()` call:

```cpp
InitProducerConsumer(num_producers, num_consumers, producer_sleep_ms, consumer_sleep_ms, total_items);
```

**Parameters:**
- `num_producers` - Number of producer threads
- `num_consumers` - Number of consumer threads
- `producer_sleep_ms` - Producer sleep time in milliseconds
- `consumer_sleep_ms` - Consumer sleep time in milliseconds
- `total_items` - Total number of items to produce/consume

## Implementation Details

### Synchronization Mechanisms
- **`count_lock`** - Protects shared counters (`produced_items`, `consumed_items`)
- **`file_lock`** - Protects file write operations
- **BoundedBuffer internal locks** - Protects buffer operations

### Key Variables
- `produced_items` - Tracks total items produced
- `consumed_items` - Tracks total items consumed
- `sharedBuffer` - Bounded buffer (size: 10) shared between threads

### Thread Functions
- **`producer()`** - Generates random items and adds to buffer
- **`consumer()`** - Removes items from buffer and processes them

## Output
The program generates `output.txt` containing:
```
Producer 0 wrote item 347
Consumer 1 read item 347
Producer 2 wrote item 892
Consumer 0 read item 892
...
```

## Example Run
```bash
$ ./producer_consumer
Creating Producer thread 0
Creating Producer thread 1
Creating Consumer thread 0
Creating Consumer thread 1
Producer 0 wrote item 42
Consumer 0 read item 42
Producer 1 wrote item 789
Consumer 1 read item 789
...
All threads completed
```

## Common Issues

### Compilation Warnings
If you see pointer cast warnings, ensure you're using:
```cpp
pthread_create(&thread, NULL, function, (void*)(long)i);
```

### Deadlocks
- Ensure proper lock/unlock ordering
- Don't hold locks while calling blocking operations

### Race Conditions
- All shared variable access must be protected by mutexes
- File writes must be synchronized

## Thread Safety
✅ Counters protected by mutex  
✅ File writes protected by mutex  
✅ Buffer operations internally synchronized  
✅ Proper thread creation and joining  

## Cleanup
The program automatically cleans up:
- Destroys all mutexes
- Closes output file
- Deletes dynamically allocated memory
- Joins all threads before exit

## License
Educational/Academic use

## Author
HINGIS NADAR

## Notes
- Buffer size is hardcoded to 10 items
- Random items are in range [0, 999]
- Ensure proper permissions for creating `output.txt`

---

For questions or issues, refer to the POSIX threads documentation or course materials.