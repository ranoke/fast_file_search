Pros:
    Fast (uses 100% of your CPU :D )
    Memory efficient (less then 1Gb of RAM usage)
Cons:
    Not fully tested
    No dynamic max_chunk_size for every scanning thread
    Might crash. (did not encounter it)
    Might produce wrong result

How it works:
    The algorithm utilizes std::vector<bool> to keep track of whether a word has been encountered. The index acts as a key, which in this case is a hashed string. 
    The value, True/False, represents whether the word has already been found in the file. Since std::vector<bool> is a template specialization, there is significant storage optimization (refer to the documentation for more details). 
    The algorithm employs MurmurHash, which returns a uint32_t (with a maximum value of 0xFFFFFFFF), to hash the strings. 
    In the end, the algorithm counts how many True values are in the vector, and this count represents the number of unique words in the file.


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

