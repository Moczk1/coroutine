#pragma once
#include <iostream>
#include <memory>
#include <atomic>
#include <functional>
#include <cassert>
#include <ucontext.h>
#include <unistd.h>
#include <mutex>

namespace moczkrin
{
    class Fiber : public std::enable_shared_from_this<Fiber> 
    {
        enum State
        {
            READY,
            RUNNING,
            TERM
        };

        private:
            Fiber();
        
        public:
            Fiber(std::function<void()> cb, uint32_t stacksize, bool runInScheduler);
            ~Fiber();

            void reset(std::function<void()>cb);
            void resume();
            void yield();

            uint64_t getId() const {return m_id;}
            State getState() const {return _state;}

        public:
            static void SetThis(Fiber* fiber);
            static std::shared_ptr<Fiber> GetThis();
            static void SetScheudler(Fiber* fiber);
            static uint64_t GetFiberId();
            static void MainFunc();

        public: 
            std::mutex _mutex;    
        private: 
            uint64_t m_id = 0;
            State _state = READY;
            ucontext_t _ctx;
            uint32_t _stacksize = 0;
            void* _stack = nullptr;
            std::function<void()> _cb = nullptr;
            bool _runInScheduler = false;
    };

}