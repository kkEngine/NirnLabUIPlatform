#pragma once

#include <comdef.h>

inline std::string CheckHresultMessage(HRESULT hr, const std::string& userMsg)
{
    if (!FAILED(hr))
    {
        return "";
    }

    _com_error err(hr);
    LPCTSTR errMsg = err.ErrorMessage();
    return fmt::format("{}: unexpected HRESULT {:#X}: {}", userMsg, static_cast<unsigned long>(hr), errMsg);
}

inline void CheckHresultThrow(HRESULT hr, const std::string& userMsg)
{
    if (auto msg = CheckHresultMessage(hr, userMsg); !msg.empty())
    {
        throw std::runtime_error(std::move(msg));
    }
}

#define CHECK_HRESULT_LOG_AND_RETURN(hr, userMsg)                      \
    do                                                                 \
    {                                                                  \
        if (auto msg = CheckHresultMessage(hr, userMsg); !msg.empty()) \
        {                                                              \
            spdlog::error("{}", msg);                                  \
            return;                                                    \
        }                                                              \
    } while (0)
