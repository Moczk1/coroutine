#ifndef _THREAD_H_
#define _THREAD_H_

#include <mutex>
#include <condition_variable>
#include <functional>
#include <string>
#include <thread> // 引入 std::thread

namespace moczkrin
{

    class Semaphore
    {
    private:
        std::mutex mtx;
        std::condition_variable cv;
        int count;

    public:
        explicit Semaphore(int count_ = 0) : count(count_) {}

        void wait()
        {
            std::unique_lock<std::mutex> lock(mtx);
            while (count == 0)
            {
                cv.wait(lock);
            }
            count--;
        }

        void signal()
        {
            std::unique_lock<std::mutex> lock(mtx);
            count++;
            cv.notify_one();
        }
    };

    class Thread
    {
    public:
        Thread(std::function<void()> cb, const std::string &name);
        ~Thread();
        pid_t getId() const { return m_id; }
        const std::string &getName() const { return m_name; }
        void join();

    public:
        static pid_t GetThreadId();
        static Thread *GetThis();
        static const std::string &GetName();
        static void SetName(const std::string &name);

    private:
        static void run(Thread *thread); // 签名简化：直接传入类指针，不再需要 void*

    private:
        pid_t m_id = -1;
        std::thread m_thread; // 替换为 std::thread
        std::function<void()> m_cb;
        std::string m_name;
        Semaphore m_semaphore;
    };

}

#endif