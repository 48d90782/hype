#include "decoder.h"
#include "spdlog/spdlog.h"
#include <stdexcept>
#include <iostream>

uint64_t Decoder::decode_varint(fat_pointer_t &fp) {
    uint64_t u = 0;
    uint64_t i = 0;

    while (true) {
        // Message should be no more than 10 bytes
        if (i >= 10 || i >= fp.len) {
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
        // 0011101101010110 = 15190 10.4.8

        u |= (uint64_t) ((uint64_t) fp.data[fp.cursor + i] & 0x7Fu) << (uint64_t) (7 * i);

        // shl -> safe shift left operation
        // here we check all 8 bits for MSB
        // if all bits are zero, we are done
        // if not, MSB is set and there is presents next byte to read
        if (((uint64_t) fp.data[fp.cursor + i] & 0x80u) == 0) {
            fp.cursor += i + 1;
            fp.len -= i + 1;
            return u;
        }
        i += 1;
    }
}

uint64_t Decoder::decode_fixed64(const fat_pointer_t &fp) {
    if (fp.len < 8) {
        throw std::runtime_error("vector len is not enough to decode fixed64");
    }

    return ((uint64_t) fp.data[0] |
            (uint64_t) fp.data[1] << 8u |
            (uint64_t) fp.data[2] << 16u |
            (uint64_t) fp.data[3] << 24u |
            ((uint64_t) fp.data[4]) << 32u |
            ((uint64_t) fp.data[5]) << 40u |
            ((uint64_t) fp.data[6]) << 48u |
            ((uint64_t) fp.data[7]) << 56u);
}

std::string Decoder::decode_string(fat_pointer_t &fp) {
    if (fp.cursor == 0 && fp.len == 0) {
        return std::string{""};
    }
    return std::string{fp.data};
}

uint32_t Decoder::decode_fixed32(const fat_pointer_t &fp) {
    if (fp.len < 4) {
        throw std::runtime_error("vector len is not enough to decode fixed32");
    }

    return ((uint32_t) fp.data[0] |
            (uint32_t) fp.data[1] << 8u |
            (uint32_t) fp.data[2] << 16u |
            (uint32_t) fp.data[3] << 24u);
}

void Decoder::decode_message(Buffer_t &buf, Profile_t &prof, fat_pointer_t &fp) {
    while (fp.len != 0) {
        // here we decode data, the algorithm is following:
        // 1. We pass whole data and buffer to the decode_field function
        // 2. As the result we get main data (which drained to the buffer size) and buffer with that drained data filled with other fields
        // 3. We also calculate field, type and u64 fields to pass it to Profile::decode_profile function
        auto res = decode_field(buf, fp);
        // TODO return type, not void
        Decoder::decode_profile_field(prof, buf, res);
    }
}

fat_pointer_t Decoder::decode_field(Buffer_t &buf, fat_pointer_t &fp) {
    auto res = Decoder::decode_varint(fp);

    buf.field = res >> 3u;
    buf.type = WireTypes(res & 7u);
    buf.u64 = 0;

    fat_pointer_t buf_data = {};

    switch (buf.type) {
        case WireBytes: {
            auto var_int = Decoder::decode_varint(fp);

            buf_data.data = new char[var_int];
            buf_data.len = var_int;
            buf_data.cursor = 0;

            std::copy(fp.data + fp.cursor, fp.data + fp.cursor + var_int, buf_data.data);
            fp.len -= var_int;
            fp.cursor += var_int;

            return buf_data;
        }

        case WireVarint: {
            auto var_int = Decoder::decode_varint(fp);
            buf.u64 = var_int;
            break;
        }

        case WireFixed32: {
            buf.u64 = Decoder::decode_fixed32(fp);

            return buf_data;
        }

        case WireFixed64: {
            buf.u64 = Decoder::decode_fixed64(fp);

            return buf_data;
        }

        default:
            throw std::runtime_error("unknown type");
    }

    return buf_data;
}

void Decoder::decode_profile_field(Profile_t &prof, Buffer_t &buf, fat_pointer_t &fp) {
    switch (buf.field) {
        case 1: {
            ValueType_t vt;
            auto d = vt.decode(buf, fp);
            prof.sample_type.push_back(d);
            break;
        }
        case 2: {
            Sample_t st;
            auto s = st.decode(buf, fp);
            prof.sample.push_back(s);
            break;
        }
        case 3: {
            Mapping_t mp;
            auto res = mp.decode(buf, fp);
            prof.mapping.push_back(res);
            break;
        }
        case 4: {
            Location_t loc;
            prof.location.push_back(loc.decode(buf, fp));
            break;
        }
        case 5: {
            Function_t f;
            prof.function.push_back(f.decode(buf, fp));
            break;
        }
        case 6: {
            prof.string_table.push_back(Decoder::decode_string(fp));
            if (!prof.string_table.at(0).empty()) {
                throw std::runtime_error("string table[0] should be empty");
            }
            break;
        }
        case 7: {
            prof.drop_frames_index = buf.u64;
            break;
        }
        case 8: {
            prof.keep_frames_index = buf.u64;
            break;
        }
        case 9: {
            if (prof.time_nanos != 0) {
                throw std::runtime_error("concatenated profiles detected");
            }
            prof.time_nanos = buf.u64;
            break;
        }
        case 10: {
            prof.duration_nanos = buf.u64;
            break;
        }
        case 11: {
            ValueType_t vt;
            prof.period_type = vt.decode(buf, fp);
            break;
        }
        case 12: {
            prof.period = buf.u64;
            break;
        }
        case 13: {
            switch (buf.type) {
                case WireBytes: {
                    while (fp.len != 0) {
                        prof.comment_index.push_back(Decoder::decode_varint(fp));
                    }
                    break;
                }
                default:
                    prof.comment_index.push_back(buf.u64);
            }
            break;
        }
        case 14: {
            prof.default_sample_type_index = buf.u64;
            break;
        }
        default:
            break;
    }
}

void Decoder::post_decode(Profile_t &prof) {
    spdlog::info("starting post decode step");
    boost::unordered_map<uint64_t, Mapping_t> mappings;
    mappings.reserve(prof.mapping.size());
    Mapping_t mappingsIds[prof.mapping.size() + 1];

    for (auto &m: prof.mapping) {
        m.filename = prof.string_table.at(m.filename_index);
        m.build_id = prof.string_table.at(m.build_id_index);
        if (m.id < prof.mapping.size() + 1) {
            mappingsIds[m.id] = m;
        } else {
            mappings[m.id] = m;
        }
    }

    boost::unordered_map<uint64_t, Function_t> functions;
    functions.reserve(prof.function.size());
    Function_t functionIds[prof.function.size() + 1];

    for (auto &f:prof.function) {
        f.name = prof.string_table.at(f.name_index);
        f.system_name = prof.string_table.at(f.system_name_index);
        f.filename = prof.string_table.at(f.filename_index);

        if (f.id < prof.function.size() + 1) {
            functionIds[f.id] = f;
        } else {
            functions[f.id] = f;
        }
    }

    boost::unordered_map<uint64_t, Location_t> locations;
    locations.reserve(locations.size());
    Location_t locationIds[prof.location.size() + 1];

    for (auto &l:prof.location) {
        // pointers arithmetics to calculate len of the array
        // (&mappingsIds + 1) - end of the array (len)
        // *(&mappingsIds + 1) - mappingsIds) - size and dereference
        if (l.mapping_index < (unsigned) (*(&mappingsIds + 1) - mappingsIds)) {
            l.mapping = mappingsIds[l.mapping_index];
        } else {
            l.mapping = mappings[l.mapping_index];
        }
        l.mapping_index = 0;

        for (auto &ln : l.line) {
            auto id = ln.function_index;
            if (id != 0) {
                ln.function_index = 0;
                if (id < (unsigned) (*(&functionIds + 1) - functionIds)) {
                    ln.function = functionIds[id];
                } else {
                    ln.function = functions[id];
                }
            }
        }

        if (l.id < (unsigned) (*(&locationIds + 1) - locationIds)) {
            locationIds[l.id] = l;
        } else {
            locations[l.id] = l;
        }
    }


    for (auto &st:prof.sample_type) {
        st.type = prof.string_table.at(st.type_index);
        st.unit = prof.string_table.at(st.unit_index);
    }

    for (auto &s: prof.sample) {
        boost::unordered_map<std::string, boost::container::vector<std::string>> labels;
        labels.reserve(s.label_index.size());
        boost::unordered_map<std::string, boost::container::vector<int64_t>> num_labels;
        num_labels.reserve(s.label_index.size());
        boost::unordered_map<std::string, boost::container::vector<std::string>> num_units;
        num_units.reserve(s.label_index.size());

        for (auto &l:s.label_index) {
            std::string key, value;
            key = prof.string_table.at(l.key_index);

            if (l.str_index != 0) {
                value = prof.string_table.at(l.str_index);
                labels[key].push_back(value);
            } else if (l.num_index != 0) {
                auto num_values = num_labels[key];
                auto units = num_units[key];

                if (l.num_unit_index != 0) {
                    std::string unit;
                    unit = prof.string_table.at(l.num_unit_index);
                    units.resize(num_values.size());
                    num_units[key].push_back(unit);
                }
                num_labels[key].push_back(l.num_index);
            }
        }
        if (!labels.empty()) {
            s.label = labels;
        }
        if (!num_labels.empty()) {
            s.num_label = num_labels;
            for (auto &it:num_units) {
                if (!it.second.empty()) {
                    num_units[it.first].resize(num_labels.size());
                }
            }
            s.num_unit_label = num_units;
        }

        s.location.resize(s.location_index.size());

        for (unsigned i = 0; i < s.location_index.size(); ++i) {
            if (s.location_index[i] < (sizeof(locationIds) / sizeof(locationIds[0]))) {
                s.location[i] = locationIds[s.location_index[i]];
            } else {
                s.location[i] = locations[s.location_index[i]];
            }
        }

        s.location_index.clear();
    }

    prof.drop_frames = prof.string_table.at(prof.drop_frames_index);
    prof.keep_frames = prof.string_table.at(prof.keep_frames_index);

    prof.period_type.type = prof.string_table.at(prof.period_type.type_index);
    prof.period_type.unit = prof.string_table.at(prof.period_type.unit_index);

    for (auto &c:prof.comment_index) {
        prof.comments.push_back(prof.string_table.at(c));
    }

    prof.comment_index.clear();
    prof.default_sample_type = prof.string_table.at(prof.default_sample_type_index);
    prof.string_table.clear();


    spdlog::info("finished post decode step");
}

std::string Decoder::to_string(const Profile_t &prof) {
    return std::string();
}




























