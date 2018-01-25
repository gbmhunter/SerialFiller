///
/// \file 				SerialFillerException.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-20
/// \last-modified		2017-06-20
/// \brief 				Contains the SerialFillerException class.
/// \details
///		See README.rst in root dir for more info.

#ifndef MN_SERIAL_FILLER_SERIAL_FILLER_EXCEPTION_H_
#define MN_SERIAL_FILLER_SERIAL_FILLER_EXCEPTION_H_

// System includes
#include <stdexcept>

namespace mn {
    namespace SerialFiller {

        class SerialFillerException : public std::runtime_error {
        public:

            SerialFillerException(std::string errorMsg) : std::runtime_error(errorMsg) {}

        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_SERIAL_FILLER_EXCEPTION_H_
