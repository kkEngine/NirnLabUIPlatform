#pragma once

#include "PCH.h"
#include "Hooks/ShutdownHook.hpp"
#include "Hooks/CharGeneratorHook.h"
#include "Common/Singleton.h"
#include "Services/UIPlatformService.h"
#include "Providers/CustomCEFSettingsProvider.h"
#include "Converters/KeyInputConverter.h"

namespace NL::Controllers
{
    using BrowserRefHandle = NL::UI::IUIPlatformAPI::BrowserRefHandle;

    class PublicAPIController : public NL::UI::IUIPlatformAPI,
                                public NL::Common::Singleton<PublicAPIController>

    {
    protected:
        struct BrowserHandleData
        {
            NL::CEF::IBrowser* browser = nullptr;
            std::unordered_set<BrowserRefHandle> refHandles;
        };

        sigslot::scoped_connection m_onShutdownConnection;
        std::queue<OnShutdownFunc_t> m_onShutdownFuncs;

    public:
        // NL::UI::IUIPlatformAPI
        BrowserRefHandle __cdecl AddOrGetBrowser(const char* a_browserName,
                                                 NL::JS::JSFuncInfo* const* a_funcInfoArr,
                                                 const std::uint32_t a_funcInfoArrSize,
                                                 const char* a_startUrl,
                                                 NL::CEF::IBrowser*& a_outBrowser) override;

        void __cdecl ReleaseBrowserHandle(BrowserRefHandle a_handle) override;

        BrowserRefHandle __cdecl AddOrGetBrowser(const char* a_browserName,
                                                 NL::JS::JSFuncInfo* const* a_funcInfoArr,
                                                 const std::uint32_t a_funcInfoArrSize,
                                                 const char* a_startUrl,
                                                 NL::UI::BrowserSettings* a_settings,
                                                 NL::CEF::IBrowser*& a_outBrowser) override;

        void RegisterOnShutdown(OnShutdownFunc_t a_callback) override;

    protected:
        NL::UI::ResponseVersionMessage m_rvMessage{NL::UI::LibVersion::AS_INT, NL::UI::APIVersion::AS_INT};
        NL::UI::ResponseAPIMessage m_rAPIMessage{this};

        std::atomic<BrowserRefHandle> m_currentRefHandle{1};

        std::mutex m_mapMutex;
        std::unordered_map<std::string, BrowserHandleData> m_browserNameMap;
        std::unordered_map<BrowserRefHandle, std::string> m_browserHandleMap;

        std::mutex m_initPlatformServiceMutex;
        std::shared_ptr<NL::Providers::ICEFSettingsProvider> m_settingsProvider = nullptr;

        std::shared_ptr<NL::Menus::MultiLayerMenu> GetMultiLayerMenu();

    public:
        NL::UI::ResponseVersionMessage* GetVersionMessage();
        NL::UI::ResponseAPIMessage* GetAPIMessage();

        void Init();
        bool InitIfNotPlatformService(const NL::UI::Settings* a_settings);
        void SetSettingsProvider(const NL::UI::Settings* a_settings);
        std::shared_ptr<NL::Providers::ICEFSettingsProvider> GetSettingsProvider();
    };
}
