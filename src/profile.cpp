#include "profile.h"
#include "decoder.h"
#include <stdexcept>

ValueType_s *ValueType_s::decode(Buffer_t *buf, boost::container::vector<char> *data) {
    while (!data->empty()) {
        switch (buf->field) {
            case 1: {
                Decoder decoder;
                auto res = decoder.decode_field(buf, data);
                this->type_index = buf->u64;
                continue;
            }
            case 2: {
                this->unit_index = buf->u64;
                continue;
            }
            default: {
                throw std::runtime_error("unknown value_type type");
            }
        }
    }
    return this;
}
