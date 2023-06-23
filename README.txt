Pros:
    Fast (uses 100% of your CPU :D )
    Memory efficient (less then 1Gb of RAM usage)
Cons:
    Not fully tested
    No dynamic max_chunk_size for every scanning thread
    Might crash. (did not encounter it)
    Might produce wrong result

How it works:
    It has std::vector<bool> that stores if the word was encountered. Index is a key - in this case the key is hashed string.
    The value True/False represents if it was already found in the file. Because of the std::vector<bool> is a template
    spezialized - so there is big storage optimization(read docs). So we use murmur hash that returns uint32_t(max 0xffffffff)
    In the end we count how many True values the vector of result has - and thats our RESULT of how many unique words we have
    in our file.

Required packages:
1. libmumurhash-dev
2. tracy(included)

Specs:
OS: Ubuntu 22.04.2 LTS x86_64
Kernel: 5.19.0-45-generic
CPU: AMD Ryzen 7 5700G with Radeon Graphics (16) @ 3.800GHz
GPU: AMD ATI 30:00.0 Cezanne
Memory: 5424MiB / 27951MiB

ryuu@ryuu-pc:~/parse_file/build$ time ./parse_file ../test.txt # 2.5Gb of random generated strings with length[1, 10]
Thread 140165824611904 finished his job!
Thread 140165807826496 finished his job!
Thread 140165859231296 finished his job!
Thread 140165798385216 finished his job!
Thread 140165867624000 finished his job!
Thread 140165695653440 finished his job!
Thread 140165712438848 finished his job!
Thread 140165841397312 finished his job!
Thread 140165816219200 finished his job!
Thread 140165850838592 finished his job!
Thread 140165687260736 finished his job!
Thread 140165833004608 finished his job!
Thread 140165704046144 finished his job!
Thread 140165876016704 finished his job!
Thread 140165678868032 finished his job!
Thread 140165720831552 finished his job!
Counting the results...
Result = 56690997

real	0m3,702s
user	0m56,384s
sys	0m0,388s

