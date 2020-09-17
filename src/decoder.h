#ifndef HYPE_DECODER_H
#define HYPE_DECODER_H

#include <utility>
#include <boost/container/vector.hpp>
#include "types.h"


class Decoder {
public:
    explicit Decoder(boost::container::vector<char> data) : data_(std::move(data)) {};

    // no default constructor
    Decoder() = delete;

    // no assigment
    Decoder &operator=(const Decoder &) = delete;

    // decode varint
    uint decode_varint();

    //
    static uint64_t decode_fixed64(const boost::container::vector<char> &p);

    static uint32_t decode_fixed32(const boost::container::vector<char> &p);

    static std::string decode_string(const boost::container::vector<char> &p);

    ~Decoder();

private:
    boost::container::vector<char> data_;
    Buffer buffer_;
};


#endif //HYPE_DECODER_H
