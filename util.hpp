#pragma once

#include <memory>
#include <mutex>
#include <functional>


template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


template <typename T, typename Lock = std::mutex>
class Synchronized {
public:
    template <typename F>
    void enter(F&& handler) {
        std::lock_guard<Lock> lkGuard(lock_);
        handler(data_);
    }
private:
    Lock lock_;
    T data_;
};
