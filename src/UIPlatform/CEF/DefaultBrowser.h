#pragma once

#include "PCH.h"
#include "Render/CEFRenderLayer.h"
#include "CEF/NirnLabCefClient.h"
#include "Services/CEFService.h"
#include "Hooks/WinProcHook.h"
#include "JS/JSFunctionStorage.h"
#include "JS/JSEventFuncInfo.h"
#include "Converters/CefValueToJSONConverter.h"
#include "Converters/KeyInputConverter.h"

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

        // Url
        std::recursive_mutex m_urlMutex;
        bool m_isUrlCached = false;
        bool m_isPageLoaded = false;
        bool m_clearJSFunctions = false;
        std::string m_urlCache = "";

        // Focus
        bool m_isFocused = false;
        bool m_isFocusedCached = false;

        // JS execution
        std::vector<std::tuple<std::string, std::string>> m_jsExecCache;

        // JS function callback
        std::list<NL::JS::JSFuncInfoString> m_jsFuncCallbackInfoCache;
        std::list<std::tuple<std::string, std::string>> m_jsFuncRemoveCache;

        RE::CursorMenu* m_cursorMenu = nullptr;
        float& m_currentMousePosX = RE::MenuCursor::GetSingleton()->cursorPosX;
        float& m_currentMousePosY = RE::MenuCursor::GetSingleton()->cursorPosY;
        CefMouseEvent m_lastCefMouseEvent;
        NL::Converters::KeyInputConverter m_keyInputConverter;

        std::uint32_t m_toggleFocusKeyCode1 = 0;
        std::uint32_t m_toggleFocusKeyCode2 = 0;

        std::uint32_t m_toggleVisibleKeyCode1 = 0;
        std::uint32_t m_toggleVisibleKeyCode2 = 0;

        bool m_wasCursorOpen = false;

        sigslot::scoped_connection m_onWndInactive_Connection;
        sigslot::scoped_connection m_onIPCMessageReceived_Connection;
        sigslot::scoped_connection m_onAfterBrowserCreated_Connection;
        sigslot::scoped_connection m_onMainFrameLoadStart_Connection;
        sigslot::scoped_connection m_onMainFrameLoadEnd_Connection;

    public:
        DefaultBrowser(std::shared_ptr<spdlog::logger> a_logger,
                       CefRefPtr<NirnLabCefClient> a_cefClient,
                       std::shared_ptr<NL::JS::JSFunctionStorage> a_jsFuncStorage);
        ~DefaultBrowser() override;

        void CheckToggleFocusKeys(const RE::ButtonEvent* a_event);
        void CheckToggleVisibleKeys(const RE::ButtonEvent* a_event);

        CefRefPtr<NirnLabCefClient> GetCefClient();
        bool IsReadyAndLog();

        void AddFunctionCallbackAndSendMessage(const NL::JS::JSFuncInfo& a_callbackInfo);
        void RemoveFunctionCallbackAndSendMessage(const char* a_objectName, const char* a_funcName);

        // IBrowser
        bool __cdecl IsBrowserReady() override;
        bool __cdecl IsPageLoaded() override;

        void __cdecl SetBrowserVisible(bool a_value) override;
        bool __cdecl IsBrowserVisible() override;
        void __cdecl ToggleBrowserVisibleByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2) override;

        void __cdecl SetBrowserFocused(bool a_value) override;
        bool __cdecl IsBrowserFocused() override;
        void __cdecl ToggleBrowserFocusByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2) override;

        void __cdecl LoadBrowserURL(const char* a_url, bool a_clearJSFunctions = true) override;
        void __cdecl ExecuteJavaScript(const char* a_script, const char* a_scriptUrl = JS_EXECUTE_SCRIPT_URL) override;
        void __cdecl AddFunctionCallback(const NL::JS::JSFuncInfo& a_callbackInfo) override;
        void __cdecl RemoveFunctionCallback(const char* a_objectName, const char* a_funcName) override;
        void __cdecl RemoveFunctionCallback(const NL::JS::JSFuncInfo& a_callbackInfo) override;
        void __cdecl ExecEventFunction(const char* a_eventName, const char* a_data) override;

        // RE::MenuEventHandler
        bool CanProcess(RE::InputEvent* a_event) override;
        bool ProcessMouseMove(RE::MouseMoveEvent* a_event) override;
        bool ProcessButton(RE::ButtonEvent* a_event) override;
    };
}
