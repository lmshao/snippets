//
// Copyright (c) 2023 Liming SHAO <lmshao@163.com>. All rights reserved.
//

#ifndef _TIMEOUT_TIMER_H
#define _TIMEOUT_TIMER_H

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

class TimeoutTimer {
public:
    TimeoutTimer();
    virtual ~TimeoutTimer();

    void SetTimeoutCallback(std::function<void()> callback) { callback_ = std::move(callback); }

    void AddTask(int timeout, std::string name = "none", std::function<void()> callback = nullptr);
    void CancelTask();

private:
    void MainLoop();

    // for testing
    std::string GetTime();

private:
    enum State {
        INIT,
        WAITING,
        WORKING,
        CANCELLED,
        EXITED
    };

    int timeout_ = 0;
    State state_ = INIT;
    std::string taskName_;

    std::mutex operateMutex_;
    std::mutex taskMutex_;
    std::condition_variable taskSignal_;

    std::mutex cancelMutex_;
    std::condition_variable cancelSignal_;

    std::mutex waitMutex_;
    std::condition_variable waitSignal_;

    std::function<void()> callback_;
    std::unique_ptr<std::thread> thread_;
};

#endif // _TIMEOUT_TIMER_H
