#pragma once

namespace NL::CEF
{
    class IBrowser
    {
      public:
        virtual ~IBrowser() = default;

        /// <summary>
        /// Browser takes time to load. You can check if browser ready using this method.
        /// </summary>
        /// <returns></returns>
        virtual bool __cdecl IsBrowserReady() = 0;

        virtual void __cdecl SetBrowserVisible(bool a_value) = 0;
        virtual bool __cdecl IsBrowserVisible() = 0;
        /// <summary>
        /// Sets browser visibility keys. See RE::BSKeyboardDevice::Keys
        /// </summary>
        /// <param name="a_keyCode1"></param>
        /// <param name="a_keyCode2"></param>
        /// <returns></returns>
        virtual void __cdecl ToggleBrowserVisibleByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2) = 0;

        virtual void __cdecl SetBrowserFocused(bool a_value) = 0;
        virtual bool __cdecl IsBrowserFocused() = 0;
        /// <summary>
        /// Sets browser autofocus keys. See RE::BSKeyboardDevice::Keys
        /// Pass 0 to key(s) if key is not needed
        /// </summary>
        /// <param name="a_keyCode1"></param>
        /// <param name="a_keyCode2"></param>
        /// <returns></returns>
        virtual void __cdecl ToggleBrowserFocusByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2) = 0;

        virtual void __cdecl LoadBrowserURL(const char* a_url) = 0;
        virtual void __cdecl SendBrowserMsg() = 0;
    };
}
