#pragma once

#define JS_EXECUTE_URL "Skyrim"

#include "JS/Types.h"

namespace NL::CEF
{
    class IBrowser
    {
      public:
        virtual ~IBrowser() = default;

        /// <summary>
        /// Browser takes time to load, so you can check if browser is ready using this method.
        /// </summary>
        /// <returns></returns>
        virtual bool __cdecl IsBrowserReady() = 0;

        virtual void __cdecl SetBrowserVisible(bool a_value) = 0;
        virtual bool __cdecl IsBrowserVisible() = 0;
        /// <summary>
        /// Sets browser visibility keys (see RE::BSKeyboardDevice::Keys)
        /// Pass zeros to disable
        /// </summary>
        /// <param name="a_keyCode1"></param>
        /// <param name="a_keyCode2"></param>
        /// <returns></returns>
        virtual void __cdecl ToggleBrowserVisibleByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2) = 0;

        virtual void __cdecl SetBrowserFocused(bool a_value) = 0;
        virtual bool __cdecl IsBrowserFocused() = 0;
        /// <summary>
        /// Sets browser autofocus keys (see RE::BSKeyboardDevice::Keys)
        /// Pass zeros to disable
        /// Pass 0 to key(s) if key is not needed
        /// </summary>
        /// <param name="a_keyCode1"></param>
        /// <param name="a_keyCode2"></param>
        /// <returns></returns>
        virtual void __cdecl ToggleBrowserFocusByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2) = 0;

        virtual void __cdecl LoadBrowserURL(const char* a_url) = 0;
        virtual void __cdecl ExecuteJavaScript(const char* a_script, const char* a_scriptUrl = JS_EXECUTE_URL) = 0;
        virtual void __cdecl AddFunctionCallback(const char* a_objectName, const char* a_funcName, NL::JS::JSFuncCallbackData a_callbackData) = 0;
    };
}
