#include <corgi/chrono/timer.h>
#include <corgi/test/test.h>

#include <iostream>

using namespace corgi::chrono;

int main()
{
    corgi::test::add_test(
        "timer", "set",
        []() -> void
        {
            std::string result;
            timer       t(
                300, [&]() { result = "a"; },
                corgi::chrono::timer::mode::async);
            t.start();
            t.set(
                150, [&]() { result = "b"; },
                corgi::chrono::timer::mode::async);
            t.start();

            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            assert_that(result, corgi::test::equals(std::string("b")));
        });

    corgi::test::add_test(
        "timer", "sync",
        []() -> void
        {
            auto        start = std::chrono::system_clock::now();
            std::string result;

            timer t(200, [&]() -> void { result += "a"; });

            t.start();
            auto end    = std::chrono::system_clock::now();
            auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                end - start);
            assert_that(result, corgi::test::equals(std::string("a")));
            assert_that(int_ms.count(), corgi::test::almost_equals(200, 30));
        });

    corgi::test::add_test(
        "timer", "async",
        []() -> void
        {
            auto start = std::chrono::system_clock::now();

            std::chrono::time_point<std::chrono::system_clock> end;
            std::string                                        result;

            timer t(
                200,
                [&]() -> void
                {
                    end = std::chrono::system_clock::now();
                    result += "a";
                },
                timer::mode::async);

            t.start();

            std::this_thread::sleep_for(std::chrono::milliseconds(400));

            auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                end - start);
            assert_that(result, corgi::test::equals(std::string("a")));
            assert_that(int_ms.count(), corgi::test::almost_equals(200, 30));
        });

    corgi::test::add_test(
        "timer", "async repeat",
        []() -> void
        {
            std::chrono::time_point<std::chrono::system_clock> end;
            std::string                                        result;
            timer                                              t(
                200,
                [&]() -> void
                {
                    end = std::chrono::system_clock::now();
                    result += "a";
                },
                timer::mode::async);

            t.repeat(3);
            auto start = std::chrono::system_clock::now();
            t.start();

            std::this_thread::sleep_for(std::chrono::milliseconds(800));

            auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                end - start);
            assert_that(result, corgi::test::equals(std::string("aaa")));
            assert_that(int_ms.count(), corgi::test::almost_equals(600, 40));
        });

    corgi::test::add_test(
        "timer", "async repeat stop",
        []() -> void
        {
            auto start = std::chrono::system_clock::now();

            std::chrono::time_point<std::chrono::system_clock> end;
            std::string                                        result;

            timer t(
                200,
                [&]() -> void
                {
                    end = std::chrono::system_clock::now();
                    result += "a";
                },
                timer::mode::async);

            // Basically stopping the timer at the second tick
            // 200 ms : a
            // 400 ms : aa
            // 550 ms : t2 triggered, t is stopped and won't fire off a third
            // time
            timer t2(
                550, [&]() -> void { t.stop(); }, timer::mode::async);

            t.repeat();
            t.start();
            t2.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(800));

            auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                end - start);
            assert_that(result, corgi::test::equals(std::string("aa")));
            assert_that(int_ms.count(), corgi::test::almost_equals(400, 40));
        });
    return corgi::test::run_all();
}