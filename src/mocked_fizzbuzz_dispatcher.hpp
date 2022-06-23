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
#pragma once

#include <coroutine>
#include <exception>
#include <map>
#include <utility>
#include <iosfwd>
#include <string_view>
#include "fizzbuzz_awaiter.hpp"

namespace FizzBuzz::Mocked
{

class Procedure {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    Procedure(handle_type handle) : handle(handle) {}
    ~Procedure() { if (handle) handle.destroy(); }

    struct promise_type {
        std::exception_ptr exception;

        Procedure get_return_object() {
            return {handle_type::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { exception = std::current_exception(); }
    };

    handle_type handle;
};

struct Dispatcher {
public:

    using AwaitNumber = Awaiter<void,void>;
    using AwaitAnyNumber = Awaiter<int,void>;

    virtual ~Dispatcher() = default;
    virtual AwaitNumber waitFor(int number, int priority) = 0;
    virtual AwaitAnyNumber waitForUnwantedNumber() = 0;
};

class RealDispatcher : public Dispatcher {
public:

    AwaitNumber waitFor(int number, int priority) override;
    AwaitAnyNumber waitForUnwantedNumber() override;

    void dispatch(int number);

private:
    struct AwaitNumberStrategy : AwaiterStrategy<void,void> {
        AwaitNumberStrategy(RealDispatcher& dispatcher, int number, int priority)
            : number(number), priority(priority), dispatcher(dispatcher)
        {}
        RealDispatcher& dispatcher;
        int number;
        int priority;
        bool ready() override {
            return false;
        }
        void suspend(std::coroutine_handle<> handle) override {
            dispatcher.waitFor(number, priority, handle);
        }
        void resume() override {}
    };

    struct AwaitAnyNumberStrategy : AwaiterStrategy<int,void> {
        AwaitAnyNumberStrategy(RealDispatcher& dispatcher)
            : dispatcher(dispatcher)
        {}
        RealDispatcher& dispatcher;
        bool ready() override {
            return false;
        }
        void suspend(std::coroutine_handle<> handle) override {
            dispatcher.waitForUnwantedNumber(handle);
        }
        int resume() override {
            return dispatcher.currentNumber;
        }
    };

    void waitFor(int number, int priority, std::coroutine_handle<> handle);
    void waitForUnwantedNumber(std::coroutine_handle<> handle);

    using PriorityAwaiters = std::map<int, std::coroutine_handle<>>;
    using AwaitersForNumber = std::map<int, PriorityAwaiters>;

    AwaitersForNumber awaiters;
    std::coroutine_handle<> awaiterForAnyOtherNumber;
    int currentNumber{};
};

struct PrintString
{
    int modulo;
    int priority;
    std::string_view str;
    Procedure run(Dispatcher& dispatcher, std::ostream& os);
};
Procedure printNumber(Dispatcher& dispatcher, std::ostream& os);

void fizzbuzz(std::ostream& os, int size);

}