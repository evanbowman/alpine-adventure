#pragma once
#include <thread>

class ThreadGuard {
public:
    template<typename Function, typename ...Args>
    explicit ThreadGuard(Function && f, Args && ...args) :
	thread_(std::forward<Function>(f), std::forward<Args>(args)...) {}
    const std::thread & get() const { return thread_; }
    std::thread && release() {
	return std::move(thread_);
    }
    ~ThreadGuard() { if (thread_.joinable()) thread_.join(); }

private:
    std::thread thread_;
};
