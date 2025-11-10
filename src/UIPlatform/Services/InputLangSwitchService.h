#pragma once

#include "PCH.h"
#include "Common/Singleton.h"
#include "Converters/KeyInputConverter.h"

namespace NL::Services
{
    class InputLangSwitchService : public NL::Common::Singleton<InputLangSwitchService>,
                                   public RE::BSTEventSink<RE::InputEvent*>
    {
      private:
        friend class NL::Common::Singleton<InputLangSwitchService>;
        std::mutex m_switchActiveMutex;
        bool m_isSwitchActive = false;

      public:
        void SetActive(bool a_value);

        // RE::BSTEventSink<RE::InputEvent*>
        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;
    };
}
