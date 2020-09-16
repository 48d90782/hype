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
    uint64_t decode_fixed64(const boost::container::vector<char> &p);
    uint32_t decode_fixed32();
    std::string decode_string(const boost::container::vector<char> &p);

    ~Decoder();

private:
    boost::container::vector<char> data_;
};


#endif //HYPE_DECODER_H
