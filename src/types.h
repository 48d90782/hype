#ifndef HYPE_TYPES_H
#define HYPE_TYPES_H

#include <cstdlib>
#include <utility>
#include <boost/container/vector.hpp>
#include <boost/unordered_map.hpp>

enum WireTypes {
    WireVarint = 0,
    WireFixed64 = 1,
    WireBytes = 2,
    WireFixed32 = 5,
};

typedef struct Buffer_s {
    Buffer_s(size_t fld, WireTypes wireTypes, uint64_t u64) :
            field(fld),
            type(wireTypes),
            u64(u64) {}

    size_t field;
    WireTypes type;
    uint64_t u64;
} Buffer_t;

typedef struct ValueType_s {
    // Type and uint do not present in proto file
    // Used only for parsing
    // cpu, wall, inuse_space, etc
    std::string type;
    // seconds, nanoseconds, bytes, etc
    std::string unit;

    // index in the string table
    int64_t type_index;
    // index in the string table
    int64_t unit_index;

public:
    ValueType_s decode(Buffer_t &buf, boost::container::vector<char> &data);
} ValueType_t;


// mapping corresponds to Profile.Mapping
typedef struct Mapping_s {
    // Unique nonzero id for the mapping.
    uint64_t id;
    // Address at which the binary (or DLL) is loaded into memory.
    uint64_t memory_start;
    // The limit of the address range occupied by this mapping.
    uint64_t memory_limit;
    // Offset in the binary that corresponds to the first mapped address.
    uint64_t memory_offset;
    // Index into string table
    // The object this entry is loaded from.  This can be a filename on
    // disk for the main binary and shared libraries, or virtual
    // abstractions like "[vdso]".
    std::string filename;
    // Index into string table
    // A string that uniquely identifies a particular program version
    // with high probability. E.g., for binaries generated by GNU tools,
    // it could be the contents of the .note.gnu.build-id field.
    std::string build_id;
    bool has_function;
    bool has_filenames;
    bool has_line_numbers;
    bool has_inline_frames;

    // Index into string table
    int64_t filename_index;
    // Index into string table
    int64_t build_id_index;
} Mapping_t;

typedef struct Label_s {
    // Index into string table
    int64_t key_index;

    // one of the two following values must be set
    int64_t str_index;
    // Index into string table
    int64_t num_index;

    // Should only be present when num is present.
    // Specifies the units of num.
    // Use arbitrary string (for example, "requests") as a custom count unit.
    // If no unit is specified, consumer may apply heuristic to deduce the unit.
    // Consumers may also  interpret units like "bytes" and "kilobytes" as memory
    // units and units like "seconds" and "nanoseconds" as time units,
    // and apply appropriate unit conversions to these.
    int64_t num_unit_index;
} Label_t;

typedef struct Function_s {
    // Unique nonzero id for the function.
    uint64_t id;
    // Name of the function, in human-readable form if available.
    std::string name;
    // Name of the function, as identified by the system.
    // For instance, it can be a C++ mangled name.
    std::string system_name;
    // Source file containing the function.
    std::string filename;
    // Line number in source file.
    int64_t start_line;

    // HELPERS
    // Index into string table
    int64_t name_index;
    // Index into string table
    int64_t system_name_index;
    // Index into string table
    int64_t filename_index;
} Function_t;

typedef struct Line_s {
    // Line number in source code.
    int64_t line{};
    // The id of the corresponding profile.Function for this line.
    uint64_t function_index{};

    // HELPERS
    Function_t function;
} Line_t;

// Describes function and line table debug information.
typedef struct Location_s {
    // Unique nonzero id for the location.  A profile could use
    // instruction addresses or any integer sequence as ids.
    uint64_t id;
    // The id of the corresponding profile.Mapping for this location.
    // It can be unset if the mapping is unknown or not applicable for
    // this profile type.
    uint64_t mapping_index;
    // The instruction address for this location, if available.  It
    // should be within [Mapping.memory_start...Mapping.memory_limit]
    // for the corresponding mapping. A non-leaf address may be in the
    // middle of a call instruction. It is up to display tools to find
    // the beginning of the instruction if necessary.
    uint64_t address;
    // Multiple line indicates this location has inlined functions,
    // where the last entry represents the caller into which the
    // preceding entries were inlined.
    //
    // E.g., if memcpy() is inlined into printf:
    //    line[0].function_name == "memcpy"
    //    line[1].function_name == "printf"
    boost::container::vector<Line_t> line;
    // Provides an indication that multiple symbols map to this location's
    // address, for example due to identical code folding by the linker. In that
    // case the line information above represents one of the multiple
    // symbols. This field must be recomputed when the symbolization state of the
    // profile changes.
    bool is_folder;

    //HELPER
    Mapping_t mapping;
} Location_t;

// Each Sample records values encountered in some program
// context. The program context is typically a stack trace, perhaps
// augmented with auxiliary information like the thread-id, some
// indicator of a higher level request being handled etc.
typedef struct Sample_s {
    // The ids recorded here correspond to a Profile.location.id.
    // The leaf is at location_id[0].
    boost::container::vector<Location_t> location;
    // The type and unit of each value is defined by the corresponding
    // entry in Profile.sample_type. All samples must have the same
    // number of values, the same as the length of Profile.sample_type.
    // When aggregating multiple samples into a single sample, the
    // result has a list of values that is the elemntwise sum of the
    // lists of the originals.
    boost::container::vector<int64_t> value;

    // label includes additional context for this sample. It can include
    // things like a thread id, allocation size, etc
    boost::unordered_map<std::string, boost::container::vector<std::string>> label;
    // key is label.key_index(in string table), value is associated str_index
    // entry in Profile.sample_type
    boost::unordered_map<std::string, boost::container::vector<int64_t>> num_label;
    // label and numbers in string table, key_index is a key
    // label and unit measurement, key_index also is a key
    boost::unordered_map<std::string, boost::container::vector<std::string>> num_unit_label;


    // These types are not present in the proto file
    boost::container::vector<uint64_t> location_index;
    boost::container::vector<Label_t> label_index;

} Sample_t;

// Profile is an in-memory representation of profile.proto
typedef struct Profile_s {
    // A description of the samples associated with each Sample.value.
    // For a cpu profile this might be:
    //   [["cpu","nanoseconds"]] or [["wall","seconds"]] or [["syscall","count"]]
    // For a heap profile, this might be:
    //   [["allocations","count"], ["space","bytes"]],
    // If one of the values represents the number of events represented
    // by the sample, by convention it should be at index 0 and use
    // sample_type.unit == "count".
    boost::container::vector<ValueType_t> sample_type;
    // The set of samples recorded in this profile.
    boost::container::vector<Sample_t> sample;
    // Mapping from address ranges to the image/binary/library mapped
    // into that address range.  mapping[0] will be the main binary.
    boost::container::vector<Mapping_t> mapping;
    // Useful program location
    boost::container::vector<Location_t> location;
    // Functions referenced by locations
    boost::container::vector<Function_t> function;
    // A common table for strings referenced by various messages.
    // string_table[0] must always be "".
    boost::container::vector<std::string> string_table;
    // frames with Function.function_name fully matching the following
    // regexp will be dropped from the samples, along with their successors.
    std::string drop_frames;


    // Index into string table.
    // frames with Function.function_name fully matching the following
    // regexp will be kept, even if it matches drop_functions.
    std::string keep_frames;
    // The following fields are informational, do not affect
    // interpretation of results.
    // Time of collection (UTC) represented as nanoseconds past the epoch.
    int64_t time_nanos;
    // Duration of the profile, if a duration makes sense.
    int64_t duration_nanos;
    // The kind of events between sampled ocurrences.
    // e.g [ "cpu","cycles" ] or [ "heap","bytes" ]
    ValueType_t period_type;
    // The number of events between sampled occurrences.
    int64_t period;
    // Freeform text associated to the profile.
    boost::container::vector<std::string> comments;
    // Indices into string table.
    // Index into the string table of the type of the preferred sample
    // value. If unset, clients should default to the last sample value.
    std::string default_sample_type;

    // Index into string table.
    boost::container::vector<int64_t> comment_index;
    // Index into string table.
    int64_t drop_frames_index;
    // Index into string table.
    int64_t keep_frames_index;
    // Index into string table.
    int64_t default_sample_type_index;

} Profile_t;

#endif //HYPE_TYPES_H
