#ifndef HYPE_DECODER_H
#define HYPE_DECODER_H

#include <utility>
#include <boost/container/vector.hpp>
#include "profile.h"

class Decoder {
public:
    // default constructor
    Decoder() = default;

    // no assigment
    Decoder &operator=(const Decoder &) = delete;

    // decode varint
    static uint64_t decode_varint(boost::container::vector<char> &data);

    static void decode_message(Buffer_t &buf, Profile_t &prof, boost::container::vector<char> &data);

    //
    static uint64_t decode_fixed64(const boost::container::vector<char> &p);

    static uint32_t decode_fixed32(const boost::container::vector<char> &p);

    static std::string decode_string(const boost::container::vector<char> &p);

    static boost::container::vector<char> decode_field(Buffer_t &buf, boost::container::vector<char> &data);

    static void decode_profile_field(Profile_t &prof, Buffer_t &buf, boost::container::vector<char> &buf_data);

    static void post_decode(Profile_t &prof);

    static std::string to_string(const Profile_t &prof);

    ~Decoder() = default;
};


#endif //HYPE_DECODER_H
