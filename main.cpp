#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#include <cxxabi.h>
#include <memory>
static std::string demangle(const char* name) {
    auto demangledName = abi::__cxa_demangle(name, 0, 0, nullptr);
    auto result = std::string(demangledName);
    free(demangledName);
    return result;
}
#define DEMANGLE(CLASS) demangle(CLASS)
#elif defined(_MSC_VER)
#define DEMANGLE(CLASS) CLASS
#endif
///
#include <unistd.h>
#include <sys/times.h>

using ClockType = decltype(times(nullptr));

struct tms start, end;
ClockType clock_start, clock_end;

#define START_TIME() \
clock_start = times(&start);

#define END_TIME() \
clock_end = times(&end);

#define DELTA_TIME() \
(double(end.tms_utime - start.tms_utime) / sysconf(_SC_CLK_TCK)) 


const static size_t MAX_SIZE_DEG = 20; 
static uint8_t buffer[1 << MAX_SIZE_DEG];

///
#include <cstdint>
template<size_t many_times>
auto test(size_t block_size, size_t stride_size) -> decltype(DELTA_TIME()) {
    volatile uint8_t dummy;
    START_TIME();
    for (auto times = 0u; times < many_times; ++times) {
        for (auto i = 0u; i < block_size; i += stride_size) {
            dummy = buffer[i];
        }
    }
    END_TIME();
    return DELTA_TIME();
}

#include <iostream>
#include <fstream>
template<typename Stream>
void benchmark(Stream&& ofstream) {
    for (auto block_size_deg = 1; block_size_deg < MAX_SIZE_DEG; ++block_size_deg) {
        auto block_size = 1 << block_size_deg;        
        std::cout << block_size << std::endl;
        for (size_t stride_size_deg = 1; stride_size_deg < block_size_deg; ++stride_size_deg) {
            auto stride_size = 1 << stride_size_deg;
            auto value = test<10000>(block_size, stride_size);
            ofstream << block_size << ' ' << stride_size << ' ' << value << std::endl;
        }
    }
}

#include <fstream>
int main(int argc, char const *argv[]) {
    benchmark(std::ofstream("output.txt"));
    return 0;
}
