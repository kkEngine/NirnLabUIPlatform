#pragma once

#include "PCH.h"
#include "CEFService.h"
#include "Common/Singleton.h"
#include "Render/IRenderLayer.h"
#include "CEF/NirnLabCefApp.h"
#include "CEF/DefaultBrowser.h"
#include "Menus/MultiLayerMenu.h"
#include "Menus/CEFMenu.h"
#include "Providers/CustomCEFSettingsProvider.h"
#include "JS/JSFunctionStorage.h"
#include "NirnLabUIPlatformAPI/Settings.h"

namespace NL::Services
{
    class UIPlatformService : public NL::Common::Singleton<UIPlatformService>
    {
    protected:
        friend class NL::Common::Singleton<UIPlatformService>;

        static inline std::mutex s_uipInitMutex;
        static inline bool s_isUIPInited = false;

        std::shared_ptr<spdlog::logger> m_logger = nullptr;
        std::shared_ptr<NL::Menus::MultiLayerMenu> m_mlMenu = nullptr;

    public:
        UIPlatformService();
        ~UIPlatformService() override = default;

        bool IsInited();

        /// <summary>
        /// Init ui service and it's dependencies
        /// </summary>
        /// <returns></returns>
        bool Init(std::shared_ptr<spdlog::logger> a_logger,
                  std::shared_ptr<NL::Providers::ICEFSettingsProvider> a_settingsProvider);

        /// <summary>
        /// Init ui service with custom settings
        /// </summary>
        /// <returns></returns>
        bool InitAndShowMenuWithSettings(std::shared_ptr<NL::Providers::ICEFSettingsProvider> a_settingsProvider);

        /// <summary>
        /// Close ui service and it's dependencies
        /// </summary>
        void Shutdown();

        std::shared_ptr<NL::Menus::MultiLayerMenu> GetMultiLayerMenu();
        std::shared_ptr<NL::Menus::CEFMenu> CreateCefMenu(std::shared_ptr<NL::JS::JSFunctionStorage> a_funcStorage,
                                                          NL::JS::JSEventFuncInfo& a_eventFuncInfo);
    };
}
