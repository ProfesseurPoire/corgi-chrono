#pragma once
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

namespace corgi::chrono
{

/**
 * @brief   Counts down time until it reaches 0 and run the assigned
 *          callback function
 */
struct timer
{
    /**
     * @brief   A timer can be run in async or sync mode.
     */
    enum class mode
    {
        async,
        sync
    };

    /**
     * @brief   Construct a new countdown timer that will count down @p time
     *          until it reaches 0. Once the timer reaches 0, it runs the @p
     *          callback function.
     *
     *          A timer can be sync, meaning it'll block the current thread, or
     *          async, meaning it'll run the timer in another thread
     *
     * @param time      : Time being counted down
     * @param callback  : Function ran when countdown reaches 0
     * @param mode      : async or sync
     *
     */
    timer(int                   time,
          std::function<void()> callback,
          timer::mode           mode = timer::mode::sync)
        : time_(time)
        , callback_(callback)
        , mode_(mode)
    {
    }

    timer() {}

    void set(int                   time,
             std::function<void()> callback,
             timer::mode           mode = timer::mode::sync)
    {
        if(running_)
            stop();

        running_  = false;
        time_     = time;
        callback_ = callback;
        mode_     = mode;
    }

    const static inline int tick = 1;

    /**
     * @brief Starts the constructed timer
     */
    void start()
    {
        running_ = true;
        switch(mode_)
        {
            case timer::mode::sync:

                while(repeat_ > 0 || repeat_ == -1)
                {
                    int  total         = 0;
                    bool keep_sleeping = true;
                    auto start         = std::chrono::system_clock::now();

                    while(keep_sleeping)
                    {
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(tick));

                        auto end    = std::chrono::system_clock::now();
                        auto int_ms = std::chrono::duration_cast<
                            std::chrono::milliseconds>(end - start);
                        if(int_ms.count() > time_)
                            keep_sleeping = false;
                    }

                    callback_();
                    if(repeat_ != -1)
                        repeat_--;
                }
                running_ = false;
                break;
            case timer::mode::async:
                t = std::thread(&timer::async_func, this);
                break;
        }
    }

    /**
     * @brief   Repeat the current timer @p n times.
     *
     *          if n is equals -1 repeat is infinite
     */
    void repeat(int n = -1) { repeat_ = n; }

    void stop()
    {
        if(!running_)
            return;

        stop_mutex_.lock();
        running_ = false;
        stop_mutex_.unlock();
        // To be fair this is debatable, should stop wait for the thread to
        // join?
        t.join();
    }

    ~timer()
    {
        // In case the timer is still running, we explicitly stop it.
        stop();
        if(mode_ == timer::mode::async)
            if(t.joinable())
                t.join();
    }

private:
    void async_func()
    {
        while(repeat_ > 0 || repeat_ == -1)
        {
            int  total         = 0;
            bool keep_sleeping = true;
            auto start         = std::chrono::system_clock::now();

            while(keep_sleeping)
            {
                if(!running_)
                    return;

                std::this_thread::sleep_for(std::chrono::milliseconds(tick));
                auto end = std::chrono::system_clock::now();
                auto int_ms =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        end - start);
                if(int_ms.count() >= time_)
                    keep_sleeping = false;
            }

            callback_();
            if(repeat_ != -1)
                repeat_--;

            if(!running_)
                return;
        }
        running_ = false;
    }
    timer::mode           mode_;
    std::function<void()> callback_;
    int                   time_;
    std::thread           t;
    int                   repeat_ = 1;
    std::mutex            stop_mutex_;
    bool                  running_ = false;
};

}    // namespace corgi::chrono