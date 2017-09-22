///
/// \file 				ThreadSafeQ.hpp
/// \author 			Geoffrey Hunter (www.mbedded.ninja) <gbmhunter@gmail.com>
/// \edited             n/a
/// \created			2017-08-09
/// \last-modified		2017-08-14
/// \brief 				Contains the ThreadSafeQ class.
/// \details
///		See README.md in root dir for more info.

#ifndef MN_CPP_UTILS_THREAD_SAFE_Q_H_
#define MN_CPP_UTILS_THREAD_SAFE_Q_H_

// System includes
#include <queue>
#include <mutex>
#include <condition_variable>



namespace mn {
    namespace CppUtils {

        /// \brief       A thread-safe queue designed for inter-thread communication.
        template<typename T>
        class ThreadSafeQ {
        public:

            /// \brief      Adds something to the back of the thread-safe queue.
            /// \details    This may be called from multiple threads at the "same time". Method
            ///             will block until item can be placed onto queue.
            void Push(const T &item) {
                std::unique_lock<std::mutex> uniqueLock(mutex_);

                // Push item onto queue
                queue_.push(item);

                // IMPORTANT: This has to be done BEFORE conditional variable is notified
                uniqueLock.unlock();

                // IMPORTANT: This has to be done AFTER mutex has been unlocked
                conditionVariable_.notify_one();
            }

            /// \brief      Waits indefinitely until an item is available on the queue. Removes one item.
            /// \details    This may be called from multiple threads at the "same time". Method
            ///             will block indefinitely (no timeout) until there is an item on the queue.
            void Pop(T& item) {

                // Lock the mutex
                std::unique_lock<std::mutex> uniqueLock(mutex_);

                conditionVariable_.wait(uniqueLock, [&] {
                    return !queue_.empty();
                });

                // If we get here, there is an item on the queue for us, and the lock has been taken out
                item = queue_.front();
                queue_.pop();

                // Mutex will automatically be unlocked here
                // (as it is a unique_lock)
            }

            /// \brief      Removes one item from the front of the thread-safe queue.
            /// \details    This may be called from multiple threads at the "same time". Method
            ///             will block until the is an item on the queue OR a timeout occurs.
            /// \returns    Returns true is item received, returns false if a timeout occurred.
            bool TryPop(T& item, const std::chrono::milliseconds& timeout) {

                // Lock the mutex
                std::unique_lock<std::mutex> uniqueLock(mutex_);

                if(!conditionVariable_.wait_for(uniqueLock, timeout, [&] {
                    return !queue_.empty();
                })) {
                    return false;
                }

                // If we get here, there is an item on the queue for us, and the lock has been taken out
                item = queue_.front();
                queue_.pop();

                // Mutex will automatically be unlocked here
                // (as it is a unique_lock)
                return true;
            }

            size_t Size() {
                return queue_.size();
            }

        private:

            std::queue<T> queue_;
            std::mutex mutex_;
            std::condition_variable conditionVariable_;

        };
    } // namespace CppUtils
} // namespace mn

#endif // #ifndef MN_CPP_UTILS_THREAD_SAFE_Q_H_