#include "InputLangSwitchService.h"

namespace NL::Services
{
    void InputLangSwitchService::SetActive(bool a_value)
    {
        std::lock_guard<std::mutex> lock(m_switchActiveMutex);
        if (m_isSwitchActive == a_value)
        {
            return;
        }

        if (a_value)
        {
            const auto inputEventSource = RE::BSInputDeviceManager::GetSingleton();
            inputEventSource->lock.Lock();
            NL::Utils::PushFront<RE::BSTEventSink<RE::InputEvent*>>(RE::BSInputDeviceManager::GetSingleton()->sinks, this);
            inputEventSource->lock.Unlock();
        }
        else
        {
            RE::BSInputDeviceManager::GetSingleton()->RemoveEventSink(this);
        }
        m_isSwitchActive = a_value;
    }

    RE::BSEventNotifyControl InputLangSwitchService::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
    {
        if (a_event == nullptr || *a_event == nullptr)
        {
            return RE::BSEventNotifyControl::kContinue;
        }

        const auto inputEvent = *a_event;
        if (inputEvent->GetEventType() == RE::INPUT_EVENT_TYPE::kButton && inputEvent->AsButtonEvent()->IsDown())
        {
            const auto kb = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();
            const std::uint8_t* kbState = kb == nullptr ? nullptr : kb->curState;
            if (kbState != nullptr && (kbState[RE::BSKeyboardDevice::Keys::kLeftShift] & 0x80) != 0 &&
                ((kbState[RE::BSKeyboardDevice::Keys::kLeftControl] & 0x80) != 0 ||
                 (kbState[RE::BSKeyboardDevice::Keys::kLeftAlt] & 0x80) != 0))
            {
                NL::Converters::KeyInputConverter::NextKeyboardLayout();
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }
}
