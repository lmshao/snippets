//
// Copyright (c) 2023 Liming SHAO <lmshao@163.com>. All rights reserved.
//

#include "TimeoutTimer.h"

TimeoutTimer::TimeoutTimer(const std::string &info) {
    printf("[%s] %s\n", GetTime().c_str(), __PRETTY_FUNCTION__);
    thread_ = std::make_unique<std::thread>(&TimeoutTimer::MainLoop, this);
    pthread_setname_np(thread_->native_handle(), info.c_str());
}

TimeoutTimer::~TimeoutTimer() {
    printf("[%s] %s\n", GetTime().c_str(), __PRETTY_FUNCTION__);
    tasks_.clear();

    if (state_ == State::WAITING) {
        state_ = State::EXITED;
        taskSignal_.notify_all();
    } else if (state_ == State::WORKING) {
        state_ = State::EXITED;
        updateSignal_.notify_all();
    }

    thread_->join();
}

void TimeoutTimer::MainLoop() {
    while (state_ != State::EXITED) {
        printf("[%s] loop\n", GetTime().c_str());
        if (tasks_.empty()) {
            std::unique_lock<std::mutex> taskLock(taskMutex_);
            state_ = State::WAITING;
            printf("[%s] - wait task\n", GetTime().c_str());
            taskSignal_.wait(taskLock);
            printf("[%s] waiter wakeup\n", GetTime().c_str());
            if (state_ == State::EXITED) {
                printf("[%s] exit\n", GetTime().c_str());
                break;
            }
        }

        if (tasks_.empty()) {
            printf("[%s] no task, continue\n", GetTime().c_str());
            continue;
        }

        // Latest task
        auto task = *tasks_.begin();

        std::unique_lock<std::mutex> uniqueLock(updateMutex_);
        printf("[%s] start task (%s) (%lu)\n", GetTime().c_str(), task.second.name.c_str(),
               task.first.time_since_epoch().count());
        state_ = State::WORKING;
        std::cv_status status = updateSignal_.wait_until(uniqueLock, task.first);
        printf("[%s] update waiter wakeup\n", GetTime().c_str());
        if (status == std::cv_status::timeout) {
            if (state_ == State::WORKING && task.second.callback) {
                task.second.callback();
                tasks_.erase(task.first);
            }
        } else {
            printf("[%s] cancel|replace task : %s\n", GetTime().c_str(), task.second.name.c_str());
        }

        printf("tasks num: %zu\n", tasks_.size());
    }
    printf("[%s] thread exit\n", GetTime().c_str());
}

uint64_t TimeoutTimer::AddTask(int timeout, std::string name, const std::function<void()> &callback) {
    if (timeout <= 0) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(operateMutex_);
    printf("[%s] %s (%s)\n", GetTime().c_str(), __PRETTY_FUNCTION__, name.c_str());

    Task task;
    TimePoint now = std::chrono::system_clock::now();
    task.id = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    task.name = std::move(name);
    task.callback = callback ? callback : callback_;
    TimePoint triggerTime = now + std::chrono::milliseconds(timeout);

    printf("Now: %lu\nTrigger: %lu\n", now.time_since_epoch().count(), triggerTime.time_since_epoch().count());
    // 当前没有任务，直接添加任务，并通知
    if (tasks_.empty()) {
        tasks_.emplace(triggerTime, task);
        printf("[%s] %s notify task\n", GetTime().c_str(), __PRETTY_FUNCTION__);
        taskSignal_.notify_all();
    } else {
        auto workingTask = *tasks_.begin();
        tasks_.emplace(triggerTime, task);

        // 如果当前任务触发时间在 进行中任务的触发时间 之前，更新此任务为进行中任务
        if (workingTask.first > triggerTime) {
            updateSignal_.notify_all();
        }
    }

    return task.id;
}

void TimeoutTimer::CancelTask(uint64_t taskId) {
    std::lock_guard<std::mutex> lock(operateMutex_);
    printf("[%s] %s\n", GetTime().c_str(), __PRETTY_FUNCTION__);

    if (tasks_.empty()) {
        return;
    }

    auto workingTask = *tasks_.begin();
    if (taskId == ~0llu) {
        printf("[%s] %s cancel all tasks\n", GetTime().c_str(), __PRETTY_FUNCTION__);
        tasks_.clear();
    } else {
        auto it = std::find_if(tasks_.begin(), tasks_.end(),
                               [&](const std::pair<TimePoint, Task> &task) { return task.second.id == taskId; });
        if (it == tasks_.end()) {
            printf("[%s] %s invalid taskId(%lu)\n", GetTime().c_str(), __PRETTY_FUNCTION__, taskId);
            return;
        } else {
            tasks_.erase(it);
            printf("[%s] %s erase taskId(%lu)\n", GetTime().c_str(), __PRETTY_FUNCTION__, taskId);
        }
    }

    if (taskId == ~0 || workingTask.second.id == taskId) {
        printf("[%s] %s notify cancel\n", GetTime().c_str(), __PRETTY_FUNCTION__);
        updateSignal_.notify_all();
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