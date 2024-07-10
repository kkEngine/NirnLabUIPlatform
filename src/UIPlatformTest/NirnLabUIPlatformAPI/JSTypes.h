#pragma once

namespace NL::JS
{
    using JSFuncCallback = void (*)(const char** a_args, int a_argsCount);

    struct JSFuncCallbackData
    {
        JSFuncCallback callback = nullptr;
        bool executeInGameThread = true;
    };

    struct JSFuncInfo
    {
        const char* objectName = nullptr;
        const char* funcName = nullptr;
        JSFuncCallbackData callbackData;
    };
}
