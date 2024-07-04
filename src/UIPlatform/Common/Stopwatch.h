#pragma once

namespace Common
{
    /// <summary>
    /// NOT thread safe
    /// </summary>
    class Stopwatch
    {
      protected:
        std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::time_point::min();
        std::chrono::steady_clock::time_point m_stopTime = std::chrono::steady_clock::time_point::min();
        bool m_isRunning = false;

      public:
        static std::shared_ptr<Stopwatch> StartNew();

        void Start();
        void Stop();
        void Reset();
        std::chrono::steady_clock::time_point GetStartTimePoint();
        std::chrono::steady_clock::duration GetDuration();
        std::uint64_t GetElapsedMilliseconds();
        std::uint64_t GetElapsedMicroseconds();
    };
}
