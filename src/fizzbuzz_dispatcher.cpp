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
    awaitAnyOtherNumber = handle;
}


void Dispatcher::dispatch(int number)
{
    currentNumber = number;
    auto it = awaiters.begin();
    if (awaitAnyOtherNumber && it == awaiters.end() || it->first.number != currentNumber)
    {
        awaitAnyOtherNumber->resume();
        awaitAnyOtherNumber = std::nullopt;
        return;
    }
    for (;
         it != awaiters.end() && it->first.number == currentNumber;
         it = awaiters.erase(it))
    {
         it->second.resume();
    }
}

Procedure fizz(Dispatcher& dispatcher, int priority, std::ostream& os)
{
    for (int n = 3;; n += 3) {
        co_await dispatcher.waitFor(n, priority);
        os << "fizz";
    }
}

Procedure buzz(Dispatcher& dispatcher, int priority, std::ostream& os)
{
    for (int n = 5;; n += 5) {
        co_await dispatcher.waitFor(n, priority);
        os << "buzz";
    }
}

Procedure other(Dispatcher& dispatcher, std::ostream& os)
{
    for (;;) {
        os << co_await dispatcher.waitForUnwantedNumber();
    }
}

void fizzbuzz(std::ostream& os, int size) {
    Dispatcher dispatcher;
    auto p1 = fizz(dispatcher, 1, os);
    auto p2 = buzz(dispatcher, 2, os);
    auto p3 = other(dispatcher, os);
    for (int n = 1; n <= size; ++n) {
        dispatcher.dispatch(n);
        os << '\n';
    }
}

}