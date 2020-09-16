#include "Decoder.h"
#include <iostream>

uint Decoder::decode_varint() {
    uint u = 0;
    uint i = 0;

    while (true) {
        // Message should be no more than 10 bytes
        if (i >= 10 || i >= data_.size()) {
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
        u |= ((unsigned) data_.at(i) & 0x7Fu) << (7 * i);

        // shl -> safe shift left operation
        // here we check all 8 bits for MSB
        // if all bits are zero, we are done
        // if not, MSB is set and there is presents next byte to read
        if (((unsigned) data_.at(i) & 0x80u) == 0) {
            data_.erase(data_.begin(), data_.begin() + i);
            return u;
        }
        i += 1;
    }
}

Decoder::~Decoder() {
    // clear all data
    data_.clear();
}
