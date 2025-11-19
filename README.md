# NirnLabUIPlatform

UI platform for Skyrim with CEF (Chromium Embedded Framework) integration and more. You can create interface based on html5, css, javascript. This project is a part of NirnLab project.

## NirnLab

NirnLab is a competitive MMO-like mod for Skyrim which will be announced after the main part is completed.
If you want to follow the progress and news, subscribe to [![Discord](https://img.shields.io/discord/1004071212361711678?label=Discord&logo=Discord)](https://discord.gg/zxgzE45Vtz).

## Features
- Chromium browser is rendered in Skyrim's native menu (can take screenshots, using native cursor)
- Creating multiple browsers
- Changing keyboard layout out of the box (SHIFT-CTRL or SHIFT-ALT) including console
- Binding js to cpp function callbacks
- Auto closing CEF processes in case of the game crash

## Usage as skse plugin (preferred)
You can find all public APIs in this [folder](https://github.com/kkEngine/NirnLabUIPlatform/tree/main/src/UIPlatform/NirnLabUIPlatformAPI). Copy and include the folder in your project (included in release).
Test [examples](https://github.com/kkEngine/NirnLabUIPlatform/tree/main/src/UIPlatformTest)  

# Get API (CommonLibSSE example)
First of all we need to check the API version. If major versions are defferent i don't recommend continuing (may crash).

Send request version message when all plugin loaded (kPostPostLoad)
```cpp
SKSE::GetMessagingInterface()->Dispatch(NL::UI::APIMessageType::RequestVersion, nullptr, 0, LibVersion::PROJECT_NAME);
```
Response message will contain API and Lib versions
```cpp
SKSE::GetMessagingInterface()->RegisterListener(LibVersion::PROJECT_NAME, [](SKSE::MessagingInterface::Message* a_msg) {
    switch (a_msg->type)
    {
    case NL::UI::APIMessageType::ResponseVersion:
        // API and Lib versions
        const auto versionInfo = reinterpret_cast<NL::UI::ResponseVersionMessage*>(a_msg->data); 
        break;
    }
});
```

Check the API version and if it's ok, request API
```cpp
NL::UI::Settings defaultSettings;
SKSE::GetMessagingInterface()->Dispatch(NL::UI::APIMessageType::RequestAPI, &defaultSettings, sizeof(defaultSettings), NL::UI::LibVersion::PROJECT_NAME);
```
When a first plugin requests API, library and CEF will initialize
Response message will contain API struct. You can save the API pointer and use it in the future.
```cpp
SKSE::GetMessagingInterface()->RegisterListener(LibVersion::PROJECT_NAME, [](SKSE::MessagingInterface::Message* a_msg) {
    switch (a_msg->type)
    {
    case NL::UI::APIMessageType::ResponseAPI:
        auto api = reinterpret_cast<NL::UI::ResponseAPIMessage*>(a_msg->data)->API;
        break;
    }
});
```

There are two helper headers for loading the API: SKSELoader.h (loads via SKSE messaging), DllLoader.h (loads via LoadLibrary win function).
See the example solution in main.cpp for how to use these loaders (Init1stMethodToGetAPI(), Init2ndMethodToGetAPI(), Init3rdMethodToGetAPI()).

# Create browser
```cpp
NL::CEF::IBrowser* g_browser = nullptr;
NL::UI::IUIPlatformAPI::BrowserRefHandle g_browserHandle = NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle;

void CreateBrowser(NL::UI::IUIPlatformAPI* a_api)
{
    NL::UI::BrowserSettings bSettings;
    bSettings.frameRate = 60;

    g_browserHandle = a_api->AddOrGetBrowser("MyPluginCEF", nullptr, 0, "https://www.youtube.com", &bSettings, g_browser);
    if (g_browserHandle == NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle)
    {
        spdlog::error("browser handle is invalid");
        return;
    }

    // Keep "g_browserHandle" util you need a browser
    // When the browser is no longer needed, release it using a_api->ReleaseBrowserHandle();
    // If this was the last browser handle, the browser will be deleted

    // Check NL::CEF::IBrowser interface for more features
    m_browser->ToggleBrowserFocusByKeys(RE::BSKeyboardDevice::Keys::kF6, 0);
    g_browser->ToggleBrowserVisibleByKeys(RE::BSKeyboardDevice::Keys::kF7, 0);
    g_browser->SetBrowserFocused(true);
    g_browser->ExecuteJavaScript("window.myString = 'Hello CEF'"); 
}

void ReleaseBrowser(NL::UI::IUIPlatformAPI::BrowserRefHandle a_handle)
{
    g_browser = nullptr;
    a_api->ReleaseBrowserHandle(g_browserHandle);
    g_browserHandle = NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle;
}
```

# Events from c++ to js
You can create special "event function" that will receive events.
An example of creating an event function can be found in LocalTestPage.cpp

```js
NL.addEventListener("on:message", (data) => {
	const para = document.createElement("p");
	const node = document.createTextNode(data);
	para.appendChild(node);
});
```

```cpp
auto eventFunc = new JS::JSFuncInfo();
eventFunc->objectName = "NL";
eventFunc->funcName = "addEventListener";
eventFunc->callbackData.isEventFunction = true;

// Pass with other functions in array when browser creating. see AddOrGetBrowser in api.

// ...
m_browser->ExecEventFunction("on:message", "EVENT_FUNC WORKS!");
```

## Dev and build requirements
- CMake 3.23+
- Vcpkg
- Microsoft Visual Studio 2022
    - C++ desktop dev
- Microsoft Visual C++ 2015-2022 Redistributable (x64)

## Runtime dependencies
- Address library (https://www.nexusmods.com/skyrimspecialedition/mods/32444)
- SKSE (https://skse.silverlock.org/)

### Thanks to
[@Pospelove](https://github.com/Pospelove) <br />
[Skymp](https://github.com/skyrim-multiplayer/skymp) <br />
[CommonLibSSE](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE) <br />
[CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) <br />
Chromium Embedded Framework ([CEF](https://bitbucket.org/chromiumembedded/cef)) <br />

### Nexus
[mod](https://www.nexusmods.com/skyrimspecialedition/mods/125447)

### License
MIT
