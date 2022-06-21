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
#include <gmock/gmock.h>
#include "mocked_await_fizzbuzz_producer.hpp"

using namespace testing;

namespace FizzBuzz::MockedAwait
{

template <typename ResumeType = void, typename SuspendType = bool>
class AwaitableStrategyMock : public AwaitableStrategy<ResumeType, SuspendType>
{
public:
    MOCK_METHOD(bool, ready, (), (override));
    MOCK_METHOD(SuspendType, suspend, (std::coroutine_handle<>), (override));
    MOCK_METHOD(ResumeType, resume, (), (override));
};

class ConsumerMock : public Consumer {
public:
    MOCK_METHOD(Awaitable<bool>, consume, (int value), (override));
    MOCK_METHOD(Awaitable<bool>, consume, (std::string_view value), (override));
};

struct ProducerTestsMockedAwait : Test {
    StrictMock<ConsumerMock> consumerMock;
    std::shared_ptr<AwaitableStrategyMock<bool>> wantsMoreMock
        = std::make_shared<NiceMock<AwaitableStrategyMock<bool>>>();
    std::shared_ptr<AwaitableStrategyMock<bool>> doesNotWantMoreMock
        = std::make_shared<NiceMock<AwaitableStrategyMock<bool>>>();

    Awaitable<bool> wantsMore{wantsMoreMock};
    Awaitable<bool> doesNotWantMore{doesNotWantMoreMock};

    ProducerTestsMockedAwait()
    {
        ON_CALL(*wantsMoreMock, ready()).WillByDefault(Return(false));
        ON_CALL(*wantsMoreMock, resume()).WillByDefault(Return(true));
        ON_CALL(*wantsMoreMock, suspend(_)).WillByDefault([](auto){return false;});

        ON_CALL(*doesNotWantMoreMock, ready()).WillByDefault(Return(true));
        ON_CALL(*doesNotWantMoreMock, resume()).WillByDefault(Return(false));
    }

};

TEST_F(ProducerTestsMockedAwait, noConsumingWhenNotWantedMore) {
    EXPECT_CALL(*wantsMoreMock, suspend(_)).Times(0);
    EXPECT_CALL(consumerMock, consume(1)).WillOnce(Return(doesNotWantMore));

    producer(consumerMock);
}

TEST_F(ProducerTestsMockedAwait, consumeOneForOneSizedSequence) {
    EXPECT_CALL(*wantsMoreMock, suspend(_)).Times(1);

    InSequence testOrder;
    EXPECT_CALL(consumerMock, consume(1)).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume(2)).WillOnce(Return(doesNotWantMore));

    producer(consumerMock);
}

TEST_F(ProducerTestsMockedAwait, consumeFullSequenceForSixteenElements) {
    EXPECT_CALL(*wantsMoreMock, suspend(_)).Times(16);

    InSequence testOrder;
    EXPECT_CALL(consumerMock, consume(1)).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume(2)).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume("fizz")).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume(4)).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume("buzz")).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume("fizz")).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume(7)).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume(8)).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume("fizz")).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume("buzz")).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume(11)).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume("fizz")).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume(13)).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume(14)).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume("fizzbuzz")).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume(16)).WillOnce(Return(wantsMore));
    EXPECT_CALL(consumerMock, consume(17)).WillOnce(Return(doesNotWantMore));

    producer(consumerMock);
}

}