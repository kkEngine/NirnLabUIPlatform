#include "Stopwatch.h"

namespace Common
{
    std::shared_ptr<Stopwatch> Stopwatch::StartNew()
    {
        const auto stopWatch = std::make_shared<Stopwatch>();
        if (stopWatch != nullptr)
        {
            stopWatch->Start();
        }
        return stopWatch;
    }

    void Stopwatch::Start()
    {
        if (m_isRunning)
        {
            return;
        }

        m_startTime = std::chrono::steady_clock::now();
        m_isRunning = true;
    }

    void Stopwatch::Stop()
    {
        if (!m_isRunning)
        {
            return;
        }

        m_stopTime = std::chrono::steady_clock::now();
        m_isRunning = false;
    }

    void Stopwatch::Reset()
    {
        if (m_isRunning)
        {
            m_startTime = std::chrono::steady_clock::now();
            m_stopTime = std::chrono::steady_clock::time_point::min();
        }
        else
        {
            m_startTime = std::chrono::steady_clock::time_point::min();
            m_stopTime = m_startTime;
        }
    }

    std::chrono::steady_clock::time_point Stopwatch::GetStartTimePoint()
    {
        return m_startTime;
    }

    std::chrono::steady_clock::duration Stopwatch::GetDuration()
    {
        return m_isRunning ? (std::chrono::steady_clock::now() - m_startTime) : (m_stopTime - m_startTime);
    }

    std::uint64_t Stopwatch::GetElapsedMilliseconds()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(GetDuration()).count();
    }

    std::uint64_t Stopwatch::GetElapsedMicroseconds()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(GetDuration()).count();
    }
}
