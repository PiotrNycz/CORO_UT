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
#include "fizzbuzz_generator.hpp"
#include <vector>
#include <ranges>

using namespace testing;

namespace FizzBuzz
{

struct GeneratorTests : TestWithParam<unsigned> {
    static inline std::vector<Generator::Value> expectedSequence{
        1,
        2,
        "fizz",
        4,
        "buzz",
        "fizz",
        7,
        8,
        "fizz",
        "buzz",
        11,
        "fizz",
        13,
        14,
        "fizzbuzz",
        16,
        17,
        "fizz",
        19,
        "buzz",
        "fizz",
        22,
        23,
        "fizz",
        "buzz",
        26,
        "fizz",
        28,
        29,
        "fizzbuzz",
        31
    };
};


TEST_P(GeneratorTests, testExpectedSequence) {
    ASSERT_LE(GetParam(), expectedSequence.size()) << " internal test error";
    auto gen = generator();
    for (auto expectedResult : expectedSequence | std::ranges::views::take(GetParam())) {
        auto actualResult = gen.next();
        ASSERT_TRUE(actualResult);
        EXPECT_EQ(*actualResult, expectedResult);
    }
}

INSTANTIATE_TEST_SUITE_P(upTo15, GeneratorTests, Range<unsigned>(0, 16));
INSTANTIATE_TEST_SUITE_P(moreThan15, GeneratorTests, Range<unsigned>(16,  1 + GeneratorTests::expectedSequence.size()));

}
