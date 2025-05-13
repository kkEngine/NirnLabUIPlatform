#pragma once

#include "PCH.h"
#include "NirnLabUIPlatformAPI/API.h"

namespace NL::JS
{
    struct JSEventFuncInfo
    {
        std::string objectName;
        std::string funcName;

        static JSEventFuncInfo CreateFromFuncInfo(const JSFuncInfo& a_funcInfo)
        {
            return {a_funcInfo.objectName, a_funcInfo.funcName};
        }
    };
}
