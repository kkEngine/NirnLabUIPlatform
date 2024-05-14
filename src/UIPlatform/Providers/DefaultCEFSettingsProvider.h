#pragma once

#include "PCH.h"
#include "Utils/PathUtils.h"
#include "ICEFSettingsProvider.h"

namespace NL::Providers
{
    class DefaultCEFSettingsProvider : public ICEFSettingsProvider
    {
      public:
        virtual ~DefaultCEFSettingsProvider() = default;
        CefSettings GetCefSettings() override;
        CefBrowserSettings GetCefBrowserSettings() override;
        CefWindowInfo GetCefWindowInfo() override;
    };
}
