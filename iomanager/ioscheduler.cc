#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>

#include "ioscheduler.h"

namespace moczkrin

{
    int IOManager::addEvent(int fd, Event event, std::function<void()> cb)
    {
        FdContext *fd_ptr;
        std::shared_lock<std::shared_mutex> read_lock(m_mutex);

        if (m_fdContexts.size() > fd)
        {
            fd_ptr = m_fdContexts[fd];
            read_lock.unlock();
        }
        else
        {
            read_lock.unlock();
            std::unique_lock<std::shared_mutex> write_lock(m_mutex);
            m_fdContexts.resize(fd * 2);
            for (size_t i = 0; i < fd * 2; i++)
            {
                if (m_fdContexts[i] == nullptr)
                {
                    m_fdContexts[i] = new FdContext();
                    m_fdContexts[i]->fd = i;
                }
            }

            fd_ptr = m_fdContexts[fd];
            // write_lock.unlock();
        }

        std::unique_lock<std::shared_mutex> write_lock(m_mutex);

        if (fd_ptr->events & event)
        {
            // fd event 不为空，且添加进来的事务和原先重复
            return -1;
        }

        epoll_event epevent;
        // 内核事件注册修改
        int op = fd_ptr ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
        epevent.events = EPOLLET | fd_ptr->events | event;
        epevent.data.ptr = fd_ptr;
        // 用户事件修改
        fd_ptr->events = (Event)(fd_ptr->events & ~event);

        // register and check
        // if return greater than 0, means fail.
        if (epoll_ctl(m_epfd, op, fd, &epevent))
        {
            std::cerr << "addEvent::epoll_ctl failed: " << strerror(errno) << std::endl;
            return -1;
        }

        m_pendingEventsCount++;

        FdContext::EventContext &event_ctx = fd_ptr->getEventContext(event);
        // assert(!event_ctx.scheduler && !event_ctx.fiber && !event_ctx.cb);
        event_ctx.scheduler = Scheduler::GetThis();
        if (cb)
        {
            event_ctx.cb.swap(cb);
        }
        else
        {
            event_ctx.fiber_ptr = Fiber::GetThis();
            assert(event_ctx.fiber_ptr->getState() == Fiber::RUNNING);
        }
    }

    bool IOManager::delEvent(int fd, Event event)
    {
        FdContext *fd_ptr = nullptr;
        std::shared_lock<std::shared_mutex> read_lock(m_mutex);
        if (m_fdContexts.size() > fd)
        {
            fd_ptr = m_fdContexts[fd];
            read_lock.unlock();
        }
        else
        {
            read_lock.unlock();
            return false;
        }

        bool exist = fd_ptr->events & event;
        if (!exist)
        {
            return false;
        }

        fd_ptr->events = (Event)(fd_ptr->events & ~event);

        epoll_event epevent;
        int op = fd_ptr->events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
        epevent.events = EPOLLET | fd_ptr->events;
        epevent.data.ptr = fd_ptr;

        if (epoll_ctl(m_epfd, op, fd, &epevent))
        {
            std::cerr << "delEvent::epoll_ctl failed: " << strerror(errno) << std::endl;
            return -1;
        }

        m_pendingEventsCount --;

        FdContext::EventContext& event_ctx = fd_ptr->getEventContext(event);
        fd_ptr->resetEventContext(event_ctx);
        return true;
    }
    //         // delete the event and trigger its callback
    //         bool cancelEvent(int fd, Event event);
    //         // delete all events and trigger its callback
    //         bool cancelAll(int fd);

    //         static IOManager *GetThis();

    //     protected:
    //         void tickle() override;

    //         bool stopping() override;

    //         void idle() override;

    //         void onTimerInsertedAtFront() override;

    //         void contextResize(size_t size);

}