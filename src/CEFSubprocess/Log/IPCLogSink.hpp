#pragma once

#include "PCH.h"

namespace NL::Log
{
    template<class Mutex>
    class IPCLogSink : public spdlog::sinks::base_sink<Mutex>
    {
    protected:
        CefRefPtr<CefBrowser> m_browser = nullptr;

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

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            if (m_browser != nullptr && m_browser->IsValid())
            {
                const auto mainFrame = m_browser->GetMainFrame();
                if (mainFrame != nullptr)
                {
                    auto message = CefProcessMessage::Create(IPC_LOG_EVENT);
                    auto argList = message->GetArgumentList();
                    argList->SetInt(0, msg.level);
                    argList->SetString(1, msg.payload.data());
                    mainFrame->SendProcessMessage(PID_BROWSER, message);
                }
            }
        }
        void flush_() override {};
        void set_pattern_(const std::string& pattern) override {};
        void set_formatter_(std::unique_ptr<spdlog::formatter> sink_formatter) override {};
    };

    using IPCLogSink_mt = IPCLogSink<std::mutex>;
    using IPCLogSink_st = IPCLogSink<spdlog::details::null_mutex>;
}
