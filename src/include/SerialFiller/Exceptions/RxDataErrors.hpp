///
/// \file 				RxDataErrors.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-20
/// \last-modified		2017-06-20
/// \brief 				Contains the RxDataErrors class.
/// \details
///		See README.rst in root dir for more info.

// System includes
#include <cstdint>
#include <iomanip>
#include <sstream>

namespace mn {

    class RxDataErrors : public std::runtime_error {
    public:

        RxDataErrors() :
                runtime_error("One or more errors occurred while processing RX data.") {
        };

        std::vector<std::runtime_error> exceptions;
    };
}