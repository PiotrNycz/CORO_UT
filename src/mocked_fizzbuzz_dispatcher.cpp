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

#include "mocked_fizzbuzz_dispatcher.hpp"
#include <ostream>

namespace FizzBuzz::Mocked
{

Dispatcher::AwaitNumber RealDispatcher::waitFor(int number, int priority)
{
    return AwaitNumber(std::make_shared<AwaitNumberStrategy>(
            *this, number, priority));
}

Dispatcher::AwaitAnyNumber RealDispatcher::waitForUnwantedNumber()
{
    return AwaitAnyNumber(std::make_shared<AwaitAnyNumberStrategy>(*this));
}

void RealDispatcher::waitFor(int number, int priority, std::coroutine_handle<> handle)
{
    awaiters[number][priority] = handle;
}

void RealDispatcher::waitForUnwantedNumber(std::coroutine_handle<> handle)
{
    awaiterForAnyOtherNumber = handle;
}

void RealDispatcher::dispatch(int number)
{
    currentNumber = number;
    auto it = awaiters.find(number);
    if (it == awaiters.end()) {
        if (auto handle = std::exchange(awaiterForAnyOtherNumber, nullptr))
            handle.resume();
    } else {
        auto awaitersForNumber = std::move(it->second);
        awaiters.erase(it);
        for (auto& entry : awaitersForNumber)
            entry.second.resume();
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
    RealDispatcher dispatcher;
    PrintString fizz{.modulo=3, .priority=1, .str="fizz"};
    PrintString buzz{.modulo=5, .priority=2, .str="buzz"};
    auto p1 = fizz.run(dispatcher, os);
    auto p2 = buzz.run(dispatcher, os);
    auto p3 = printNumber(dispatcher, os);
    for (int n = 0; n < size; ++n) {
        dispatcher.dispatch(n + 1);
        os << '\n';
    }
}

}