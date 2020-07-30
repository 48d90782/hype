#ifndef HYPE_TYPES_H
#define HYPE_TYPES_H

#include <cstdlib>
#include <vector>

enum WireTypes {
    WireVarint [[maybe_unused]] = 0,
    WireFixed64 [[maybe_unused]] = 1,
    WireBytes [[maybe_unused]] = 2,
    WireFixed32 [[maybe_unused]] = 5,
};

struct Buffer {
    size_t field;
    WireTypes wireTypes;
    u_int64_t u64;
    std::vector<char> data;
};

#endif //HYPE_TYPES_H
