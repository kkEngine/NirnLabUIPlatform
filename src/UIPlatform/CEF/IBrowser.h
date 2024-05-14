#pragma once

namespace NL::CEF
{
    class IBrowser
    {
      public:
        virtual ~IBrowser() = default;

        virtual bool __cdecl IsReady() = 0;

        virtual void __cdecl SetVisible(bool a_value) = 0;
        virtual bool __cdecl IsVisible() = 0;

        virtual void __cdecl SetFocused(bool a_value) = 0;
        virtual bool __cdecl IsFocused() = 0;

        virtual void __cdecl LoadURL(const char* a_url) = 0;
        virtual void __cdecl SendMsg() = 0;
    };
}
