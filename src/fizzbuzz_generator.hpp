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
#include <optional>
#include <variant>
#include <string_view>

namespace FizzBuzz
{

class Generator {
public:
    using Value = std::variant<int, std::string_view>;
    using Result = std::optional<Value>;

    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    Generator(handle_type handle) : handle(handle) {}
    ~Generator() { if (handle) handle.destroy(); }


    struct promise_type {
        Result result;
        std::exception_ptr exception;

        Generator get_return_object() {
            return {handle_type::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(int value) {
            result = value;
            return {};
        }
        std::suspend_always yield_value(std::string_view value) {
            result = value;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { exception = std::current_exception(); }
    };

    Result next() {
        handle();
        if (handle.done())
            return std::nullopt;
        if (handle.promise().exception)
            std::rethrow_exception(handle.promise().exception);
        return handle.promise().result;
    }

    handle_type handle;
};

Generator generator();

}