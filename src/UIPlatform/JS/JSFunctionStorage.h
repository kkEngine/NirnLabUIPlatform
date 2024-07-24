#pragma once

#include "PCH.h"
#include "Converters/CefValueToJSONConverter.h"

namespace NL::JS
{
    class JSFunctionStorage
    {
      protected:
        std::recursive_mutex m_funcCallbackMapMutex;
        std::unordered_map<std::string, std::unordered_map<std::string, NL::JS::JSFuncCallbackData>> m_funcCallbackMap;

      public:
        sigslot::signal<> OnQueueItemAdded;

        std::recursive_mutex& GetCallbackMutex();
        // Returns true if the function is new, otherwise false (replaced)
        virtual bool AddFunctionCallback(const NL::JS::JSFuncInfo& a_funcInfo);
        // Returns true if the function found and removed, otherwise false
        virtual bool RemoveFunctionCallback(const std::string& a_objectName, const std::string& a_funcName);
        virtual void ClearFunctionCallback();
        virtual JSFuncCallbackData GetFunctionCallbackData(const std::string& a_objectName, const std::string& a_funcName);
        virtual void ExecuteFunctionCallback(const std::string& a_objectName,
                                             const std::string& a_funcName,
                                             std::shared_ptr<std::vector<std::string>> a_funcArgs,
                                             std::shared_ptr<JSFunctionStorage> a_storage = nullptr);

        virtual CefRefPtr<CefDictionaryValue> ConvertToCefDictionary();
    };
}
