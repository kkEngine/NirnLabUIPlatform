#pragma once

namespace NL::UI
{
    enum class RendererType : char
    {
        DeferredContext = 0,
        SyncCopy = 1,
    };

    /// <summary>
    /// Global (API) settings
    /// </summary>
    struct Settings
    {
        /// <summary>
        /// Cef debugging port (http://localhost:9009)
        /// </summary>
        int remoteDebuggingPort = 9009;
        /// <summary>
        /// Set to true to allow language switching in the game native menus (console, race, etc.)
        /// </summary>
        bool nativeMenuLangSwitching = true;
        /// <summary>
        /// Renderer type
        /// </summary>
        RendererType rendererType = RendererType::SyncCopy;
        /// <summary>
        /// This value determines the rendering order of the menu among the game's native menus.
        /// Native menu example:
        /// Cursor: menuDepth = 13
        /// Console: menuDepth = 12
        /// HUD: menuDepth = 2
        /// </summary>
        std::int8_t menuDepthPriority = 8;
    };
    static_assert(sizeof(Settings) == 8);

    /// <summary>
    /// Browser settings
    /// </summary>
    struct BrowserSettings
    {
        /// <summary>
        /// Desired frame rate
        /// </summary>
        int frameRate = 60;
        int reservPad = 0;
    };
    static_assert(sizeof(BrowserSettings) == 8);
}
