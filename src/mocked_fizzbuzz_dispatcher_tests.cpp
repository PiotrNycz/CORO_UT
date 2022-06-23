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

#include <gtest/gtest.h>
#include "mocked_fizzbuzz_dispatcher.hpp"
#include "fizzbuzz_tests.hpp"
#include "fizzbuzz_awaiter_mock.hpp"

using namespace testing;

namespace FizzBuzz::Mocked
{

struct DispatcherMockedTests : TestWithSequence {
    std::stringstream actualStream;
};

TEST_P(DispatcherMockedTests, testExpectedSequence) {
    fizzbuzz(actualStream, GetParam().size);
    EXPECT_EQ(actualStream.str(), GetParam().sequence);
}

INSTANTIATE_TEST_SUITE_P(some, DispatcherMockedTests, ValuesIn(Test::someSequences));

struct DispatcherMock : Dispatcher
{
    MOCK_METHOD(AwaitNumber, waitFor, (int number, int priority), (override));
    MOCK_METHOD(AwaitAnyNumber, waitForUnwantedNumber, (), (override));
};

struct CoroutineTest : Test {
    std::stringstream actualStream;
    std::coroutine_handle<> handle;
    StrictMock<DispatcherMock> dispatcherMock;

    void resume()
    {
        ASSERT_TRUE(handle);
        std::exchange(handle, nullptr)();
    }
};

struct PrintStringTests : CoroutineTest {
    const int MODULO = 13;
    const int PRIORITY = 24;
    const std::string STR = "foo\n";
    PrintString objectUnderTest{MODULO, PRIORITY, STR};

    std::shared_ptr<AwaiterStrategyMock<void,void>> awaitNumberMock
        = std::make_shared<NiceMock<AwaiterStrategyMock<void,void>>>();
    Dispatcher::AwaitNumber awaitNumber{awaitNumberMock};

    PrintStringTests()
    {
        ON_CALL(dispatcherMock, waitFor(_, _)).WillByDefault(Return(awaitNumber));
        EXPECT_CALL(*awaitNumberMock, suspend(_))
            .WillRepeatedly(SaveArg<0>(&handle));
    }

};

TEST_F(PrintStringTests, printNothingWhenSuspendedInFirstIteration)
{
    EXPECT_CALL(dispatcherMock, waitFor(MODULO, PRIORITY));
    auto coro = objectUnderTest.run(dispatcherMock, actualStream);
    EXPECT_THAT(actualStream.str(), IsEmpty());
}

TEST_F(PrintStringTests, printStringWhenSuspendedInSecondIteration)
{
    {
        InSequence seq;
        EXPECT_CALL(dispatcherMock, waitFor(MODULO, PRIORITY));
        EXPECT_CALL(dispatcherMock, waitFor(2 * MODULO, PRIORITY));
    }
    auto coro = objectUnderTest.run(dispatcherMock, actualStream);
    resume();
    EXPECT_EQ(actualStream.str(), STR);
}

TEST_F(PrintStringTests, printDoubleStringWhenSuspendedInThirdIteration)
{
    {
        InSequence seq;
        EXPECT_CALL(dispatcherMock, waitFor(MODULO, PRIORITY));
        EXPECT_CALL(dispatcherMock, waitFor(2 * MODULO, PRIORITY));
        EXPECT_CALL(dispatcherMock, waitFor(3 * MODULO, PRIORITY));
    }
    auto coro = objectUnderTest.run(dispatcherMock, actualStream);
    resume();
    resume();
    EXPECT_EQ(actualStream.str(), STR + STR);
}

TEST_F(PrintStringTests, printNthStringsWhenSuspendedInNthPlusOneIteration)
{
    const unsigned N = 20;
    {
        InSequence seq;
        for (unsigned n = 1; n <= N + 1u; ++n)
            EXPECT_CALL(dispatcherMock, waitFor(n * MODULO, PRIORITY));
    }
    auto coro = objectUnderTest.run(dispatcherMock, actualStream);
    std::ostringstream expectedStream;
    for (unsigned n = 1; n <= N; ++n)
        expectedStream << STR;
    for (unsigned n = 1; n <= N; ++n)
        resume();
    EXPECT_EQ(actualStream.str(), expectedStream.str());
}

struct PrintNumberTests : CoroutineTest {
    std::shared_ptr<AwaiterStrategyMock<int,void>> awaitAnyNumberMock
        = std::make_shared<NiceMock<AwaiterStrategyMock<int,void>>>();
    Dispatcher::AwaitAnyNumber awaitAnyNumber{awaitAnyNumberMock};

    PrintNumberTests()
    {
        ON_CALL(dispatcherMock, waitForUnwantedNumber()).WillByDefault(Return(awaitAnyNumber));
        EXPECT_CALL(*awaitAnyNumberMock, suspend(_))
            .WillRepeatedly(SaveArg<0>(&handle));
    }
};

TEST_F(PrintNumberTests, printNothingWhenSuspendedInFirstIteration)
{
    EXPECT_CALL(dispatcherMock, waitForUnwantedNumber());
    auto coro = printNumber(dispatcherMock, actualStream);
    EXPECT_THAT(actualStream.str(), IsEmpty());
}

TEST_F(PrintNumberTests, printNumberWhenSuspendedInSecondIteration)
{
    EXPECT_CALL(dispatcherMock, waitForUnwantedNumber()).Times(2);
    EXPECT_CALL(*awaitAnyNumberMock, resume()).WillRepeatedly(Return(6));
    auto coro = printNumber(dispatcherMock, actualStream);
    resume();
    EXPECT_EQ(actualStream.str(), "6");
}

TEST_F(PrintNumberTests, printDoubleNumberWhenSuspendedInThirdIteration)
{
    EXPECT_CALL(dispatcherMock, waitForUnwantedNumber()).Times(3);
    EXPECT_CALL(*awaitAnyNumberMock, resume()).WillRepeatedly(Return(6));
    auto coro = printNumber(dispatcherMock, actualStream);
    resume();
    resume();
    EXPECT_EQ(actualStream.str(), "66");
}

TEST_F(PrintNumberTests, printNthNumbersWhenSuspendedInNthPlusOneIteration)
{
    const unsigned N = 13;
    EXPECT_CALL(dispatcherMock, waitForUnwantedNumber()).Times(N + 1u);
    EXPECT_CALL(*awaitAnyNumberMock, resume()).WillRepeatedly(Return(6));
    auto coro = printNumber(dispatcherMock, actualStream);
    for (unsigned n = 1; n <= N; ++n)
        resume();
    EXPECT_EQ(actualStream.str(), std::string(N, '6'));
}

}
