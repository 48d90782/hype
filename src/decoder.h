#ifndef HYPE_DECODER_H
#define HYPE_DECODER_H

#include <utility>
#include <boost/container/vector.hpp>
#include "types.h"
#include "profile.h"

class Decoder {
public:
    explicit Decoder(boost::container::vector<char> data, Buffer_t buf) : data_(std::move(data)), buffer_(std::move(buf)) {};

    // no default constructor
    Decoder() = delete;

    // no assigment
    Decoder &operator=(const Decoder &) = delete;

    // decode varint
    uint decode_varint();

    void decode_message(Profile_t *prof);

    //
    static uint64_t decode_fixed64(const boost::container::vector<char> &p);

    static uint32_t decode_fixed32(const boost::container::vector<char> &p);

    static std::string decode_string(const boost::container::vector<char> &p);

    boost::container::vector<char> decode_field(Buffer_t *buf);

    ~Decoder();

private:
    boost::container::vector<char> data_;
    Buffer_t buffer_;
};


#endif //HYPE_DECODER_H
