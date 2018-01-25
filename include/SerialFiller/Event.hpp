///
/// \file 				Event.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-08-23
/// \last-modified		2018-01-25
/// \brief 				Contains the Event class.
/// \details
///		See README.md in root dir for more info.

#ifndef MN_SERIAL_FILLER_EVENT_H_
#define MN_SERIAL_FILLER_EVENT_H_

// System includes
#include <string>
#include <functional>

namespace mn {
    namespace SerialFiller {

        template<typename T>
        class Event {

            using FuncType = T;

        public:

            /// \brief      Attach a listener to this event. Will be called when Event::Fire() is called.
            void AddListener(std::function<FuncType> listener) {
                listeners_.push_back(listener);
            };

            void RemoveListener(uint32_t index) {

                if(index >= listeners_.size())
                    throw std::invalid_argument("RemoveListener() called with index >= size of listeners_ array.");

                listeners_.erase(listeners_.begin() + index);
            }

            /// \brief      Remove all listeners attached to this event.
            void RemoveAllListeners() {
                listeners_.clear();
            }

            template <class ...Arg>
            void Fire(Arg... parameters) {
                for(auto listener : listeners_) {
                    // Perfect forward the parameters to the listener
                    listener(std::forward<Arg>(parameters)...);
                }
            }

        private:

            /// \brief      Keeps track of all the attached listeners.
            std::vector<std::function<T>> listeners_;

        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_EVENT_H_