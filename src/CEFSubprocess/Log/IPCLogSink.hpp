#pragma once

#include "PCH.h"

namespace NL::Log
{
    template<class Mutex>
    class IPCLogSink : public spdlog::sinks::base_sink<Mutex>
    {
        struct LogMsg
        {
            spdlog::level::level_enum level;
            std::string msg;
        };

    protected:
        CefRefPtr<CefBrowser> m_browser = nullptr;
        std::queue<LogMsg> m_delayedMsgQueue;

        bool SendProcessMessage(const CefRefPtr<CefBrowser> browser, const spdlog::level::level_enum level, const char* msg)
        {
            if (browser != nullptr && browser->IsValid())
            {
                const auto mainFrame = browser->GetMainFrame();
                if (mainFrame != nullptr && mainFrame->IsValid())
                {
                    auto message = CefProcessMessage::Create(IPC_LOG_EVENT);
                    auto argList = message->GetArgumentList();
                    argList->SetInt(0, level);
                    argList->SetString(1, msg);
                    mainFrame->SendProcessMessage(PID_BROWSER, message);

                    return true;
                }
            }

            return false;
        }

        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            if (!SendProcessMessage(m_browser, msg.level, msg.payload.data()))
            {
                m_delayedMsgQueue.emplace(msg.level, msg.payload.data());
            }
        }

        void flush_() override
        {
            while (!m_delayedMsgQueue.empty())
            {
                const auto& msg = m_delayedMsgQueue.front();
                if (!SendProcessMessage(m_browser, msg.level, msg.msg.data()))
                {
                    return;
                }

                m_delayedMsgQueue.pop();
            }
        };

        void set_pattern_(const std::string& pattern) override {};
        void set_formatter_(std::unique_ptr<spdlog::formatter> sink_formatter) override {};

    public:
        IPCLogSink(CefRefPtr<CefBrowser> a_browser)
        {
            m_browser = a_browser;
        }
        IPCLogSink(IPCLogSink&&) = delete;

        void SetBrowser(CefRefPtr<CefBrowser> a_browser)
        {
            std::lock_guard locker(this->mutex_);
            m_browser = a_browser;
        }
    };

    using IPCLogSink_mt = IPCLogSink<std::mutex>;
    using IPCLogSink_st = IPCLogSink<spdlog::details::null_mutex>;
}
