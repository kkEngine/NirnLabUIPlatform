#pragma once

#include "PCH.h"
#include "CEFService.h"
#include "Common/Singleton.h"
#include "Render/IRenderLayer.h"
#include "CEF/NirnLabCefApp.h"
#include "CEF/IBrowser.h"
#include "CEF/CEFBrowser.h"
#include "Menus/MultiLayerMenu.h"

namespace NL::Services
{
    class UIPlatformService : public NL::Common::Singleton<UIPlatformService>
    {
      protected:
        friend class NL::Common::Singleton<UIPlatformService>;

        static inline std::mutex s_uipInitMutex;
        static inline bool s_isUIPInited = false;

        std::shared_ptr<spdlog::logger> m_logger = nullptr;
        std::shared_ptr<CEFService> m_cefService = nullptr;
        std::shared_ptr<NL::Menus::MultiLayerMenu> m_mlMenu = nullptr;

      public:
        UIPlatformService();
        ~UIPlatformService() override;

        /// <summary>
        /// Init ui service and it's dependencies
        /// </summary>
        /// <returns></returns>
        bool Init(
            std::shared_ptr<spdlog::logger> a_logger,
            std::shared_ptr<CEFService> a_cefService);

        /// <summary>
        /// Close ui service and it's dependencies
        /// </summary>
        void Shutdown();

        std::shared_ptr<NL::Menus::MultiLayerMenu> GetNativeMenu();
    };
}
