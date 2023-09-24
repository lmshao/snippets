//
// Copyright (c) 2023 Liming SHAO <lmshao@163.com>. All rights reserved.
//

#include "TimeoutTimer.h"

TimeoutTimer::TimeoutTimer() {
    printf("[%s] %s\n", GetTime().c_str(), __PRETTY_FUNCTION__);
    thread_ = std::make_unique<std::thread>(&TimeoutTimer::MainLoop, this);
    pthread_setname_np(thread_->native_handle(), "TimeoutTimer");
}

TimeoutTimer::~TimeoutTimer() {
    printf("[%s] %s\n", GetTime().c_str(), __PRETTY_FUNCTION__);
    if (state_ == State::WAITING) {
        state_ = State::EXITED;
        taskSignal_.notify_all();
    } else if (state_ == State::WORKING) {
        state_ = State::EXITED;
        cancelSignal_.notify_all();
    }

    thread_->join();
}

void TimeoutTimer::MainLoop() {
    while (state_ != State::EXITED) {
        printf("[%s] -\n", GetTime().c_str());
        std::unique_lock<std::mutex> taskLock(taskMutex_);
        state_ = State::WAITING;
        waitSignal_.notify_all();
        taskSignal_.wait(taskLock);
        printf("[%s] wakeup\n", GetTime().c_str());
        if (state_ == State::EXITED) {
            printf("[%s] exit\n", GetTime().c_str());
            break;
        }

        std::unique_lock<std::mutex> cancelLock(cancelMutex_);
        printf("[%s] start task (%s)\n", GetTime().c_str(), taskName_.c_str());
        state_ = State::WORKING;
        cancelSignal_.wait_for(cancelLock, std::chrono::milliseconds(timeout_));
        printf("[%s] wakeup 2\n", GetTime().c_str());
        if (state_ == State::WORKING && callback_) {
            callback_();
        }
    }
    printf("[%s] thread exit\n", GetTime().c_str());
}

void TimeoutTimer::AddTask(int timeout, std::string name, std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(operateMutex_);
    printf("[%s] %s (%s)\n", GetTime().c_str(), __PRETTY_FUNCTION__, name.c_str());
    timeout_ = timeout;
    taskName_ = std::move(name);
    if (callback) {
        callback_ = std::move(callback);
    }

    if (state_ == State::WORKING) {
        state_ = State::CANCELLED;
        printf("[%s] %s notify cancel\n", GetTime().c_str(), __PRETTY_FUNCTION__);
        cancelSignal_.notify_all();

        std::unique_lock<std::mutex> waitLock(waitMutex_);
        waitSignal_.wait(waitLock);
    }
    printf("[%s] %s notify task\n", GetTime().c_str(), __PRETTY_FUNCTION__);
    taskSignal_.notify_all();
}

void TimeoutTimer::CancelTask() {
    std::lock_guard<std::mutex> lock(operateMutex_);
    printf("[%s] %s\n", GetTime().c_str(), __PRETTY_FUNCTION__);

    if (state_ == State::WORKING) {
        state_ = State::CANCELLED;
        printf("[%s] %s notify cancel\n", GetTime().c_str(), __PRETTY_FUNCTION__);
        cancelSignal_.notify_all();
        std::unique_lock<std::mutex> waitLock(waitMutex_);
        waitSignal_.wait(waitLock);
    }
}

#include <iomanip>
#include <sstream>
#include <sys/time.h>
std::string TimeoutTimer::GetTime() {
    struct timeval tv {};
    gettimeofday(&tv, nullptr);
    std::stringstream ss;
    ss << tv.tv_sec << "." << std::setfill('0') << std::setw(3) << tv.tv_usec / 1000 << " " << std::setfill('0')
       << std::setw(3) << tv.tv_usec % 1000;
    return ss.str();
}