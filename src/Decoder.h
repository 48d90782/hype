#ifndef HYPE_DECODER_H
#define HYPE_DECODER_H

#include <boost/container/vector.hpp>

class Decoder {
public:
    explicit Decoder(boost::container::vector<char> &data): _data(data) {};

    size_t decode_varint();

private:
    boost::container::vector<char> _data;
};


#endif //HYPE_DECODER_H
