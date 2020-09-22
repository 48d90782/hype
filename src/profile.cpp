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


Sample_s Sample_s::decode(Buffer_t &buf, boost::container::vector<char> &data) {
    while (!data.empty()) {
        auto res = Decoder::decode_field(buf, data);
        switch (buf.field) {
            case 1: {
                if (buf.type == WireBytes) {
                    while (!res.empty()) {
                        auto var_int = Decoder::decode_varint(res);
                        location_index.push_back(var_int);
                    }
                    continue;
                }

                assert(buf.type == WireVarint);
                location_index.push_back(buf.u64);
            }
            case 2: {
                if (buf.type == WireBytes) {
                    while (!res.empty()) {
                        auto var_int = Decoder::decode_varint(res);
                        value.push_back(var_int);
                    }
                    continue;
                }
                assert(buf.type == WireVarint);
                value.push_back(buf.u64);
            }
            case 3: {
                Label_t l;
                auto lbl = l.decode(buf, data);
                label_index.push_back(lbl);
            }
        }

    }
    return *this;
}

Label_s Label_s::decode(Buffer_t &buf, boost::container::vector<char> &data) {
    key_index = 0;
    str_index = 0;
    num_index = 0;
    num_unit_index = 0;

    while (!data.empty()) {
        Decoder::decode_field(buf, data);
        switch (buf.field) {
            case 1:
                key_index = buf.u64;
                break;
            case 2:
                str_index = buf.u64;
                break;
            case 3:
                num_index = buf.u64;
                break;
            case 4:
                num_unit_index = buf.u64;
                break;
            default:
                throw std::runtime_error("unknown label type");
        }
    }

    return *this;
}

Location_s Location_s::decode(Buffer_t &buf, boost::container::vector<char> &data) {
    id = 0;
    mapping_index = 0;
    address = false;
    line = {};
    is_folder = false;
    mapping = {};

    while (!data.empty()) {
        auto decode_res = Decoder::decode_field(buf, data);
        switch (buf.field) {
            case 1: {
                id = buf.u64;
                break;
            }
            case 2: {
                mapping_index = buf.u64;
                break;
            }
            case 3: {
                address = buf.u64;
                break;
            }
            case 4: {
                Line_t ln{};
                line.push_back(ln.decode(buf, decode_res));
                break;
            }
            case 5: {
                if (buf.u64 == 0) {
                    is_folder = false;
                } else {
                    is_folder = true;
                }
                break;
            }

            default:
                throw std::runtime_error("unknown type of location");
        }
    }

    return *this;
}

Line_s Line_s::decode(Buffer_t &buf, boost::container::vector<char> &data) {
    function_index = 0;
    line = 0;
    while (!data.empty()) {
        Decoder::decode_field(buf, data);
        switch (buf.field) {
            case 1: {
                function_index = buf.u64;
                break;
            }
            case 2: {
                line = buf.u64;
                break;
            }

            default:
                throw std::runtime_error("unknown type of line");
        }
    }
    return *this;
}

Function_s Function_s::decode(Buffer_t &buf, boost::container::vector<char> &data) {
    id = 0;
    name = std::string{};
    system_name = std::string{};
    filename = std::string{};
    start_line = 0;

    name_index = 0;
    system_name_index = 0;
    filename_index = 0;

    while (!data.empty()) {
        Decoder::decode_field(buf, data);
        switch (buf.field) {
            case 1: {
                id = buf.u64;
                break;
            }
            case 2: {
                name_index = buf.u64;
                break;
            }
            case 3: {
                system_name_index = buf.u64;
                break;
            }
            case 4: {
                filename_index = buf.u64;
                break;
            }
            case 5: {
                start_line = buf.u64;
            }
            default:
                throw std::runtime_error("unknown function case");
        }
    }

    return *this;
}
