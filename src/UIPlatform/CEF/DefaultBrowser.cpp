#include "DefaultBrowser.h"

namespace NL::CEF
{
    DefaultBrowser::DefaultBrowser(
        std::shared_ptr<spdlog::logger> a_logger,
        CefRefPtr<NirnLabCefClient> a_cefClient,
        std::shared_ptr<NL::JS::JSFunctionStorage> a_jsFuncStorage)
    {
        ThrowIfNullptr(DefaultBrowser, a_logger);
        m_logger = a_logger;

        ThrowIfNullptr(DefaultBrowser, a_cefClient);
        m_cefClient = a_cefClient;

        ThrowIfNullptr(DefaultBrowser, a_jsFuncStorage);
        m_jsFuncStorage = a_jsFuncStorage;

        ZeroMemory(&m_lastCefMouseEvent, sizeof(CefMouseEvent));
        ZeroMemory(&m_lastCharCefKeyEvent, sizeof(CefKeyEvent));

        m_onWndInactive_Connection = NL::Hooks::WinProcHook::OnWndInactive.connect([&]() {
            ClearCefKeyModifiers();
        });

        m_onIPCMessageReceived_Connection = m_cefClient->onIPCMessageReceived.connect([&, a_jsFuncStorage](CefRefPtr<CefProcessMessage> a_message) {
            if (a_message->GetName() == IPC_JS_FUNCTION_CALL_EVENT)
            {
                const auto ipcArgs = a_message->GetArgumentList();
                const auto objName = ipcArgs->GetString(0).ToString();
                const auto funcName = ipcArgs->GetString(1).ToString();
                const auto argList = ipcArgs->GetList(2);

                const auto params = NL::Converters::CefValueToJSONConverter::ConvertToJSONStringArgs(argList);
                m_jsFuncStorage->ExecuteFunctionCallback(objName, funcName, params, a_jsFuncStorage);
            }
            else if (a_message->GetName() == IPC_LOG_EVENT)
            {
                const auto logger = spdlog::get(NL_UI_SUBPROC_NAME);
                if (logger != nullptr)
                {
                    auto argList = a_message->GetArgumentList();
                    logger->log(static_cast<spdlog::level::level_enum>(argList->GetInt(0)), argList->GetString(1).ToString().c_str());
                }
            }
        });

        m_onAfterBrowserCreated_Connection = m_cefClient->onAfterBrowserCreated.connect([&](CefRefPtr<CefBrowser> a_cefBrowser) {
            std::lock_guard locker(m_urlMutex);
            // load url
            if (m_isUrlCached)
            {
                LoadBrowserURL(m_urlCache.c_str(), m_clearJSFunctions);
            }
        });

        m_onMainFrameLoadStart_Connection = m_cefClient->onMainFrameLoadStart.connect([&]() {
            std::lock_guard locker(m_urlMutex);
            m_isPageLoaded = true;

            // Add js func callbacks
            if (m_clearJSFunctions)
            {
                m_jsFuncStorage->ClearFunctionCallback();
            }
            else
            {
                const auto browser = m_cefClient->GetBrowser();
                if (browser != nullptr && m_jsFuncStorage->GetSize() > 0)
                {
                    auto cefMessage = CefProcessMessage::Create(IPC_JS_FUNCION_ADD_EVENT);
                    cefMessage->GetArgumentList()->SetDictionary(0, m_jsFuncStorage->ConvertToCefDictionary());
                    browser->GetMainFrame()->SendProcessMessage(CefProcessId::PID_RENDERER, cefMessage);
                }
            }

            for (auto& funcInfo : m_jsFuncCallbackInfoCache)
            {
                AddFunctionCallbackAndSendMessage(funcInfo);
            }
            m_jsFuncCallbackInfoCache.clear();

            // Remove js func callbacks
            for (auto& funcInfo : m_jsFuncRemoveCache)
            {
                RemoveFunctionCallbackAndSendMessage(std::get<0>(funcInfo).c_str(), std::get<1>(funcInfo).c_str());
            }
            m_jsFuncRemoveCache.clear();

            // JS exec scripts
            if (!m_jsExecCache.empty())
            {
                for (auto& jsScript : m_jsExecCache)
                {
                    ExecuteJavaScript(std::get<0>(jsScript).c_str(), std::get<1>(jsScript).c_str());
                }
                m_jsExecCache.clear();
                m_jsExecCache.shrink_to_fit();
            }

            // Focus
            if (m_isFocusedCached)
            {
                SetBrowserFocused(m_isFocused);
            }
        });
    }

    DefaultBrowser::~DefaultBrowser()
    {
        auto browser = m_cefClient->GetBrowser();
        if (browser != nullptr)
        {
            browser->GetHost()->CloseBrowser(true);
        }

        m_jsFuncStorage->ClearFunctionCallback();
    }

    void DefaultBrowser::UpdateCefKeyModifiers(const RE::ButtonEvent* a_event, const cef_event_flags_t a_flags)
    {
        if (a_event->IsDown())
        {
            m_cefKeyModifiers |= a_flags;
        }
        else if (a_event->IsUp())
        {
            m_cefKeyModifiers &= ~a_flags;
        }
        m_lastCefMouseEvent.modifiers = m_cefKeyModifiers;
        m_lastCharCefKeyEvent.modifiers = m_cefKeyModifiers;
    }

    void DefaultBrowser::ClearCefKeyModifiers()
    {
        m_cefKeyModifiers = 0;
        m_lastCefMouseEvent.modifiers = 0;
        m_lastCharCefKeyEvent.modifiers = 0;
    }

    void DefaultBrowser::UpdateCefKeyModifiersFromVK(const RE::ButtonEvent* a_event, const std::uint32_t a_vkCode)
    {
        if (a_vkCode >= VK_NUMPAD0 && a_vkCode <= VK_DIVIDE)
        {
            UpdateCefKeyModifiers(a_event, EVENTFLAG_IS_KEY_PAD);
        }
        else
        {
            switch (a_vkCode)
            {
            case VK_CAPITAL:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_CAPS_LOCK_ON);
                break;
            case VK_SHIFT:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_SHIFT_DOWN);
                break;
            case VK_CONTROL:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_CONTROL_DOWN);
                break;
            case VK_MENU:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_ALT_DOWN);
                break;
            case VK_NUMLOCK:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_NUM_LOCK_ON);
                break;
            case VK_LCONTROL:
            case VK_LMENU:
            case VK_LSHIFT:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_IS_LEFT);
                break;
            case VK_RCONTROL:
            case VK_RMENU:
            case VK_RSHIFT:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_IS_RIGHT);
                break;
            default:
                break;
            }
        }
    }

    void DefaultBrowser::CheckToggleFocusKeys(const RE::ButtonEvent* a_event)
    {
        if (!a_event->IsDown() || m_toggleFocusKeyCode1 == 0 && m_toggleFocusKeyCode2 == 0)
        {
            return;
        }

        const auto keyboard = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();
        const std::uint8_t* keyboardState = keyboard == nullptr ? nullptr : keyboard->curState;
        if (
            keyboardState != nullptr &&
            (m_toggleFocusKeyCode1 == 0 || (keyboardState[m_toggleFocusKeyCode1] & 0x80) != 0) &&
            (m_toggleFocusKeyCode2 == 0 || (keyboardState[m_toggleFocusKeyCode2] & 0x80) != 0))
        {
            SetBrowserFocused(!IsBrowserFocused());
        }
    }

    void DefaultBrowser::CheckToggleVisibleKeys(const RE::ButtonEvent* a_event)
    {
        if (!a_event->IsDown() || m_toggleVisibleKeyCode1 == 0 && m_toggleVisibleKeyCode2 == 0)
        {
            return;
        }

        const auto keyboard = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();
        const std::uint8_t* keyboardState = keyboard == nullptr ? nullptr : keyboard->curState;
        if (
            keyboardState != nullptr &&
            (m_toggleVisibleKeyCode1 == 0 || (keyboardState[m_toggleVisibleKeyCode1] & 0x80) != 0) &&
            (m_toggleVisibleKeyCode2 == 0 || (keyboardState[m_toggleVisibleKeyCode2] & 0x80) != 0))
        {
            SetBrowserVisible(!IsBrowserVisible());
        }
    }

    CefRefPtr<NirnLabCefClient> DefaultBrowser::GetCefClient()
    {
        return m_cefClient;
    }

    bool DefaultBrowser::IsReadyAndLog()
    {
        const auto result = IsBrowserReady();
        if (!result)
        {
            m_logger->info("{}: browser is still loading, try later", NameOf(DefaultBrowser));
        }
        return result;
    }

    void DefaultBrowser::AddFunctionCallbackAndSendMessage(const NL::JS::JSFuncInfo& a_callbackInfo)
    {
        m_jsFuncStorage->AddFunctionCallback(a_callbackInfo);
        const auto browser = m_cefClient->GetBrowser();
        if (browser != nullptr)
        {
            auto cefMessage = CefProcessMessage::Create(IPC_JS_FUNCION_ADD_EVENT);
            auto dictValue = CefDictionaryValue::Create();
            auto listValue = CefListValue::Create();
            listValue->SetSize(1);
            listValue->SetString(0, a_callbackInfo.funcName);
            dictValue->SetList(a_callbackInfo.objectName, listValue);
            cefMessage->GetArgumentList()->SetDictionary(0, dictValue);
            browser->GetMainFrame()->SendProcessMessage(CefProcessId::PID_RENDERER, cefMessage);
        }
    }

    void DefaultBrowser::RemoveFunctionCallbackAndSendMessage(const char* a_objectName, const char* a_funcName)
    {
        m_jsFuncStorage->RemoveFunctionCallback(a_objectName, a_funcName);
        const auto browser = m_cefClient->GetBrowser();
        if (browser != nullptr)
        {
            auto cefMessage = CefProcessMessage::Create(IPC_JS_FUNCTION_REMOVE_EVENT);
            auto dictValue = CefDictionaryValue::Create();
            auto listValue = CefListValue::Create();
            listValue->SetSize(1);
            listValue->SetString(0, a_objectName);
            dictValue->SetList(a_funcName, listValue);
            cefMessage->GetArgumentList()->SetDictionary(0, dictValue);
            browser->GetMainFrame()->SendProcessMessage(CefProcessId::PID_RENDERER, cefMessage);
        }
    }

#pragma region IBrowser

    bool __cdecl DefaultBrowser::IsBrowserReady()
    {
        return m_cefClient != nullptr && m_cefClient->IsBrowserReady();
    }

    bool __cdecl DefaultBrowser::IsPageLoaded()
    {
        return m_isPageLoaded;
    }

    void __cdecl DefaultBrowser::SetBrowserVisible(bool a_value)
    {
        m_cefClient->GetRenderLayer()->SetVisible(a_value);
        if (!a_value)
        {
            SetBrowserFocused(false);
        }
    }

    bool __cdecl DefaultBrowser::IsBrowserVisible()
    {
        return m_cefClient->GetRenderLayer()->GetVisible();
    }

    void __cdecl DefaultBrowser::ToggleBrowserVisibleByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2)
    {
        m_toggleVisibleKeyCode1 = a_keyCode1 < sizeof(RE::BSInputDeviceManager::GetSingleton()->GetKeyboard()->curState) ? a_keyCode1 : 0;
        m_toggleVisibleKeyCode2 = a_keyCode2 < sizeof(RE::BSInputDeviceManager::GetSingleton()->GetKeyboard()->curState) ? a_keyCode2 : 0;
    }

    void __cdecl DefaultBrowser::SetBrowserFocused(bool a_value)
    {
        std::lock_guard locker(m_urlMutex);
        if (!IsPageLoaded())
        {
            m_isFocusedCached = true;
            m_isFocused = a_value;
            return;
        }

        const auto uiMsgQ = RE::UIMessageQueue::GetSingleton();
        if (a_value)
        {
            m_wasCursorOpen = RE::UI::GetSingleton()->pad17D;
            if (uiMsgQ != nullptr)
            {
                uiMsgQ->AddMessage(RE::CursorMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, NULL);
            }
        }
        else
        {
            if (uiMsgQ != nullptr && !m_wasCursorOpen)
            {
                uiMsgQ->AddMessage(RE::CursorMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, NULL);
            }
        }

        m_cefClient->GetBrowser()->GetHost()->SetFocus(a_value);
        m_isFocusedCached = false;
        m_isFocused = a_value;
    }

    bool __cdecl DefaultBrowser::IsBrowserFocused()
    {
        return m_isFocused;
    }

    void __cdecl DefaultBrowser::ToggleBrowserFocusByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2)
    {
        m_toggleFocusKeyCode1 = a_keyCode1 < sizeof(RE::BSInputDeviceManager::GetSingleton()->GetKeyboard()->curState) ? a_keyCode1 : 0;
        m_toggleFocusKeyCode2 = a_keyCode2 < sizeof(RE::BSInputDeviceManager::GetSingleton()->GetKeyboard()->curState) ? a_keyCode2 : 0;
    }

    void __cdecl DefaultBrowser::LoadBrowserURL(const char* a_url, bool a_clearJSFunctions)
    {
        std::lock_guard locker(m_urlMutex);
        m_clearJSFunctions = a_clearJSFunctions;
        if (!IsPageLoaded())
        {
            m_isUrlCached = true;
            m_urlCache = a_url;
            return;
        }

        const auto frame = m_cefClient->GetBrowser()->GetMainFrame();
        if (frame)
        {
            m_isPageLoaded = false;
            frame->LoadURL(CefString(a_url));
        }
        else
        {
            m_logger->error("{}: can't get main frame to load url \"{}\"", NameOf(DefaultBrowser), a_url);
        }
        m_isUrlCached = false;
    }

    void __cdecl DefaultBrowser::ExecuteJavaScript(const char* a_script, const char* a_scriptUrl)
    {
        std::lock_guard locker(m_urlMutex);
        if (!IsPageLoaded())
        {
            m_jsExecCache.push_back({a_script, a_scriptUrl});
            return;
        }

        if (!m_jsExecCache.empty())
        {
            for (auto& jsScript : m_jsExecCache)
            {
                m_cefClient->GetBrowser()->GetMainFrame()->ExecuteJavaScript(std::get<0>(jsScript).c_str(), std::get<1>(jsScript).c_str(), 0);
            }
            m_jsExecCache.clear();
            m_jsExecCache.shrink_to_fit();
        }

        if (a_script != nullptr)
        {
            m_cefClient->GetBrowser()->GetMainFrame()->ExecuteJavaScript(a_script, a_scriptUrl, 0);
        }
    }

    void __cdecl DefaultBrowser::AddFunctionCallback(const NL::JS::JSFuncInfo& a_callbackInfo)
    {
        std::lock_guard locker(m_urlMutex);
        if (!IsPageLoaded())
        {
            for (auto it = m_jsFuncRemoveCache.begin(); it != m_jsFuncRemoveCache.end(); ++it)
            {
                if (std::get<0>(*it) == a_callbackInfo.objectName && std::get<1>(*it) == a_callbackInfo.funcName)
                {
                    m_jsFuncRemoveCache.erase(it);
                }
            }

            m_jsFuncCallbackInfoCache.push_back(a_callbackInfo);
            return;
        }

        AddFunctionCallbackAndSendMessage(a_callbackInfo);
    }

    void __cdecl DefaultBrowser::RemoveFunctionCallback(const char* a_objectName, const char* a_funcName)
    {
        std::lock_guard locker(m_urlMutex);
        if (!IsPageLoaded())
        {
            for (auto it = m_jsFuncCallbackInfoCache.begin(); it != m_jsFuncCallbackInfoCache.end(); ++it)
            {
                if (it->objectNameString == a_objectName && it->funcNameString == a_funcName)
                {
                    m_jsFuncCallbackInfoCache.erase(it);
                }
            }

            if (!m_jsFuncStorage->RemoveFunctionCallback(a_objectName, a_funcName))
            {
                m_jsFuncRemoveCache.push_back({a_objectName, a_funcName});
            }
            return;
        }

        RemoveFunctionCallbackAndSendMessage(a_objectName, a_funcName);
    }

    void __cdecl DefaultBrowser::RemoveFunctionCallback(const NL::JS::JSFuncInfo& a_callbackInfo)
    {
        RemoveFunctionCallback(a_callbackInfo.objectName, a_callbackInfo.funcName);
    }

    void __cdecl DefaultBrowser::ExecEventFunction(const char* a_eventName, const char* a_data)
    {
        const auto browser = m_cefClient->GetBrowser();
        if (IsPageLoaded() && browser != nullptr)
        {
            auto cefMessage = CefProcessMessage::Create(IPC_JS_EVENT_FUNCTION_CALL_EVENT);
            cefMessage->GetArgumentList()->SetString(0, a_eventName);
            cefMessage->GetArgumentList()->SetString(1, a_data);

            browser->GetMainFrame()->SendProcessMessage(CefProcessId::PID_RENDERER, cefMessage);
        }
    }

#pragma endregion

#pragma region RE::MenuEventHandler

    bool DefaultBrowser::CanProcess(RE::InputEvent* a_event)
    {
        return IsBrowserFocused();
    }

    bool DefaultBrowser::ProcessMouseMove(RE::MouseMoveEvent* a_event)
    {
        if (!IsBrowserFocused())
        {
            return false;
        }

        RE::MenuControls::GetSingleton()->ProcessEvent(reinterpret_cast<RE::InputEvent**>(&a_event), nullptr);

        m_lastCefMouseEvent.x = static_cast<int>(m_currentMousePosX);
        m_lastCefMouseEvent.y = static_cast<int>(m_currentMousePosY);
        m_cefClient->GetBrowser()->GetHost()->SendMouseMoveEvent(m_lastCefMouseEvent, false);

        return true;
    }

    bool DefaultBrowser::ProcessButton(RE::ButtonEvent* a_event)
    {
        CheckToggleFocusKeys(a_event);
        CheckToggleVisibleKeys(a_event);

        if (!IsBrowserFocused())
        {
            return false;
        }

        const auto scanCode = a_event->GetIDCode();
        const auto browserHost = m_cefClient->GetBrowser()->GetHost();
        switch (a_event->GetDevice())
        {
        case RE::INPUT_DEVICE::kMouse:
            if (!a_event->IsDown() && !a_event->IsUp())
            {
                return true;
            }

            switch (scanCode)
            {
            case RE::BSWin32MouseDevice::Keys::kWheelUp:
                browserHost->SendMouseWheelEvent(m_lastCefMouseEvent, 0, MOUSE_WHEEL_DELTA);
                break;
            case RE::BSWin32MouseDevice::Keys::kWheelDown:
                browserHost->SendMouseWheelEvent(m_lastCefMouseEvent, 0, -MOUSE_WHEEL_DELTA);
                break;
            case RE::BSWin32MouseDevice::Key::kLeftButton:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_LEFT_MOUSE_BUTTON);
                browserHost->SendMouseClickEvent(m_lastCefMouseEvent, CefBrowserHost::MouseButtonType::MBT_LEFT, a_event->IsUp(), 1);
                break;
            case RE::BSWin32MouseDevice::Key::kRightButton:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_RIGHT_MOUSE_BUTTON);
                browserHost->SendMouseClickEvent(m_lastCefMouseEvent, CefBrowserHost::MouseButtonType::MBT_RIGHT, a_event->IsUp(), 1);
                break;
            case RE::BSWin32MouseDevice::Key::kMiddleButton:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_MIDDLE_MOUSE_BUTTON);
                browserHost->SendMouseClickEvent(m_lastCefMouseEvent, CefBrowserHost::MouseButtonType::MBT_MIDDLE, a_event->IsUp(), 1);
                break;
            default:
                break;
            }
            break;
        case RE::INPUT_DEVICE::kVirtualKeyboard:
        case RE::INPUT_DEVICE::kKeyboard: {
            const auto isKeyStateChanged = a_event->IsDown() || a_event->IsUp();
            if (isKeyStateChanged)
            {
                const auto vkCode = NL::Utils::InputConverter::GetVirtualKey(scanCode);
                UpdateCefKeyModifiersFromVK(a_event, vkCode);
                m_lastCharCefKeyEvent.native_key_code = scanCode;

                if (a_event->IsDown())
                {
                    m_lastScanCode = scanCode;
                    m_lastCharCefKeyEvent.type = cef_key_event_type_t::KEYEVENT_KEYDOWN;
                    m_lastCharCefKeyEvent.windows_key_code = vkCode;
                    m_keyHeldDuration = KEY_FIRST_CHAR_DELAY;
                    browserHost->SendKeyEvent(m_lastCharCefKeyEvent);

                    if (NL::Utils::InputConverter::ShouldConvertToChar(scanCode, vkCode))
                    {
                        m_lastCharCefKeyEvent.type = cef_key_event_type_t::KEYEVENT_CHAR;
                        m_lastCharCefKeyEvent.windows_key_code = NL::Utils::InputConverter::VkCodeToChar(scanCode, vkCode, m_cefKeyModifiers & (EVENTFLAG_SHIFT_DOWN | EVENTFLAG_CAPS_LOCK_ON));
                        browserHost->SendKeyEvent(m_lastCharCefKeyEvent);
                    }

                    //  Shortcuts
                    if (m_cefKeyModifiers & EVENTFLAG_CONTROL_DOWN)
                    {
                        switch (vkCode)
                        {
                        case 0x43:
                            // CTRL-C
                            m_cefClient->GetBrowser()->GetFocusedFrame()->Copy();
                            break;
                        case 0x58:
                            // CTRL-X
                            m_cefClient->GetBrowser()->GetFocusedFrame()->Cut();
                            break;
                        case 0x56:
                            // CTRL-V
                            m_cefClient->GetBrowser()->GetFocusedFrame()->Paste();
                            break;
                        case 0x41:
                            // CTRL-A
                            m_cefClient->GetBrowser()->GetFocusedFrame()->SelectAll();
                            break;
                        default:
                            break;
                        }
                    }
                }
                else if (a_event->IsUp())
                {
                    m_lastCharCefKeyEvent.type = cef_key_event_type_t::KEYEVENT_KEYUP;
                    m_lastCharCefKeyEvent.windows_key_code = vkCode;
                    browserHost->SendKeyEvent(m_lastCharCefKeyEvent);
                    m_lastScanCode = 0;
                }
            }

            if (scanCode == m_lastScanCode && a_event->IsPressed() && (a_event->HeldDuration() - m_keyHeldDuration) > KEY_CHAR_REPEAT_DELAY)
            {
                m_keyHeldDuration = a_event->HeldDuration();

                const auto vkCode = NL::Utils::InputConverter::GetVirtualKey(scanCode);
                m_lastScanCode = scanCode;
                m_lastCharCefKeyEvent.type = cef_key_event_type_t::KEYEVENT_KEYDOWN;
                m_lastCharCefKeyEvent.windows_key_code = vkCode;
                browserHost->SendKeyEvent(m_lastCharCefKeyEvent);

                if (NL::Utils::InputConverter::ShouldConvertToChar(scanCode, vkCode))
                {
                    m_lastCharCefKeyEvent.type = cef_key_event_type_t::KEYEVENT_CHAR;
                    m_lastCharCefKeyEvent.windows_key_code = NL::Utils::InputConverter::VkCodeToChar(scanCode, vkCode, m_cefKeyModifiers & (EVENTFLAG_SHIFT_DOWN | EVENTFLAG_CAPS_LOCK_ON));
                    browserHost->SendKeyEvent(m_lastCharCefKeyEvent);
                }
            }
            break;
        }
        default:
            break;
        }

        return true;
    }

#pragma endregion

}
