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
    static uint64_t decode_varint(fat_pointer_t &fp);

    static void decode_message(Buffer_t &buf, Profile_t &prof, fat_pointer_t &fp);

    //
    static uint64_t decode_fixed64(const fat_pointer_t &fp);

    static uint32_t decode_fixed32(const fat_pointer_t &fp);

    static std::string decode_string(const fat_pointer_t &fp);

    static fat_pointer_t decode_field(Buffer_t &buf, fat_pointer_t &fp);

    static void decode_profile_field(Profile_t &prof, Buffer_t &buf, fat_pointer_t &fp);

    static void post_decode(Profile_t &prof);

    static std::string to_string(const Profile_t &prof);

    ~Decoder() = default;
};


#endif //HYPE_DECODER_H
