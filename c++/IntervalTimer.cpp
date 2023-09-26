//
// Copyright (c) 2023 Liming SHAO <lmshao@163.com>. All rights reserved.
//

#include "IntervalTimer.h"

IntervalTimer::~IntervalTimer() {
    printf("[%s] %s\n", GetTime().c_str(), __PRETTY_FUNCTION__);
    if (running_) {
        running_ = false;
        signal_.notify_all();
        thread_->join();
    }
}

void IntervalTimer::AddTask(int interval, std::function<void()> task, std::string taskName) {
    printf("[%s] %s\n", GetTime().c_str(), __PRETTY_FUNCTION__);
    if (interval <= 0 || task == nullptr) {
        return;
    }

    if (running_) {
        CancelTask();
    }
    running_ = true;
    interval_ = interval;
    task_ = std::move(task);
    taskName_ = std::move(taskName);

    thread_.reset(new std::thread(&IntervalTimer::MainLoop, this));
}

void IntervalTimer::CancelTask() {
    printf("[%s] %s\n", GetTime().c_str(), __PRETTY_FUNCTION__);
    if (running_) {
        running_ = false;
        signal_.notify_all();
        thread_->join();
    }
}

void IntervalTimer::MainLoop() {
    std::chrono::system_clock::time_point time = std::chrono::system_clock::now();
    while (running_) {
        time += std::chrono::milliseconds(interval_);
        std::unique_lock<std::mutex> lock(mutex_);
        signal_.wait_until(lock, time);
        if (running_ && task_) {
            printf("[%s] invoke task(%s) callback\n", GetTime().c_str(), taskName_.c_str());
            task_();
        }
        printf("[%s] end\n", GetTime().c_str());
    }
}

#include <iomanip>
#include <sstream>
#include <sys/time.h>
std::string IntervalTimer::GetTime() {
    struct timeval tv {};
    gettimeofday(&tv, nullptr);
    std::stringstream ss;
    ss << tv.tv_sec << "." << std::setfill('0') << std::setw(3) << tv.tv_usec / 1000 << " " << std::setfill('0')
       << std::setw(3) << tv.tv_usec % 1000;
    return ss.str();
}