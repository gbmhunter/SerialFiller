
#include "SerialFiller/CobsTranscoder.hpp"

namespace mn {

    void CobsTranscoder::Encode(
            const std::string &rawData,
            std::string &encodedData) {

        int startOfCurrBlock = 0;
        uint8_t numElementsInCurrBlock = 0;

        auto it = rawData.begin();

        // Create space for first (this will be
        // overwritten once count to next 0x00 is known)
        encodedData.push_back(0x00);

        while (it != rawData.end()) {

            if (*it == 0x00) {
                // Save the number of elements before the next 0x00 into
                // the output
                encodedData[startOfCurrBlock] = (uint8_t) (numElementsInCurrBlock + 1);

                // Add placeholder at start of next block
                encodedData.push_back(0x00);

                startOfCurrBlock = encodedData.size() - 1;

                // Reset count of num. elements in current block
                numElementsInCurrBlock = 0;

            } else {
                encodedData.push_back(*it);
                numElementsInCurrBlock++;
            }
            it++;
        }

        // Finish the last block
        // Insert pointer to the terminating 0x00 character
        encodedData[startOfCurrBlock] = numElementsInCurrBlock + 1;
        encodedData.push_back(0x00);
    }


    CobsTranscoder::DecodeStatus
    CobsTranscoder::Decode(
            const std::string &encodedData,
            std::string &decodedData) {

        decodedData.clear();

        int encodedDataPos = 0;

        while (encodedDataPos < encodedData.size()) {

            int numElementsInBlock = encodedData[encodedDataPos] - 1;
            encodedDataPos++;

            // Copy across all bytes within block
            for (int i = 0; i < numElementsInBlock; i++) {
                uint8_t byteOfData = encodedData[encodedDataPos];
                if (byteOfData == 0x00) {
                    decodedData.clear();
                    return DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED;
                }

                decodedData.push_back(encodedData[encodedDataPos]);
                encodedDataPos++;
            }

            if (encodedData[encodedDataPos] == 0x00) {
                // End of packet found!
                break;
            }

            decodedData.push_back(0x00);
        }

        return DecodeStatus::SUCCESS;
    }
}