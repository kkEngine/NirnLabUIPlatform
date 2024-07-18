#pragma once

#include "PCH.h"
#include "Converters/CefValueToJSONConverter.h"

namespace NL::JS
{
    class JSFunctionStorage
    {
      public:
        struct QueueItem
        {
            enum class OpCodes : std::uint8_t
            {
                AddOrReplace = 0,
                Remove,
                Clear,
            };

            OpCodes opCode = OpCodes::Clear;
            std::string objectName = "";
            std::string functionName = "";
            NL::JS::JSFuncCallbackData functionCallbackData;
        };

      protected:
        std::mutex m_dequeMutex;
        std::deque<std::shared_ptr<QueueItem>> m_deque;

        std::recursive_mutex m_funcCallbackMapMutex;
        std::unordered_map<std::string, std::unordered_map<std::string, NL::JS::JSFuncCallbackData>> m_funcCallbackMap;

      public:
        sigslot::signal<> OnQueueItemAdded;

        std::recursive_mutex& GetCallbackMutex();

        virtual void QueueAddFunctionCallback(const std::string& a_objectName,
                                              const std::string& a_funcName,
                                              JSFuncCallbackData a_callbackData);
        virtual void QueueRemoveFunctionCallback(const std::string& a_objectName, const std::string& a_funcName);
        virtual std::shared_ptr<QueueItem> GetNextQueueItem();

        virtual void ProcessQueue();
        // Returns true if the function is new, otherwise false (replaced)
        virtual bool AddFunctionCallback(const std::string& a_objectName,
                                         const std::string& a_funcName,
                                         JSFuncCallbackData a_callbackData);
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
