#pragma once

#include "Version.h"
#include "JSTypes.h"
#include "IBrowser.h"

namespace NL::UI
{
    class IUIPlatformAPI
    {
      public:
        using BrowserRefHandle = std::uint32_t;
        static inline BrowserRefHandle InvalidBrowserRefHandle = 0;

      public:
        virtual ~IUIPlatformAPI() = default;

        /// <summary>
        /// Add or get browser
        /// </summary>
        /// <param name="a_browserName">Unique name, returns the same browser for the same name</param>
        /// <param name="a_funcInfoArr">Function info array for binding to the browser's js. Pass nullptr for no bindings</param>
        /// <param name="a_funcInfoArrSize"></param>
        /// <param name="a_outBrowser">Browser interface</param>
        /// <returns>A handle to release when you no longer need the browser</returns>
        virtual BrowserRefHandle __cdecl AddOrGetBrowser(const char* a_browserName,
                                                         NL::JS::JSFuncInfo* const* a_funcInfoArr,
                                                         const std::uint32_t a_funcInfoArrSize,
                                                         const char* a_startUrl,
                                                         NL::CEF::IBrowser*& a_outBrowser) = 0;

        /// <summary>
        /// Releases browser if no one has the handle
        /// </summary>
        /// <param name="a_handle"></param>
        /// <returns></returns>
        virtual void __cdecl ReleaseBrowserHandle(BrowserRefHandle a_handle) = 0;
    };

    enum APIMessageType : std::uint32_t
    {
        /// <summary>
        /// Request library and api version. No data is needed.
        /// Call after SKSE::MessagingInterface::kPostPostLoad
        /// </summary>
        RequestVersion = 2250,

        /// <summary>
        /// Response with version info. See ResponseVersionMessage struct.
        /// You should check current API version (NL::UI::APIVersion::AS_INT) and version in response.
        /// It is not guaranteed that the major versions are compatible. In this case, I recommend not using the library.
        /// </summary>
        ResponseVersion,

        /// <summary>
        /// Request API. First request initializes library. No data is needed.
        /// Call after SKSE::MessagingInterface::kInputLoaded
        /// </summary>
        RequestAPI,

        /// <summary>
        /// Response with API info. See ResponseAPIMessage struct.
        /// </summary>
        ResponseAPI,
    };

    struct ResponseVersionMessage
    {
        /// <summary>
        /// NirnLabUIPlatform version
        /// </summary>
        std::uint32_t libVersion = NL::UI::LibVersion::AS_INT;

        /// <summary>
        /// NirnLabUIPlatform API version
        /// </summary>
        std::uint32_t apiVersion = NL::UI::APIVersion::AS_INT;
    };

    struct ResponseAPIMessage
    {
        IUIPlatformAPI* API = nullptr;
    };
}
