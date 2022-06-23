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
#include "fizzbuzz_tests.hpp"
#include "fizzbuzz_awaiter_mock.hpp"

using namespace testing;

namespace FizzBuzz::MockedAwait
{

class ConsumerMock : public Consumer {
public:
    MOCK_METHOD(Awaiter<bool>, consume, (int value), (override));
    MOCK_METHOD(Awaiter<bool>, consume, (std::string_view value), (override));
};

struct ProducerTestsMockedAwait : Test {
    StrictMock<ConsumerMock> consumerMock;
    std::shared_ptr<AwaiterStrategyMock<bool>> wantsMoreMock
        = std::make_shared<NiceMock<AwaiterStrategyMock<bool>>>();
    std::shared_ptr<AwaiterStrategyMock<bool>> doesNotWantMoreMock
        = std::make_shared<NiceMock<AwaiterStrategyMock<bool>>>();

    Awaiter<bool> wantsMore{wantsMoreMock};
    Awaiter<bool> doesNotWantMore{doesNotWantMoreMock};

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

struct ProducerTestsNotMockedAwait : TestWithSequence {
    std::stringstream actualStream;
    StreamConsumerUpToLimit consumer{actualStream, GetParam().size};
};

TEST_P(ProducerTestsNotMockedAwait, testExpectedSequence) {
    auto gen = producer(consumer);
    EXPECT_EQ(actualStream.str(), GetParam().sequence);
}

INSTANTIATE_TEST_SUITE_P(some, ProducerTestsNotMockedAwait, ValuesIn(Test::someSequences));

}