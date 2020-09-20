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

    auto res = Decoder::decode_fixed64(sample);
    ASSERT_EQ(res, 7811887657498189936);
}

TEST(decode_fixed32, HandleCorrectVector) {
    boost::container::vector<char> sample;
    sample.push_back('-');
    sample.push_back('-');
    sample.push_back(' ');
    sample.push_back('c');

    auto res = Decoder::decode_fixed32(sample);
    ASSERT_EQ(res, 1663053101);
}

//TEST(IteratorTest, IteratorTestMe) {
//    std::vector<int> v1, v2;
//
//    for (int i = 0; i < 100; ++i) {
//        v1.push_back(i);
//    }
//
//    v1.erase(v1.begin(), v1.begin() + 2);
//    v2.reserve(v1.size());
//}


int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}