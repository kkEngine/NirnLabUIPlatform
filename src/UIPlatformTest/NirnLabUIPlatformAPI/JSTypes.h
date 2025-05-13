#pragma once

namespace NL::JS
{
    using JSFuncCallback = void (*)(const char** a_args, int a_argsCount);

    struct JSFuncCallbackData
    {
        JSFuncCallback callback = nullptr;
        bool executeInGameThread = true;
        bool isEventFunction = false;
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
        JSFuncInfoString(const char* a_objectName, const char* a_funcName)
            : objectNameString(a_objectName),
              funcNameString(a_funcName)
        {
            objectName = objectNameString.c_str();
            funcName = funcNameString.c_str();
        }
        JSFuncInfoString(const JSFuncInfo& a_info)
            : objectNameString(a_info.objectName),
              funcNameString(a_info.funcName)
        {
            objectName = objectNameString.c_str();
            funcName = funcNameString.c_str();
            callbackData = a_info.callbackData;
        }
        JSFuncInfoString(JSFuncInfoString& a_other)
        {
            objectNameString = a_other.objectNameString;
            funcNameString = a_other.funcNameString;
            objectName = objectNameString.c_str();
            funcName = funcNameString.c_str();
            callbackData = a_other.callbackData;
        }
        JSFuncInfoString(JSFuncInfoString&& a_other)
        {
            objectNameString = std::move(a_other.objectNameString);
            funcNameString = std::move(a_other.funcNameString);
            objectName = objectNameString.c_str();
            funcName = funcNameString.c_str();
            callbackData = a_other.callbackData;

            a_other.objectName = nullptr;
            a_other.funcName = nullptr;
        }

        std::string objectNameString;
        std::string funcNameString;
    };
}
