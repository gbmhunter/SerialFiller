///
/// \file 				NoTopicDataSeparator.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-20
/// \last-modified		2017-06-20
/// \brief 				Contains the NoTopicDataSeparator exception.
/// \details
///		See README.rst in root dir for more info.

#ifndef MN_SERIAL_FILLER_NO_TOPIC_DATA_SEPARATOR_H_
#define MN_SERIAL_FILLER_NO_TOPIC_DATA_SEPARATOR_H_

// System includes


// Forward declaration
namespace mn {
    namespace SerialFiller {
        class NoTopicDataSeparator;
    }
}

// User includes

namespace mn {
    namespace SerialFiller {

        class NoTopicDataSeparator : public std::runtime_error {
        public:

            NoTopicDataSeparator(ByteArray packet) :
                    runtime_error("No topic/data separator ':' was found in packet.") {
                packet_ = packet;
            };

            ByteArray packet_;
        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_NO_TOPIC_DATA_SEPARATOR_H_
