#pragma once

namespace NL::CEF
{
    class IBrowser
    {
      public:
        virtual ~IBrowser() = default;

        virtual bool __cdecl IsBrowserReady() = 0;

        virtual void __cdecl SetBrowserVisible(bool a_value) = 0;
        virtual bool __cdecl IsBrowserVisible() = 0;

        virtual void __cdecl SetBrowserFocused(bool a_value) = 0;
        virtual bool __cdecl IsBrowserFocused() = 0;

        virtual void __cdecl LoadBrowserURL(const char* a_url) = 0;
        virtual void __cdecl SendBrowserMsg() = 0;
    };
}
