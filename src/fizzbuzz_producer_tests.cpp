/*
MIT License

Copyright (c) 2022 Piotr Nycz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <gtest/gtest.h>
#include "fizzbuzz_producer.hpp"
#include <sstream>
#include <algorithm>

using namespace testing;

namespace FizzBuzz
{

struct ProducerTestsBase : Test {
    struct Param {
        unsigned sequenceSize;
        std::string sequence;
    };
};

struct ProducerTests : ProducerTestsBase, WithParamInterface<ProducerTestsBase::Param> {
    std::stringstream actualStream;
    Consumer consumer{actualStream, GetParam().sequenceSize};

    static inline std::string fullSequence =
        "1\n"
        "2\n"
        "fizz\n"
        "4\n"
        "buzz\n"
        "fizz\n"
        "7\n"
        "8\n"
        "fizz\n"
        "buzz\n"
        "11\n"
        "fizz\n"
        "13\n"
        "14\n"
        "fizzbuzz\n"
        "16\n"
        "17\n"
        "fizz\n"
        "19\n"
        "buzz\n"
        "fizz\n"
        "22\n"
        "23\n"
        "fizz\n"
        "buzz\n"
        "26\n"
        "fizz\n"
        "28\n"
        "29\n"
        "fizzbuzz\n"
        "31\n";
    static std::string getSequenceOfSize(unsigned size) {
        auto it = std::find_if(fullSequence.begin(), fullSequence.end(), [&](char c) { return c=='\n' && size-- <= 1u; });
        return std::string(fullSequence.begin(), std::next(it));
    }
};

TEST_P(ProducerTests, testExpectedSequence) {
    auto gen = producer(consumer);
    EXPECT_EQ(actualStream.str(), GetParam().sequence);
}

INSTANTIATE_TEST_SUITE_P(upTo15, ProducerTests,
    Values(
        ProducerTests::Param{0, ""},
        ProducerTests::Param{1, ProducerTests::getSequenceOfSize(1)},
        ProducerTests::Param{2, ProducerTests::getSequenceOfSize(2)},
        ProducerTests::Param{15, ProducerTests::getSequenceOfSize(15)},
        ProducerTests::Param{31, ProducerTests::fullSequence}
    ));

}