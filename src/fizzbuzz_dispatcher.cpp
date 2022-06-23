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

#include "fizzbuzz_dispatcher.hpp"
#include <ostream>

namespace FizzBuzz
{

Dispatcher::AwaitNumber Dispatcher::waitFor(int number, int priority)
{
    return AwaitNumber{
        .dispatcher = *this,
        .number = number,
        .priority = priority};
}

Dispatcher::AwaitAnyNumber Dispatcher::waitForUnwantedNumber()
{
    return AwaitAnyNumber{*this};
}

void Dispatcher::waitFor(int number, int priority, std::coroutine_handle<> handle)
{
    awaiters[Key{.number=number, .priority = priority}] = handle;
}

void Dispatcher::waitForUnwantedNumber(std::coroutine_handle<> handle)
{
    awaitersForAnyOtherNumber.push_back(handle);
}


void Dispatcher::dispatch(int number)
{
    currentNumber = number;
    auto it = awaiters.begin();
    if (it == awaiters.end() || it->first.number != currentNumber)
    {
        auto awaitersForAnyOtherNumber = std::move(this->awaitersForAnyOtherNumber);
        for (auto handle : awaitersForAnyOtherNumber)
            handle.resume();
        return;
    }
    for (;
         it != awaiters.end() && it->first.number == currentNumber;
         it = awaiters.erase(it))
    {
         it->second.resume();
    }
}

Procedure PrintString::run(Dispatcher& dispatcher, std::ostream& os)
{
    for (int n = modulo;; n += modulo) {
        co_await dispatcher.waitFor(n, priority);
        os << str;
    }
}

Procedure printNumber(Dispatcher& dispatcher, std::ostream& os)
{
    for (;;) {
        os << co_await dispatcher.waitForUnwantedNumber();
    }
}

void fizzbuzz(std::ostream& os, int size) {
    Dispatcher dispatcher;
    PrintString fizz{.modulo=3, .priority=1, .str="fizz"};
    PrintString buzz{.modulo=5, .priority=2, .str="buzz"};
    auto p1 = fizz.run(dispatcher, os);
    auto p2 = buzz.run(dispatcher, os);
    auto p3 = printNumber(dispatcher, os);
    for (int n = 1; n <= size; ++n) {
        dispatcher.dispatch(n);
        os << '\n';
    }
}

}