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

#include "mocked_await_fizzbuzz_producer.hpp"
#include <limits>


namespace FizzBuzz::MockedAwait
{

Producer producer(Consumer& consumer)
{
    const int sequence_size = 3*5;
    for (int i = 0; i + sequence_size < std::numeric_limits<int>::max(); i += sequence_size)
    {
        if (!co_await consumer.consume( i + 1 )) break;
        if (!co_await consumer.consume( i + 2 )) break;
        if (!co_await consumer.consume( "fizz" )) break;
        if (!co_await consumer.consume( i + 4 )) break;
        if (!co_await consumer.consume( "buzz" )) break;
        if (!co_await consumer.consume( "fizz" )) break;
        if (!co_await consumer.consume( i + 7 )) break;
        if (!co_await consumer.consume( i + 8 )) break;
        if (!co_await consumer.consume( "fizz" )) break;
        if (!co_await consumer.consume( "buzz" )) break;
        if (!co_await consumer.consume( i + 11 )) break;
        if (!co_await consumer.consume( "fizz" )) break;
        if (!co_await consumer.consume( i + 13 )) break;
        if (!co_await consumer.consume( i + 14 )) break;
        if (!co_await consumer.consume( "fizz""buzz" )) break;
    }
}

}