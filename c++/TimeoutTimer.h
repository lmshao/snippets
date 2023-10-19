//
// Copyright (c) 2023 Liming SHAO <lmshao@163.com>. All rights reserved.
//

#ifndef _TIMEOUT_TIMER_H
#define _TIMEOUT_TIMER_H

#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

class TimeoutTimer {
public:
    explicit TimeoutTimer(const std::string &info = "TimeoutTimer");
    virtual ~TimeoutTimer();

    void SetTimeoutCallback(std::function<void()> callback) { callback_ = std::move(callback); }

    uint64_t AddTask(int timeout, std::string name = "none", const std::function<void()> &callback = nullptr);
    void CancelTask(uint64_t taskId = ~0llu /*cancel all tasks*/);

private:
    void MainLoop();

    // for testing
    static std::string GetTime();

private:
    enum State {
        INIT,
        WAITING,
        WORKING,
        EXITED
    };

    State state_ = INIT;

    std::mutex operateMutex_;
    std::mutex taskMutex_;
    std::condition_variable taskSignal_;

    std::mutex updateMutex_;
    std::condition_variable updateSignal_;

    std::function<void()> callback_;
    std::unique_ptr<std::thread> thread_;

    using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

    struct Task {
        uint64_t id;
        std::string name;
        std::function<void()> callback;
    };

    std::map<TimePoint, Task> tasks_;
};

#endif // _TIMEOUT_TIMER_H
