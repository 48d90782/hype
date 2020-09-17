#include "decoder.h"
#include <iostream>
#include <stdexcept>

uint Decoder::decode_varint(boost::container::vector<char> *data) {
    uint u = 0;
    uint i = 0;

    while (true) {
        // Message should be no more than 10 bytes
        if (i >= 10 || i >= data->size()) {
            throw std::runtime_error("i more then data size");
        }

        // get 7 bits except MSB
        // here is would be a number w/o the sign bit
        // 0x7F --> 127. So, if the number in the self.data[i]
        // is eq to 127 there is probably MSB would be set to 1, and if it is
        // there is would be a second 7 bits of information
        // than we shift like this:
        //  1010 1100 0000 0010
        //  →010 1100  000 0010
        // and doing OR, because OR is like an ADDITION while A & B == 0
        // 86 | 15104 == 15190
        //         01010110         OR
        // 0011101100000000
        // 0011101101010110 = 15190
        u |= ((unsigned) data->at(i) & 0x7Fu) << (7 * i);

        // shl -> safe shift left operation
        // here we check all 8 bits for MSB
        // if all bits are zero, we are done
        // if not, MSB is set and there is presents next byte to read
        if (((unsigned) data->at(i) & 0x80u) == 0) {
            data->erase(data->begin(), data->begin() + i + 1);
            return u;
        }
        i += 1;
    }
}

uint64_t Decoder::decode_fixed64(const boost::container::vector<char> &p) {
    if (p.size() < 8) {
        throw std::runtime_error("vector len is not enough to decode fixed64");
    }

    return ((uint64_t) p.at(0) |
            (uint64_t) p.at(1) << 8u |
            (uint64_t) p.at(2) << 16u |
            (uint64_t) p.at(3) << 24u |
            ((uint64_t) p.at(4)) << 32u |
            ((uint64_t) p.at(5)) << 40u |
            ((uint64_t) p.at(6)) << 48u |
            ((uint64_t) p.at(7)) << 56u);
}

std::string Decoder::decode_string(const boost::container::vector<char> &p) {
    return std::string{p.begin(), p.end()};
}

uint32_t Decoder::decode_fixed32(const boost::container::vector<char> &p) {
    if (p.size() < 4) {
        throw std::runtime_error("vector len is not enough to decode fixed32");
    }

    return ((uint32_t) p.at(0) |
            (uint32_t) p.at(1) << 8u |
            (uint32_t) p.at(2) << 16u |
            (uint32_t) p.at(3) << 24u);
}

void Decoder::decode_message(Buffer_t &buf, Profile_t &prof, boost::container::vector<char> *data) {
    assert(data != nullptr);

    while (!data->empty()) {
        // here we decode data, the algorithm is following:
        // 1. We pass whole data and buffer to the decode_field function
        // 2. As the result we get main data (which drained to the buffer size) and buffer with that drained data filled with other fields
        // 3. We also calculate field, type and u64 fields to pass it to Profile::decode_profile function
        auto res = decode_field(buf, data);
        decode_profile_field(prof, buf, &res);
    }
}

boost::container::vector<char> Decoder::decode_field(Buffer_t &buf, boost::container::vector<char> *data) {
    auto res = decode_varint(data);

    std::cout << (res & 7u) << std::endl;
    buf.field = res >> 3u;
    buf.type = WireTypes(res & 7u);
    buf.u64 = 0;
    boost::container::vector<char> buf_data{};

    switch (buf.type) {
        case WireBytes: {
            auto varint = decode_varint(data);
            buf_data.reserve(varint);
            std::move(data->begin(), data->begin() + varint, std::back_inserter(buf_data));
            data->erase(data->begin(), data->begin() + varint);
            return buf_data;
        }

        case WireVarint: {
            break;
        }

        case WireFixed32: {
            break;
        }

        case WireFixed64: {
            break;
        }

        default:
            throw std::runtime_error("unknown type");
    }

    return buf_data;
}

void Decoder::decode_profile_field(Profile_t &prof, Buffer_t &buf, boost::container::vector<char> *buf_data) {
    switch (buf.field) {
        case 1: {
            ValueType_t vt;
            auto d = vt.decode(buf, buf_data);
            prof.sample_type.push_back(*d);
            std::cout << "done" << std::endl;
        }
    }
}




























