#pragma once

#include "PCH.h"
#include "Common/Singleton.h"
#include "Services/UIPlatformService.h"

namespace NL::Controllers
{
    using BrowserRefHandle = NL::UI::IUIPlatformAPI::BrowserRefHandle;

    class PublicAPIController : public NL::Common::Singleton<PublicAPIController>,
                                public NL::UI::IUIPlatformAPI
    {
      protected:
          struct BrowserHandleData
          {
              NL::CEF::IBrowser* browser = nullptr;
              std::unordered_set<BrowserRefHandle> refHandles;
          };

      protected:
        NL::UI::ResponseVersionMessage m_rvMessage{LibVersion::AS_INT, APIVersion::AS_INT};
        NL::UI::ResponseAPIMessage m_rAPIMessage{this};

        std::atomic<BrowserRefHandle> m_currentRefHandle{1};

        std::mutex m_mapMutex;
        std::unordered_map<std::string, BrowserHandleData> m_browserNameMap;
        std::unordered_map<BrowserRefHandle, std::string> m_browserHandleMap;

        std::shared_ptr<NL::Menus::MultiLayerMenu> GetMultiLayerMenu();

      public:
        NL::UI::ResponseVersionMessage* GetVersionMessage();
        NL::UI::ResponseAPIMessage* GetAPIMessage();

        void Init();

        // NL::UI::IUIPlatformAPI
        BrowserRefHandle __cdecl AddOrGetBrowser(const char* a_browserName,
                                                 const NL::JS::JSFuncInfo** a_funcInfoArr,
                                                 const std::uint32_t a_funcInfoArrSize,
                                                 const char* a_startUrl,
                                                 NL::CEF::IBrowser*& a_outBrowser) override;
        void __cdecl ReleaseBrowserHandle(BrowserRefHandle a_handle) override;
    };
}
