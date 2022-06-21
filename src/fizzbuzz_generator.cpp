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

#include "fizzbuzz_generator.hpp"
#include <limits>

namespace FizzBuzz
{

Generator generator()
{
    const int sequence_size = 3*5;
    for (int i = 0; i + sequence_size < std::numeric_limits<int>::max(); i += sequence_size)
    {
        co_yield i + 1;
        co_yield i + 2;
        co_yield "fizz";
        co_yield i + 4;
        co_yield "buzz";
        co_yield "fizz";
        co_yield i + 7;
        co_yield i + 8;
        co_yield "fizz";
        co_yield "buzz";
        co_yield i + 11;
        co_yield "fizz";
        co_yield i + 13;
        co_yield i + 14;
        co_yield "fizz""buzz";
    }
}

}