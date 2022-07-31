#pragma once
#include <queue>
#include <thread>
#include <mutex>    // pthread_mutex_t
#include <condition_variable>   // pthread_condition_t

// 异步写日志的日志队列

template<typename T>
class LockQueue
{
public:
    // 入队 多个worker线程都会写日志queue
    void Push(const T& data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_condvariable.notify_one();
    }

    // 出队 一个线程读日志queue
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty())
        {
            // 日志队列为空，线程进入wait状态，并释放锁
            m_condvariable.wait(lock);
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;

};