///
/// \file 				String.hpp
/// \author 			Geoffrey Hunter (www.mbedded.ninja) <gbmhunter@gmail.com>
/// \edited             n/a
/// \created			2017-08-11
/// \last-modified		2017-09-28
/// \brief 				Contains the String class.
/// \details
///		See README.md in root dir for more info.

#ifndef MN_CPP_UTILS_STRING_H_
#define MN_CPP_UTILS_STRING_H_

// System includes
#include <cstdint>
#include <iomanip>
#include <sstream>

namespace mn {
    namespace CppUtils {

        /// \brief      Contains static methods for converting various objects to strings.
        class String {

        public:

            static std::string ToHex(uint32_t hexVal, uint8_t numHexChars) {
                std::stringstream output;
                output << "0x" << std::hex << std::uppercase << std::setw(numHexChars) << std::setfill('0') << hexVal;
                return output.str();
            }

            template<typename T>
            static std::string ToHex(T iterable) {

                // Special formatting when there are no values
                if(iterable.begin() == iterable.end())
                    return "{}";

                std::stringstream stream;
                stream << "{ ";
                for(auto it = iterable.begin(); it != iterable.end(); ++it) {
                    stream << "0x"
                           << std::setfill('0') << std::setw(sizeof(typename T::value_type) * 2)
                           << std::uppercase << std::hex << (int)(*it);
                    if(it != iterable.end() - 1)
                        stream << ", ";
                }
                stream << " }";
                return stream.str();
            }

            template<typename T>
            static std::string ToAscii(T iterable) {

                // Special formatting when there are no values
                if(iterable.begin() == iterable.end())
                    return "{}";

                std::stringstream stream;
                stream << "{ ";
                for(auto it = iterable.begin(); it != iterable.end(); ++it) {
                    stream << "'" << static_cast<char>(*it) << "'";
                    if(it != iterable.end() - 1)
                        stream << ", ";
                }
                stream << " }";
                return stream.str();
            }



        };
    } // namespace CppUtils
} // namespace mn


#endif // #ifndef MN_CPP_UTILS_STRING_H_