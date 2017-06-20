
#include <stdint-gcc.h>
#include <string>

#include "SerialFiller/Crc16Ccitt1021.hpp"

namespace mn {
    namespace SerialFiller {

        uint16_t Crc16Ccitt1021::Calc(ByteArray data) {
            uint8_t x;
            uint16_t crc = 0xFFFF;

            // This calculates the CRC using an efficient algorithm
            // as shown online
            for (auto it = data.begin(); it != data.end(); ++it) {
                x = (uint8_t) (crc >> 8 ^ *it);
                x ^= x >> 4;
                crc = (crc << 8) ^ ((uint16_t) (x << 12)) ^ ((uint16_t) (x << 5)) ^ ((uint16_t) x);
            }
            return crc;
        }
    } // namespace SerialFiller
} // namespace mn