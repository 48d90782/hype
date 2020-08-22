#ifndef HYPE_PROFILE_H
#define HYPE_PROFILE_H

#include <boost/numeric/ublas/vector.hpp>
#include <string>

struct ValueType {
    // Type and uint do not present in proto file
    // Used only for parsing
    // cpu, wall, inuse_space, etc
    std::string type;
    // seconds, nanoseconds, bytes, etc
    std::string unit;

    // index in the string table
    int64_t type_index;
    // index in the string table
    int64_t unit_index;
};

struct Profile {
    boost::numeric::ublas::vector<int> sample_type;


};

#endif //HYPE_PROFILE_H
