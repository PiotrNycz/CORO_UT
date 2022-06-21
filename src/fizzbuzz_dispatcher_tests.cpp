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
#include "fizzbuzz_dispatcher.hpp"
#include <sstream>
#include <algorithm>

using namespace testing;

namespace FizzBuzz
{

struct DispatcherTestsBase : Test {
    struct Param {
        unsigned sequenceSize;
        std::string sequence;
    };
};

struct DispatcherTests : DispatcherTestsBase, WithParamInterface<DispatcherTestsBase::Param> {
    std::stringstream actualStream;

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

TEST_P(DispatcherTests, testExpectedSequence) {
    fizzbuzz(actualStream, GetParam().sequenceSize);
    EXPECT_EQ(actualStream.str(), GetParam().sequence);
}

INSTANTIATE_TEST_SUITE_P(variousSequences, DispatcherTests,
    Values(
        DispatcherTests::Param{0, ""},
        DispatcherTests::Param{1, DispatcherTests::getSequenceOfSize(1)},
        DispatcherTests::Param{2, DispatcherTests::getSequenceOfSize(2)},
        DispatcherTests::Param{15, DispatcherTests::getSequenceOfSize(15)},
        DispatcherTests::Param{31, DispatcherTests::fullSequence}
    ));
}
