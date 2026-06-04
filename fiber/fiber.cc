#include "fiber.h"
#include <atomic>

namespace moczkrin
{
    static thread_local Fiber *t_running_fiber = nullptr;
    static thread_local std::shared_ptr<Fiber> t_main_fiber = nullptr;
    static thread_local Fiber *t_scheduler_fiber = nullptr;
    static std::atomic<uint64_t> t_fiber_id = {0};
    static std::atomic<uint64_t> t_fiber_count = {0};
    Fiber::Fiber()
    {
        SetThis(this);
        _state = READY;
        if (getcontext(&_ctx))
        {
            std::cerr << "Fiber() failed\n";
            pthread_exit(NULL);
        }
        m_id = t_fiber_id++;
        t_fiber_count++;
    }

    Fiber::Fiber(std::function<void()> cb, uint32_t stacksize, bool runInScheduler) : _cb(cb), _runInScheduler(runInScheduler)
    {
        _state = READY;
        _stacksize = stacksize ? stacksize : 128000;
        _stack = malloc(_stacksize);
        if (getcontext(&_ctx))
        {
            std::cerr << "Fiber(std::function<void()> cb, size_t stacksize, bool run_in_scheduler) failed\n";
            pthread_exit(NULL);
        }

        _ctx.uc_link = nullptr;
        _ctx.uc_stack.ss_size = _stacksize;
        _ctx.uc_stack.ss_sp = _stack;
        makecontext(&_ctx, &MainFunc, 0);
        m_id = t_fiber_id++;
        t_fiber_count++;
    }
    Fiber::~Fiber()
    {
        t_fiber_count--;
        if (_stack)
        {
            free(_stack);
        }
    }

    void Fiber::reset(std::function<void()> cb)
    {
        assert(_state == TERM);
        _state = READY;
        _cb = cb;
        getcontext(&_ctx);
        _ctx.uc_link = nullptr;
        _ctx.uc_stack.ss_size = _stacksize;
        _ctx.uc_stack.ss_sp = _stack;
        makecontext(&_ctx, &MainFunc, 0);
    }
    void Fiber::resume()
    {
        assert(_state == READY);
        if (_runInScheduler)
        {
            SetThis(this);
            if (swapcontext(&(t_scheduler_fiber->_ctx), &_ctx))
            {
                std::cerr << "resume() to t_scheduler_fiber failed\n";
                pthread_exit(NULL);
            }
        }
        else
        {
            SetThis(this);
            if (swapcontext(&(t_main_fiber->_ctx), &_ctx))
            {
                std::cerr << "resume() to t_main_fiber failed\n";
                pthread_exit(NULL);
            }
        }
    }
    void Fiber::yield()
    {
        assert(_state == TERM || _state == RUNNING);
        if (_runInScheduler)
        {
            SetThis(this);
            if (swapcontext(&_ctx, &(t_scheduler_fiber->_ctx)))
            {
                std::cerr << "yield() to t_scheduler_fiber failed\n";
                pthread_exit(NULL);
            }
        }
        else
        {
            SetThis(this);
            if (swapcontext(&_ctx, &(t_main_fiber->_ctx)))
            {
                std::cerr << "yield() to t_main_fiber failed\n";
                pthread_exit(NULL);
            }
        }
    }

    // public:
    void Fiber::SetThis(Fiber *fiber)
    {
        t_running_fiber = fiber;
    }
    std::shared_ptr<Fiber> Fiber::GetThis()
    {
        if (t_running_fiber)
        {
            return t_running_fiber->shared_from_this();
        }
        std::shared_ptr<Fiber> ptr(new Fiber());
        t_running_fiber = ptr.get();
        t_main_fiber = ptr;
        t_scheduler_fiber = ptr.get();
        return ptr->shared_from_this();
    }
    void Fiber::SetScheudler(Fiber *f)
    {
        t_scheduler_fiber = f;
    }
    uint64_t Fiber::GetFiberId()
    {
        if (t_fiber_id)
        {
            return t_fiber_id;
        }
        return (uint64_t)-1;
    }
    void Fiber::MainFunc()
    {
        std::shared_ptr<Fiber> curr = GetThis();
        assert(curr != nullptr);
        curr->_cb();
        curr->_cb = nullptr;
        curr->_state = TERM;
        auto ptr = curr.get();
        curr.reset();
        ptr->yield();
    }
}