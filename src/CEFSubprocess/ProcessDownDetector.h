#pragma once

#include "PCH.h"

namespace NL
{
    class ProcessDownDetector
    {
    public:
        using OnProcessDownCallback = std::function<void()>;

    protected:
        std::jthread m_threadDetector;
        OnProcessDownCallback m_onDownCallback;

    public:
        ProcessDownDetector(DWORD a_processId, OnProcessDownCallback a_callback);
    };
}
