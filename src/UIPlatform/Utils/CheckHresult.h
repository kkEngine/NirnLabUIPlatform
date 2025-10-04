#pragma once

#include <comdef.h>

inline std::string CheckHresultMessage(HRESULT hr, const std::string& userMsg)
{
    if (!FAILED(hr))
    {
      return "";
    }
    _com_error err(hr);
    LPCTSTR errMsg = err.ErrorMessage();                                                      \
    return fmt::format("{}: unexpected HRESULT {:#X}: {}", msg, static_cast<unsigned long>(hr), errMsg);
}

inline std::string CheckHresultThrow(HRESULT hr, const std::string& userMsg)
{
    if (auto msg = CheckHresultMessge(hr, userMsg))
    {
        throw std::runtime_error(std::move(msg));
    }
}

#define CHECK_HRESULT_LOG_AND_RETURN(hr, userMsg)        \
    do                                                   \
    {                                                    \
        if (auto msg = CheckHresultMessage(hr, userMsg)) \
        {                                                \
            spdlog::error("{}", msg);                    \
            return;                                      \
        }                                                \
    } while (0)
