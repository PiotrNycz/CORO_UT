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
#include "mocked_fizzbuzz_producer.hpp"
#include "fizzbuzz_tests.hpp"

using namespace testing;

namespace FizzBuzz::Mocked
{

struct ConsumerStrategyMock : ConsumerStrategy {
    MOCK_METHOD(bool, wantsMore, (), (const,override));
    MOCK_METHOD(void, consume, (int), (override));
    MOCK_METHOD(void, consume, (std::string_view), (override));
};

struct ProducerTestsMocked : Test {
    StrictMock<ConsumerStrategyMock> consumerStrategyMock;
    Consumer consumer{consumerStrategyMock};
};

TEST_F(ProducerTestsMocked, noConsumingWhenNotWantedMore) {
    EXPECT_CALL(consumerStrategyMock, wantsMore()).WillRepeatedly(Return(false));
    producer(consumer);
}

TEST_F(ProducerTestsMocked, consumeOneForOneSizedSequence) {
    EXPECT_CALL(consumerStrategyMock, wantsMore()).WillOnce(Return(true)).WillRepeatedly(Return(false));
    InSequence testOrder;
    EXPECT_CALL(consumerStrategyMock, consume(1));
    producer(consumer);
}

TEST_F(ProducerTestsMocked, consumeFullSequenceForSixteenElements) {
    bool wantsMore = true;
    EXPECT_CALL(consumerStrategyMock, wantsMore()).WillRepeatedly([&]{ return wantsMore; });
    InSequence testOrder;
    EXPECT_CALL(consumerStrategyMock, consume(1));
    EXPECT_CALL(consumerStrategyMock, consume(2));
    EXPECT_CALL(consumerStrategyMock, consume("fizz"));
    EXPECT_CALL(consumerStrategyMock, consume(4));
    EXPECT_CALL(consumerStrategyMock, consume("buzz"));
    EXPECT_CALL(consumerStrategyMock, consume("fizz"));
    EXPECT_CALL(consumerStrategyMock, consume(7));
    EXPECT_CALL(consumerStrategyMock, consume(8));
    EXPECT_CALL(consumerStrategyMock, consume("fizz"));
    EXPECT_CALL(consumerStrategyMock, consume("buzz"));
    EXPECT_CALL(consumerStrategyMock, consume(11));
    EXPECT_CALL(consumerStrategyMock, consume("fizz"));
    EXPECT_CALL(consumerStrategyMock, consume(13));
    EXPECT_CALL(consumerStrategyMock, consume(14));
    EXPECT_CALL(consumerStrategyMock, consume("fizzbuzz"));
    EXPECT_CALL(consumerStrategyMock, consume(16)).WillOnce([&]{wantsMore=false;});
    producer(consumer);
}

struct ProducerTestsNotMocked : TestWithSequence {
    std::stringstream actualStream;
    StreamConsumerUpToLimit consumerStrategy{actualStream, GetParam().size};
    Consumer consumer{consumerStrategy};
};

TEST_P(ProducerTestsNotMocked, testExpectedSequence) {
    auto gen = producer(consumer);
    EXPECT_EQ(actualStream.str(), GetParam().sequence);
}

INSTANTIATE_TEST_SUITE_P(some, ProducerTestsNotMocked, ValuesIn(Test::someSequences));

}