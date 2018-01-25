///
/// \file 				Definitions.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2018-01-25
/// \last-modified		2018-01-25
/// \brief 				Contains definitions that are used in various serial filler modules.
/// \details
///		See README.md in root dir for more info.

#ifndef MN_SERIAL_FILLER_DEFINITIONS_H_
#define MN_SERIAL_FILLER_DEFINITIONS_H_

// System includes
#include <cstdint>
#include <memory>
#include <queue>
#include <vector>

namespace mn {
    namespace SerialFiller {
        using ByteArray = std::vector<uint8_t>;
        using ByteQueue = std::deque<uint8_t>;
    }
}

#endif // #ifndef MN_SERIAL_FILLER_DEFINITIONS_H_