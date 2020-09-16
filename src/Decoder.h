#ifndef HYPE_DECODER_H
#define HYPE_DECODER_H

#include <utility>
#include <boost/container/vector.hpp>

class Decoder {
public:
    explicit Decoder(boost::container::vector<char> data) : data_(std::move(data)) {};

    // no default constructor
    Decoder() = delete;

    uint decode_varint();

    ~Decoder();

private:
    boost::container::vector<char> data_;
};


#endif //HYPE_DECODER_H
