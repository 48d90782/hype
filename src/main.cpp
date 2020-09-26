#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include "decoder.h"
#include <boost/container/vector.hpp>
#include <boost/program_options.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/assert.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include "spdlog/spdlog.h"

namespace opt = boost::program_options;

int main(int argc, char *argv[]) {
    opt::options_description description("All options");

    description.add_options()(
            "path", opt::value<std::string>(), "path to the pprof file"
    );

    opt::variables_map vm;

    opt::store(opt::parse_command_line(argc, argv, description), vm);

    try {
        opt::notify(vm);
    } catch (const opt::required_option &err) {
        std::cout << "Error: " << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    BOOST_ASSERT(!vm.empty());
    auto path = vm["path"].as<std::string>();
    std::unique_ptr<std::ifstream> file = std::make_unique<std::ifstream>();


    // buffer is the initial file content
    // it could be compressed

    try {
        spdlog::info("start reading the file");
        file->open(path, std::ios::binary);
        if (file->is_open()) {
            file->seekg(0, std::ios::end);
            size_t length = file->tellg();
            file->seekg(0, std::ios::beg);

            if (length > 0) {
                boost::container::vector<char> buffer(length);
                file->read(&buffer[0], length);

                // check is there data gzipped
                // https://tools.ietf.org/html/rfc1952#page-5
                if ((buffer[0] == char(0x1f)) && (buffer[1] == char(0x8b))) {
                    // gzipped data
                    file->seekg(0, std::ios::beg);
                    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
                    in.push(boost::iostreams::gzip_decompressor());
                    in.push(*file); // deref smart pointer
                    buffer.clear();
                    buffer.assign(std::istreambuf_iterator<char>{&in}, {});
                    BOOST_ASSERT(!buffer.empty());
                }

                //char *arr = &buffer[0];

                fat_pointer_t fp{
                        .data = buffer.data(),
                        .len = buffer.size(),
                        .cursor = 0,
                };

                assert(fp.data != nullptr);

                Buffer_t buf{0, WireBytes, 0};
                Profile_t profile{};

                spdlog::info("start decoding");
                Decoder::decode_message(buf, profile, fp);
                spdlog::info("decode_message done");
                Decoder::post_decode(profile);
                spdlog::info("post_decode done");
            }
        }
    } catch (const std::exception &e) { // catch all exceptions here
        std::cout << e.what() << std::endl;
        file->close();
        return EXIT_FAILURE;
    }

    // close the file (good path)
    file->close();

    return EXIT_SUCCESS;
}