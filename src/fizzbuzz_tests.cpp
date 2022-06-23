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

#include "fizzbuzz_tests.hpp"
#include <ranges>
#include <sstream>

namespace FizzBuzz
{

const std::vector<Test::Element> Test::expectedSequence{
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

const std::vector<Test::Sequence> Test::someSequences{
    Sequence{0},
    Sequence{1},
    Sequence{2},
    Sequence{3},
    Sequence{4},
    Sequence{5},
    Sequence{7},
    Sequence{15},
    Sequence{}};

std::string Test::expectedSequenceOfSize(unsigned size)
{
    std::ostringstream oss;
    for (auto const& elem : expectedSequence | std::views::take(size))
        std::visit([&](const auto& value) { oss << value << '\n'; }, elem);
    return oss.str();
}

Test::Sequence::Sequence()
    : size(expectedSequence.size()),
      sequence(expectedSequenceOfSize(size))
{}

Test::Sequence::Sequence(unsigned size)
    : size(size),
      sequence(expectedSequenceOfSize(size))
{}

std::ostream& operator<<(std::ostream& os, const Test::Sequence& rhs)
{
    return os << rhs.size << ':' << rhs.sequence;
}

}
