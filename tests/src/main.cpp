#include <corgi/chrono/timer.h>
#include <corgi/test/test.h>

#include <iostream>

using namespace corgi::chrono;

int main()
{
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

            timer t2(
                550, [&]() -> void { t.stop(); }, timer::mode::async);

            t.repeat();
            t.start();
            t2.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(800));

            auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                end - start);
            assert_that(result, corgi::test::equals(std::string("aaa")));
            assert_that(int_ms.count(), corgi::test::almost_equals(600, 40));
        });
    return corgi::test::run_all();
}