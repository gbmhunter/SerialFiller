

// System includes
#include <cstdint>
#include <iomanip>
#include <sstream>

class CrcCheckFailed : public std::runtime_error {
public:

    template< typename T >
    std::string IntToHex(T value)
    {
        std::stringstream stream;
        stream << "0x"
               << std::setfill ('0') << std::setw(sizeof(T)*2)
               << std::hex << value;
        return stream.str();
    }

    CrcCheckFailed(uint16_t calculatedCrcVal, uint16_t sentCrcVal) :
            runtime_error("CRC check failed. Calculated CRC value = " + IntToHex(calculatedCrcVal) + ", sent CRC value = " + IntToHex(sentCrcVal) + ".") {
        calculatedCrcVal_ = calculatedCrcVal;
        sentCrcVal_ = sentCrcVal;
    };

    uint16_t calculatedCrcVal_;
    uint16_t sentCrcVal_;
};