#ifndef YVM_CONCURRENT_H
#define YVM_CONCURRENT_H
#include <queue>
#include <future>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>


using namespace std;

class ThreadPool{
public:
    ThreadPool() noexcept;
    ~ThreadPool() noexcept;

    template<typename Func>
    future<void> submit(Func task);
    void storeTaskFuture(shared_future<void> taskFuture) { taskFutures.push_back(taskFuture); }
    vector<shared_future<void>> getTaskFutures() const { return taskFutures; }

private:
    void runPendingWork();

    unsigned defaultThreadCnt;

    vector<shared_future<void>> taskFutures;
    vector<thread> threads;
    queue<packaged_task<void()>> taskQueue;
    mutex taskQueueMtx;
    atomic_bool done{};
};

template<typename Func>
future<void> ThreadPool::submit(Func task) {
    packaged_task<void()> pt(task);
    future<void> f = pt.get_future();
    lock_guard<mutex> lock(taskQueueMtx);
    taskQueue.push(std::move(pt));
    return f;
}

#endif

