#include "profile.h"
#include "decoder.h"
#include <stdexcept>

ValueType_s ValueType_s::decode(Buffer_t &buf, boost::container::vector<char> &data) {
    type_index = 0;
    unit_index = 0;

    while (!data.empty()) {
        Decoder decoder;
        decoder.decode_field(buf, data);
        switch (buf.field) {
            case 1: {
                this->type_index = buf.u64;
                continue;
            }
            case 2: {
                this->unit_index = buf.u64;
                continue;
            }
            default: {
                throw std::runtime_error("unknown value_type type");
            }
        }
    }
    return *this;
}
