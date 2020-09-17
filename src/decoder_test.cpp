#include <gtest/gtest.h>
#include "decoder.h"

TEST(decode_fixed64, HandleCorrectVector) {
    boost::container::vector<char> sample;
    sample.push_back(char(112));
    sample.push_back(char(32));
    sample.push_back(char(112));
    sample.push_back(char(114));
    sample.push_back(char(111));
    sample.push_back(char(102));
    sample.push_back(char(105));
    sample.push_back(char(108));

    Decoder d{sample};
    auto res = d.decode_fixed64(sample);
    ASSERT_EQ(res, 7811887657498189936);
}

TEST(decode_fixed32, HandleCorrectVector) {
    boost::container::vector<char> sample;
    sample.push_back('-');
    sample.push_back('-');
    sample.push_back(' ');
    sample.push_back('c');

    Decoder d{sample};
    auto res = d.decode_fixed32(sample);
    ASSERT_EQ(res, 1663053101);
}


int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}