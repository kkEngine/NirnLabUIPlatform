#include "ProcessDownDetector.h"

namespace NL
{
    ProcessDownDetector::ProcessDownDetector(DWORD a_processId, OnProcessDownCallback a_callback)
    {
        m_onDownCallback = a_callback;
        m_threadDetector = std::jthread([&, a_processId](std::stop_token a_stopToken) {
            try
            {
                const auto procHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, a_processId);
                while (!a_stopToken.stop_requested())
                {
                    if (::WaitForSingleObject(procHandle, 500) != WAIT_TIMEOUT)
                    {
                        break;
                    }
                }
            }
            catch (const std::exception& err)
            {
                MessageBox(NULL, err.what(), "CEF subprocess error!", MB_OK);
                ::TerminateProcess(::GetCurrentProcess(), EXIT_FAILURE);
            }

            m_onDownCallback();
        });
    }
}
