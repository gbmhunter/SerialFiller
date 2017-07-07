///
/// \file 				CobsTranscoder.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2017-06-20
/// \brief 				Contains the CobsTranscoder class.
/// \details
///		See README.rst in root dir for more info.

// User includes
#include "SerialFiller/CobsTranscoder.hpp"
#include "SerialFiller/Exceptions/CobsDecodingFailed.hpp"

namespace mn {
    namespace SerialFiller {


        void CobsTranscoder::Encode(
                const ByteArray &rawData,
                ByteArray &encodedData) {

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

                    if(numElementsInCurrBlock == 254) {

                        encodedData[startOfCurrBlock] = (uint8_t) (numElementsInCurrBlock + 1);

                        // Add placeholder at start of next block
                        encodedData.push_back(0x00);

                        startOfCurrBlock = encodedData.size() - 1;

                        // Reset count of num. elements in current block
                        numElementsInCurrBlock = 0;
                    }

                }
                it++;
            }

            // Finish the last block
            // Insert pointer to the terminating 0x00 character
            encodedData[startOfCurrBlock] = numElementsInCurrBlock + 1;
            encodedData.push_back(0x00);
        }

        void CobsTranscoder::Decode(
                const ByteArray &encodedData,
                ByteArray &decodedData) {

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
                        throw CobsDecodingFailed(encodedData);
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

        }
    } // namespace SerialFiller
} // namespace mn