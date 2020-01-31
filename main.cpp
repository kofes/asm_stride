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

///
#include <cstdint>
template<size_t many_times, typename stride_type>
auto test(size_t block_size) -> decltype(DELTA_TIME()) {
    uint8_t buffer[block_size];
    stride_type dummy;
    START_TIME();
    for (auto times = 0u; times < many_times; ++times) {
        for (auto i = 0u; i < block_size; i += sizeof(stride_type)) {
            dummy = reinterpret_cast<stride_type&>(buffer[i]);
        }
    }
    END_TIME();
    return DELTA_TIME();
}

#include <iostream>
template<typename stride_type, typename Stream>
void benchmark(Stream&& ofstream) {
    size_t max_size = 1 << 12;
    for (size_t size = 0u; size < max_size; ++size) {
        ofstream << size << ' ' << sizeof(stride_type) << ' ' << test<100000, stride_type>(size)  << std::endl;
    }
}

#include <fstream>
int main(int argc, char const *argv[]) {
    using type1 = uint8_t;
    benchmark<type1>(std::ofstream(DEMANGLE(typeid(type1).name()) + ".txt"));
    using type2 = uint16_t;
    benchmark<type2>(std::ofstream(DEMANGLE(typeid(type2).name()) + ".txt"));
    using type3 = uint32_t;
    benchmark<type3>(std::ofstream(DEMANGLE(typeid(type3).name()) + ".txt"));
    using type4 = uint64_t;
    benchmark<type4>(std::ofstream(DEMANGLE(typeid(type4).name()) + ".txt"));
    return 0;
}
