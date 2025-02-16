#pragma once

#include "../PCH.h"

namespace NL::Hooks
{
    class ShutdownHook
    {
    public:
        static inline bool IsGameClosing = false;
        static inline sigslot::signal<> OnShutdown;

        static void Shutdown()
        {
            IsGameClosing = true;
            OnShutdown();
            _Shutdown();

            std::exit(EXIT_SUCCESS);
        }

        static inline REL::Relocation<decltype(&Shutdown)> _Shutdown;

        static bool Install()
        {
            static constexpr REL::ID WinMain(36544);

            static REL::Relocation<std::uintptr_t> target{WinMain};
            auto& trampoline = SKSE::GetTrampoline();
            _Shutdown = trampoline.write_call<5>(target.address() + 0x1AE, &Shutdown); // Main::Shutdown

            return true;
        }
    };
}
