#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>


int main() {
    auto path = std::string("/home/valery/Projects/opensource/github/hype/tests/RR_CPU.pb.gz");
    std::ifstream myfile;


    std::vector<char> buffer;
    try {
        myfile.open(path, std::ios_base::binary);
        if (myfile.is_open()) {
            myfile.seekg(0, std::ios_base::end);
            size_t length = myfile.tellg();
            myfile.seekg(0, std::ios_base::beg);

            if (length > 0) {
                buffer.resize(length);
                myfile.read(&buffer[0], length);

                // check if file is gzipped
                if ((buffer[0] == char (0x1f)) and (buffer[1] == char (0x8b))) {
                    std::cout << "here" << std::endl;
                }
            }
        }
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        myfile.close();
        return EXIT_FAILURE;
    }


    myfile.

            close();

    return EXIT_SUCCESS;
}