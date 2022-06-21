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

#include <coroutine>
#include <exception>
#include <string_view>
#include <ostream>
#include <memory>

namespace FizzBuzz::MockedAwait
{

template <typename ResumeType = void, typename SuspendType = bool>
class AwaitableStrategy
{
public:
    virtual ~AwaitableStrategy() = default;

    virtual bool ready() = 0;
    virtual SuspendType suspend(std::coroutine_handle<>) = 0;
    virtual ResumeType resume() = 0;
};

template <typename ResumeType = void, typename SuspendType = bool>
class Awaitable
{
public:
    Awaitable(std::shared_ptr<AwaitableStrategy<ResumeType, SuspendType>> strategy)
        : strategy(std::move(strategy))
    {}

    bool await_ready() { return strategy->ready(); }
    SuspendType await_suspend(std::coroutine_handle<> h) { return strategy->suspend(h); }
    ResumeType await_resume() { return strategy->resume(); }
private:
    std::shared_ptr<AwaitableStrategy<ResumeType, SuspendType>> strategy;
};

class Consumer {
public:
    virtual ~Consumer() = default;
    virtual Awaitable<bool> consume(int value) = 0;
    virtual Awaitable<bool> consume(std::string_view value) = 0;
};


class StreamConsumerUpToLimit : public Consumer {
public:
    template <typename T>
    struct AwaitValue : AwaitableStrategy<bool> {
        AwaitValue(T value, std::ostream* os)
            : value(std::move(value)), os(os)
        {}
        T value;
        std::ostream* os;

        bool ready() override {
            return os == nullptr;
        }
        bool suspend(std::coroutine_handle<>) override {
            *os << value << '\n';
            return false;
        }
        bool resume() override {
            return os != nullptr;
        }
    };

    StreamConsumerUpToLimit(std::ostream& os, unsigned limit) : os(os), limit(limit) {}
    Awaitable<bool> consume(int value) override {
        awaitInt->value = value;
        awaitInt->os = current++ < limit ? &os : nullptr;
        return Awaitable<bool, bool>(awaitInt);
    }
    Awaitable<bool> consume(std::string_view value) override {
        awaitString->value = value;
        awaitString->os = current++ < limit ? &os : nullptr;
        return Awaitable<bool, bool>(awaitString);
    }

private:
    std::ostream& os;
    const unsigned limit;
    unsigned current = 0;

    std::shared_ptr<AwaitValue<int>> awaitInt =
        std::make_shared<AwaitValue<int>>(0, nullptr);
    std::shared_ptr<AwaitValue<std::string_view>> awaitString =
        std::make_shared<AwaitValue<std::string_view>>("", nullptr);
};

class Producer {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    Producer(handle_type handle) : handle(handle) {}
    ~Producer() { handle.destroy(); }

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