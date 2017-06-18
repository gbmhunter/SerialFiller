#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace mn;

namespace {

    class CallbackTests : public ::testing::Test {
    protected:

        CallbackTests() {
        }

        virtual ~CallbackTests() {
        }
    };

    TEST_F(CallbackTests, CallbackTest1) {


        SerialFiller serialFiller;

        ByteArray savedTxData;
        serialFiller.txDataReady_ = [&](ByteArray txData) -> void {
            savedTxData = txData;
        };

        serialFiller.Publish("test-topic", ByteArray({ 'h', 'e', 'l', 'l', 'o' }));

        EXPECT_NE(0, savedTxData.size());



    }

}  // namespace