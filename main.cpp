/*
 * Copyright © 2023 Taras Boichuk
 * All rights reserved.
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 *   documentation files (the "Software"), to deal in the Software without restriction, including without limitation
 *   the rights to use, copy, modify, merge, publish, and distribute, subject to the following conditions:
 * 1. The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 2. The Software may not be sold, nor may it be used in a commercial product or activity without the express written 
 *   permission of the copyright holder, Taras Boichuk.
 * 3. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
 *   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 *   COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * By using, copying or distributing this software you indicate your agreement to the above terms.
 */

// #define TRACY_ENABLED
#ifdef TRACY_ENABLED
    #include <Tracy.hpp>
#else
    #define ZoneScoped 
#endif

#include <murmurhash.h>

#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <string_view>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>

// #define ENABLE_LOG
#ifdef ENABLE_LOG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif 

constexpr size_t max_chunk_size = 1048576 * 1; // 1mb * 10
constexpr size_t max_words_count = 0xffffffff;

struct Chunk
{
    size_t start;
    size_t end;
};

struct UserData
{
    std::string_view filepath;
    size_t filesize;

    std::mutex owned_chunks_lock;
    std::vector<Chunk> owned_chunks;

    std::mutex result_lock;
    std::vector<bool> result; // std::vector<bool> - bit manipulation, so saves a lot of RAM
};

inline uint32_t hash_str(char* str, size_t length)
{
    ZoneScoped;
    uint32_t out;
    lmmh_x86_32(str, length, 0, &out);
    return out;
}

void scan(UserData* const ud)
{
    ZoneScoped;
    ud->owned_chunks_lock.lock();
    const int32_t fd = open(ud->filepath.data(), O_RDONLY);
    const size_t start = ud->owned_chunks.size() ? ud->owned_chunks.back().end : 0;
    if (start >= ud->filesize)
    {
        ud->owned_chunks_lock.unlock();
        LOG("Thread %lu finished his job!\n", std::this_thread::get_id());
        return;
    }

    const size_t page_size = sysconf(_SC_PAGE_SIZE);
    const size_t offset = start - (start % page_size);

    char* mem = static_cast<char*>(mmap(nullptr, max_chunk_size, PROT_READ, MAP_PRIVATE, fd, offset));
    if (mem == MAP_FAILED)
    {
        LOG("Mapping failed. Exiting");
        exit(-1);
    }
    close(fd);

    size_t end_pos = 0;
    if (start + max_chunk_size >= ud->filesize)
    {
        end_pos = ud->filesize - start;
    }
    else
    {
        end_pos = max_chunk_size - 1;
        while (mem[end_pos] != ' ')
            end_pos--;
    }

    const size_t end = start + end_pos;

    ud->owned_chunks.push_back({ .start = start, .end = end });
    ud->owned_chunks_lock.unlock();

    size_t start_pos = start - offset;
    for (size_t i = start_pos; i < end_pos;)
    {
        while (i < end_pos && mem[i] == ' ')
            i++;

        // LOG("Working\n");
        size_t s = i;
        while (s < end_pos && mem[s] != ' ')
            s++;
        
        uint32_t hashed = hash_str(&mem[i], s - i);

        // ud->result_lock.lock(); // might be not exact result without? but slower! what chances???
        ud->result[hashed] = true;
        // ud->result_lock.unlock();
        
        i = s + 1;
    }

    if (munmap(mem, max_chunk_size))
        LOG("Faileed to free mem!");
    scan(ud);
}

int main(int argc, char** argv)
{
    ZoneScoped;
    if (argc < 2)
    {
        LOG("Invalid arguments! Please provide filepath\n");
        LOG("  ./parse_file <filepath>\n");
        return -1;
    }

    const std::string_view filepath = argv[1];
    const uint16_t thread_count = std::thread::hardware_concurrency();
    const uint16_t scan_thread_count = thread_count;

    const size_t filesize = [&](){
        struct stat st;
        stat(filepath.data(), &st);
        return st.st_size;
    }();

    UserData ud = {
        .filepath = filepath,
        .filesize = filesize,
        .result = std::vector<bool>(max_words_count),
    };

    {
        std::vector<std::thread> thread_pool(scan_thread_count);
        for (uint16_t i = 0; i < scan_thread_count; i++)
        {
            thread_pool[i] = std::thread(scan, &ud);
        }

        for (auto& t : thread_pool)
            t.join();
    }

    LOG("Counting the results...\n");

    std::vector<size_t> results(scan_thread_count);
    {
        std::vector<std::thread> thread_pool(scan_thread_count);
        for (uint16_t i = 0; i < scan_thread_count; i++)
        {
            const size_t chunk = max_words_count / (size_t)scan_thread_count;
            const size_t start = i*chunk;
            const size_t end = i*chunk+chunk - 1;
            thread_pool[i] = std::thread([](uint16_t id, 
                size_t start, size_t end, 
                const std::vector<bool>& result,
                size_t& out 
            ){
                out = 0;
                for (size_t i = start; i < end; i++)
                    out += result[i];
                // printf("Local res = %lu\n", out);
            }, i, start, end, std::ref(ud.result), std::ref(results[i]));
        }

        for (auto& t : thread_pool)
            t.join();
    }
    
    size_t result = 0;
    for (const auto r : results)
        result += r;

    printf("Result = %lu\n", result);

    return 0;
}
