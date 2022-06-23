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
#include <string_view>
#include <ostream>

namespace FizzBuzz
{

class Consumer {
    template <typename T>
    struct AwaitValue {
        T value;
        std::ostream* os;
        bool await_ready() {
            return os == nullptr;
        }
        bool await_suspend(std::coroutine_handle<> handle) {
            *os << value << '\n';
            return false;
        }
        bool await_resume() {
            return os != nullptr;
        }
    };

public:
    Consumer(std::ostream& os, unsigned limit) : os(os), limit(limit) {}
    auto consume(int value) { return AwaitValue{value, current++ < limit ? &os : nullptr}; }
    auto consume(std::string_view value) { return AwaitValue{value, current++ < limit ? &os : nullptr}; }

private:
    std::ostream& os;
    const unsigned limit;
    unsigned current = 0;
};

class Producer {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    Producer(handle_type handle) : handle(handle) {}
    ~Producer() { if (handle) handle.destroy(); }

    struct promise_type {
        std::exception_ptr exception;

        Producer get_return_object() {
            return {handle_type::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { exception = std::current_exception(); }
    };

    handle_type handle;
};

Producer producer(Consumer&);

}