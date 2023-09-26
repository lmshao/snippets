//
// Copyright (c) 2023 Liming SHAO <lmshao@163.com>. All rights reserved.
//

#ifndef INTERVAL_TIMER_H
#define INTERVAL_TIMER_H

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

class IntervalTimer {
public:
    IntervalTimer() = default;
    virtual ~IntervalTimer();

    void AddTask(int interval, std::function<void()> task, std::string taskName = "");
    void CancelTask();

private:
    void MainLoop();

    // for testing
    std::string GetTime();

private:
    int interval_ = 0;
    std::string taskName_;
    std::function<void()> task_;

    std::mutex mutex_;
    std::condition_variable signal_;

    bool running_ = false;
    std::unique_ptr<std::thread> thread_;
};

#endif // INTERVAL_TIMER_H
