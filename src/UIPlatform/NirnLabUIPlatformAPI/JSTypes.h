#pragma once

namespace NL::JS
{
    using JSFuncCallback = void (*)(const char** a_args, int a_argsCount);

    struct JSFuncCallbackData
    {
        JSFuncCallback callback = nullptr;
        bool executeInGameThread = true;
    };
}
