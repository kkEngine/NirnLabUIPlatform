#pragma once

#include "PCH.h"
#include "IBrowser.h"
#include "Render/CEFRenderLayer.h"
#include "CEF/NirnLabCefClient.h"
#include "Services/CEFService.h"

#define UPDATE_MODIFIER_FLAG(eventParam, modifierParam, flagName) \
    if (eventParam->IsDown())                                      \
        modifierParam |= flagName;                                \
    else if (eventParam->IsUp())                                   \
        modifierParam &= ~flagName;

namespace NL::CEF
{


    class CEFBrowser : public IBrowser,
                       public RE::MenuEventHandler
    {
      protected:
        std::shared_ptr<spdlog::logger> m_logger = nullptr;
        CefRefPtr<NirnLabCefClient> m_cefClient = nullptr;

      public:
        CEFBrowser(
            std::shared_ptr<spdlog::logger> a_logger,
            CefRefPtr<NirnLabCefClient> a_cefClient);
        ~CEFBrowser() override = default;

        bool m_isFocused = false;

        RE::CursorMenu* m_cursorMenu = nullptr;
        float& m_CurrentMousePosX = RE::MenuCursor::GetSingleton()->cursorPosX;
        float& m_CurrentMousePosY = RE::MenuCursor::GetSingleton()->cursorPosY;
        CefMouseEvent m_LastCefMouseEvent;
        std::uint32_t m_CefKeyModifiers = 0;

        CefRefPtr<NirnLabCefClient> GetCefClient();
        bool IsReadyAndLog();

        // IBrowser
        bool __cdecl IsBrowserReady() override;

        void __cdecl SetBrowserVisible(bool a_value) override;
        bool __cdecl IsBrowserVisible() override;

        void __cdecl SetBrowserFocused(bool a_value) override;
        bool __cdecl IsBrowserFocused() override;

        void __cdecl LoadBrowserURL(const char* a_url) override;
        void __cdecl SendBrowserMsg() override;

        // RE::MenuEventHandler
        bool CanProcess(RE::InputEvent* a_event) override;
        bool ProcessMouseMove(RE::MouseMoveEvent* a_event) override;
        bool ProcessButton(RE::ButtonEvent* a_event) override;
    };
}
