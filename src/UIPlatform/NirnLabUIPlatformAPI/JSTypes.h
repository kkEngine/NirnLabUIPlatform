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

    struct JSFuncInfoString : public JSFuncInfo
    {
        JSFuncInfoString() = default;
        JSFuncInfoString(const JSFuncInfo& a_info)
            : objectNameString(a_info.objectName),
              funcNameString(a_info.funcName)
        {
            callbackData = a_info.callbackData;
        }

        std::string objectNameString;
        std::string funcNameString;
    };
}
