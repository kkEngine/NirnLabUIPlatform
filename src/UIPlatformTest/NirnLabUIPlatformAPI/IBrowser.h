#pragma once

#define JS_EXECUTE_SCRIPT_URL "Skyrim"

#include "JSTypes.h"

namespace NL::CEF
{
    class IBrowser
    {
    public:
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

        virtual void __cdecl ExecuteJavaScript(const char* a_script, const char* a_scriptUrl = JS_EXECUTE_SCRIPT_URL) = 0;
        virtual void __cdecl AddFunctionCallback(const NL::JS::JSFuncInfo& a_callbackInfo) = 0;
        virtual void __cdecl RemoveFunctionCallback(const char* a_objectName, const char* a_funcName) = 0;
        virtual void __cdecl RemoveFunctionCallback(const NL::JS::JSFuncInfo& a_callbackInfo) = 0;
        virtual void __cdecl ExecEventFunction(const char* a_eventName, const char* a_data) = 0;
    };
}
