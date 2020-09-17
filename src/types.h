#ifndef HYPE_TYPES_H
#define HYPE_TYPES_H

#include <cstdlib>
#include <utility>
#include <boost/container/vector.hpp>

enum WireTypes {
    WireVarint [[maybe_unused]] = 0,
    WireFixed64 [[maybe_unused]] = 1,
    WireBytes [[maybe_unused]] = 2,
    WireFixed32 [[maybe_unused]] = 5,
};

typedef struct Buffer_s {
    Buffer_s(size_t fld, WireTypes wireTypes, uint64_t u64) :
            field(fld),
            type(wireTypes),
            u64(u64) {}

    size_t field;
    WireTypes type;
    uint64_t u64;
} Buffer_t;

#endif //HYPE_TYPES_H
