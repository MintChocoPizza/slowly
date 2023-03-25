#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace ThreadPool 
{
    class ThreadPool
    {
        public:
            ThreadPool(size_t num_threads);
            ~ThreadPool();

            // job을 추가한다.
            void EnqueueJob(std::function<void()> job);

        private:
            // 총 Worker 쓰레드의 개수
            size_t num_threads_;




            // 모든 쓰레드 종료
            bool stop_all
    }

    ThreadPool::ThreadPool(size_t num_threads)
        : num_threads_(num_threads), stop_all(false)
    {

    }
}
