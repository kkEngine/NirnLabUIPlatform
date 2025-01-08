#pragma once

namespace NL::UI
{
    /// <summary>
    /// Global (API) settings
    /// </summary>
    struct Settings
    {
        /// <summary>
        /// Cef debugging port (http://localhost:9009)
        /// </summary>
        int remoteDebuggingPort = 9009;
        int reservPad = 0;
    };

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
}
