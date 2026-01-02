#include "NirnLabCefApp.h"

namespace NL::CEF
{
    namespace
    {
        std::string GetAdapterLuid()
        {
            const auto device = reinterpret_cast<ID3D11Device*>(RE::BSGraphics::Renderer::GetDevice());
            ThrowIfNullptr(NirnLabCefApp, device);

            HRESULT hr;
            IDXGIDevice* dxgiDevice;
            hr = device->QueryInterface<IDXGIDevice>(&dxgiDevice);
            CheckHresultThrow(hr, fmt::format("{}: failed to query interface {}", NameOf(NirnLabCefApp), NameOf(IDXGIDevice)));

            IDXGIAdapter* dxgiAdapter;
            hr = dxgiDevice->GetAdapter(&dxgiAdapter);
            CheckHresultThrow(hr, fmt::format("{}: failed to get dxgi adapter", NameOf(NirnLabCefApp)));

            DXGI_ADAPTER_DESC adapterDesc;
            hr = dxgiAdapter->GetDesc(&adapterDesc);
            CheckHresultThrow(hr, fmt::format("{}: failed to get dxgi adapter desc", NameOf(NirnLabCefApp)));

            return fmt::format("{},{}", adapterDesc.AdapterLuid.HighPart, adapterDesc.AdapterLuid.LowPart);
        }
    }

    // command line switches https://peter.sh/experiments/chromium-command-line-switches/
    void NirnLabCefApp::OnBeforeCommandLineProcessing(CefString const& process_type, CefRefPtr<CefCommandLine> command_line)
    {
        // disable creation of a GPUCache/ folder on disk
        // command_line->AppendSwitch("disable-gpu-shader-disk-cache");

        // command_line->AppendSwitch("disable-accelerated-video-decode");

        // un-comment to show the built-in Chromium fps meter
        command_line->AppendSwitch("show-fps-counter");

        // command_line->AppendSwitch("disable-gpu-vsync");

        // Disables lazy loading of images and frames
        // command_line->AppendSwitch("disable-lazy-loading");

        // Most systems would not need to use this switch - but on older hardware,
        // Chromium may still choose to disable D3D11 for gpu workarounds.
        // Accelerated OSR will not at all with D3D11 disabled, so we force it on.
        command_line->AppendSwitchWithValue("use-angle", "d3d11");

        // Ensure Chromium runs on the same GPU (will not be able to copy frames otherwise)
        // Also requires D3D11
        auto luid = GetAdapterLuid();
        command_line->AppendSwitchWithValue("use-adapter-luid", luid);
        spdlog::info(NameOf(NirnLabCefApp) ": using adapter luid={}", luid);

        // tell Chromium to autoplay <video> elements without
        // requiring the muted attribute or user interaction
        command_line->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");

        // Allow local files
        command_line->AppendSwitch("allow-file-access-from-files");
        command_line->AppendSwitch("allow-insecure-localhost");

        // https://chromium.googlesource.com/chromium/src/+/main/docs/process_model_and_site_isolation.md
        command_line->AppendSwitch("disable-site-isolation-for-policy");
        command_line->AppendSwitch("disable-site-isolation-trials");
        command_line->AppendSwitchWithValue("process-per-site", "false");
        // command_line->AppendSwitch("single-process");

        // Allow remote debugging
        command_line->AppendSwitchWithValue("remote-allow-origins", "*");

        // Allow subprocesses to run with inherited admin privileges
        command_line->AppendSwitch("do-not-de-elevate");

        // Run the GPU process as a thread in the browser process (fixed GPU lag?)
        command_line->AppendSwitch("in-process-gpu");

        // Disable task throttling of timer tasks from background pages
        command_line->AppendSwitch("disable-background-timer-throttling");

        // Prevent renderer process backgrounding when set
        command_line->AppendSwitch("disable-renderer-backgrounding");

        // Specifies that the main-thread Isolate should initialize in foreground mode
        // If not specified, the the Isolate will start in background mode for extension processes and foreground mode otherwise
        command_line->AppendSwitch("init-isolate-as-foreground");

        // Set default encoding to UTF-8
        command_line->AppendSwitchWithValue("default-encoding", "utf-8");

        // https://chromestatus.com/features
        // command_line->AppendSwitchWithValue("disable-features", "PrintCompositorLPAC");
    }

    CefRefPtr<CefBrowserProcessHandler> CEF::NirnLabCefApp::GetBrowserProcessHandler()
    {
        return this;
    }

    void CEF::NirnLabCefApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
    {
        command_line->AppendSwitchWithValue(IPC_CL_PROCESS_ID_NAME, std::to_string(::GetCurrentProcessId()).c_str());
    }
}
