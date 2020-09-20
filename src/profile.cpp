#include "profile.h"
#include "decoder.h"
#include <stdexcept>

ValueType_s ValueType_s::decode(Buffer_t &buf, boost::container::vector<char> &data) {
    type_index = 0;
    unit_index = 0;

    while (!data.empty()) {
        Decoder::decode_field(buf, data);
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

Mapping_s Mapping_s::decode(Buffer_t &buf, boost::container::vector<char> &data) {
    // set defaults
    id = 0;
    memory_start = 0;
    memory_limit = 0;
    memory_offset = 0;
    filename = std::string{""};
    build_id = std::string{""};
    has_function = false;
    has_filenames = false;
    has_line_numbers = false;
    has_inline_frames = false;
    filename_index = 0;
    build_id_index = 0;

    while (!data.empty()) {
        Decoder::decode_field(buf, data);
        switch (buf.field) {
            case 1:
                id = buf.u64;
                break;
            case 2:
                memory_start = buf.u64;
                break;
            case 3:
                memory_limit = buf.u64;
                break;
            case 4:
                memory_offset = buf.u64;
                break;
            case 5:
                filename_index = buf.u64;
                break;
            case 6:
                build_id_index = buf.u64;
                break;
            case 7: {
                if (buf.u64 == 0)
                    has_function = false;
                else
                    has_function = true;
                break;
            }
            case 8: {
                if (buf.u64 == 0)
                    has_filenames = false;
                else
                    has_filenames = true;
                break;
            }
            case 9: {
                if (buf.u64 == 0)
                    has_line_numbers = false;
                else
                    has_line_numbers = true;
                break;
            }
            case 10: {
                if (buf.u64 == 0)
                    has_inline_frames = false;
                else
                    has_inline_frames = true;
                break;
            }
            default:
                throw std::runtime_error("unknown mapping type");
        }
    }

    return *this;
}
