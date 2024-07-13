#pragma once

#include "PCH.h"
#include "Render/CEFRenderLayer.h"
#include "CEF/NirnLabCefClient.h"
#include "Services/CEFService.h"
#include "Hooks/WinProcHook.h"
#include "JS/JSFunctionStorage.h"
#include "Converters/CefValueToJSONConverter.h"

namespace NL::CEF
{
    class NirnLabCefClient;

    class DefaultBrowser : public IBrowser,
                           public RE::MenuEventHandler
    {
      protected:
        std::shared_ptr<spdlog::logger> m_logger = nullptr;
        CefRefPtr<NirnLabCefClient> m_cefClient = nullptr;
        std::shared_ptr<NL::JS::JSFunctionStorage> m_jsFuncStorage = nullptr;

      public:
        DefaultBrowser(
            std::shared_ptr<spdlog::logger> a_logger,
            CefRefPtr<NirnLabCefClient> a_cefClient,
            std::shared_ptr<NL::JS::JSFunctionStorage> a_jsFuncStorage);
        ~DefaultBrowser() override = default;

        bool m_isFocused = false;

        RE::CursorMenu* m_cursorMenu = nullptr;
        float& m_currentMousePosX = RE::MenuCursor::GetSingleton()->cursorPosX;
        float& m_currentMousePosY = RE::MenuCursor::GetSingleton()->cursorPosY;
        CefMouseEvent m_lastCefMouseEvent;
        CefKeyEvent m_lastCharCefKeyEvent;
        std::uint32_t m_cefKeyModifiers = 0;
        std::uint32_t m_lastScanCode = 0;
        float m_keyHeldDuration = 0;

        std::uint32_t m_toggleFocusKeyCode1 = 0;
        std::uint32_t m_toggleFocusKeyCode2 = 0;

        std::uint32_t m_toggleVisibleKeyCode1 = 0;
        std::uint32_t m_toggleVisibleKeyCode2 = 0;

        sigslot::connection onWndInactiveConnection;
        bool m_wasCursorOpen = false;

        void UpdateCefKeyModifiers(const RE::ButtonEvent* a_event, const cef_event_flags_t a_flags);
        void ClearCefKeyModifiers();
        void UpdateCefKeyModifiersFromVK(const RE::ButtonEvent* a_event, const std::uint32_t a_vkCode);
        void CheckToggleFocusKeys(const RE::ButtonEvent* a_event);
        void CheckToggleVisibleKeys(const RE::ButtonEvent* a_event);

        CefRefPtr<NirnLabCefClient> GetCefClient();
        bool IsReadyAndLog();

        // IBrowser
        bool __cdecl IsBrowserReady() override;

        void __cdecl SetBrowserVisible(bool a_value) override;
        bool __cdecl IsBrowserVisible() override;
        void __cdecl ToggleBrowserVisibleByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2) override;

        void __cdecl SetBrowserFocused(bool a_value) override;
        bool __cdecl IsBrowserFocused() override;
        void __cdecl ToggleBrowserFocusByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2) override;

        void __cdecl LoadBrowserURL(const char* a_url) override;
        void __cdecl ExecuteJavaScript(const char* a_script, const char* a_scriptUrl = JS_EXECUTE_SCRIPT_URL) override;
        void __cdecl AddFunctionCallback(const char* a_objectName, const char* a_funcName, NL::JS::JSFuncCallbackData a_callbackData) override;

        // RE::MenuEventHandler
        bool CanProcess(RE::InputEvent* a_event) override;
        bool ProcessMouseMove(RE::MouseMoveEvent* a_event) override;
        bool ProcessButton(RE::ButtonEvent* a_event) override;
    };
}
