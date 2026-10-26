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
            REL::Relocation<std::uintptr_t> winMain{RELOCATION_ID(35545, 36544), REL::VariantOffset(0x35, 0x1AE, 0)};
            _Shutdown = winMain.write_call<5>(&Shutdown);

            return true;
        }
    };
}
