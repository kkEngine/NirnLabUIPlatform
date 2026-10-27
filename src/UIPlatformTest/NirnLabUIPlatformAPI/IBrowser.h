#pragma once

#include "JSTypes.h"

namespace RE
{
    class ButtonEvent;
    class InputEvent;
    class MouseMoveEvent;
}

namespace NL::CEF
{
    class IBrowser
    {
    public:
        static constexpr auto kJSExecuteScriptURL = "Skyrim";

        enum class InputMode : std::uint8_t
        {
            /// <summary>
            /// Browser handles input before the game and stops input propagation.
            /// Shows the cursor.
            /// </summary>
            Exclusive,

            /// <summary>
            /// Browser handles input before the game and allows input propagation.
            /// Shows the cursor.
            /// </summary>
            NonExclusive,

            /// <summary>
            /// You can control input manually with this mode
            /// </summary>
            Manual,
        };

        virtual ~IBrowser() = default;

        /// <summary>
        /// Browser takes time to load, so you can check if browser is ready using some methods.
        /// </summary>
        /// <returns></returns>
        virtual bool __cdecl IsBrowserReady() = 0;
        /// <summary>
        /// Page takes time to load. You can check that the page has loaded using this method.
        /// </summary>
        /// <returns></returns>
        virtual bool __cdecl IsPageLoaded() = 0;

        virtual void __cdecl SetBrowserVisible(bool a_visible) = 0;
        virtual bool __cdecl IsBrowserVisible() = 0;
        /// <summary>
        /// Sets browser visibility keys (see RE::BSKeyboardDevice::Keys)
        /// Pass zeros to disable
        /// </summary>
        /// <param name="a_keyCode1"></param>
        /// <param name="a_keyCode2"></param>
        /// <returns></returns>
        virtual void __cdecl ToggleBrowserVisibleByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2) = 0;

        /// <summary>
        /// Focuses or unfocuses the browser with input mode (see SetInputMode)
        /// </summary>
        /// <param name="a_focused"></param>
        /// <returns></returns>
        virtual void __cdecl SetBrowserFocused(bool a_focused) = 0;
        virtual bool __cdecl IsBrowserFocused() = 0;
        /// <summary>
        /// Sets browser autofocus keys (see RE::BSKeyboardDevice::Keys)
        /// Pass zeros to disable
        /// </summary>
        /// <param name="a_keyCode1"></param>
        /// <param name="a_keyCode2"></param>
        /// <returns></returns>
        virtual void __cdecl ToggleBrowserFocusByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2) = 0;

        /// <summary>
        /// Loads new url
        /// </summary>
        /// <param name="a_url"></param>
        /// <param name="a_clearJSFunctions">If true then remove all previous js function callbacks</param>
        /// <returns></returns>
        virtual void __cdecl LoadBrowserURL(const char* a_url, bool a_clearJSFunctions = true) = 0;

        virtual void __cdecl ExecuteJavaScript(const char* a_script, const char* a_scriptUrl = kJSExecuteScriptURL) = 0;
        virtual void __cdecl AddFunctionCallback(const NL::JS::JSFuncInfo& a_callbackInfo) = 0;
        virtual void __cdecl RemoveFunctionCallback(const char* a_objectName, const char* a_funcName) = 0;
        virtual void __cdecl RemoveFunctionCallback(const NL::JS::JSFuncInfo& a_callbackInfo) = 0;
        virtual void __cdecl ExecEventFunction(const char* a_eventName, const char* a_data) = 0;

        /// <summary>
        /// Sets input mode
        /// </summary>
        /// <param name="a_mode"></param>
        /// <returns></returns>
        virtual void __cdecl SetInputMode(InputMode a_mode) = 0;
        virtual InputMode __cdecl GetInputMode() = 0;

        // The following methods allow manual control over input without intercepting Skyrim's input system.
        // They provide the ability to programmatically inject simulated mouse and keyboard events.
        // There are forward declarations for the input classes; if you decide to use these methods,
        // you should use a CommonLib (see RE::MenuEventHandler) or do some reverse engineering.
        virtual bool __cdecl CanProcess(RE::InputEvent* a_event) = 0;
        virtual bool __cdecl ProcessMouseMove(RE::MouseMoveEvent* a_event) = 0;
        virtual bool __cdecl ProcessButton(RE::ButtonEvent* a_event) = 0;
    };
}
