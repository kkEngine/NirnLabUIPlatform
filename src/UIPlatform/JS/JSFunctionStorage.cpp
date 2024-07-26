#include "JSFunctionStorage.h"

namespace NL::JS
{
    std::recursive_mutex& JSFunctionStorage::GetCallbackMutex()
    {
        return m_funcCallbackMapMutex;
    }

    bool JSFunctionStorage::AddFunctionCallback(const NL::JS::JSFuncInfo& a_funcInfo)
    {
        if (a_funcInfo.objectName == nullptr || a_funcInfo.funcName == nullptr)
        {
            return false;
        }

        std::lock_guard lock(m_funcCallbackMapMutex);
        const auto objIt = m_funcCallbackMap.find(a_funcInfo.objectName);
        if (objIt == m_funcCallbackMap.cend())
        {
            m_funcCallbackMap.insert({a_funcInfo.objectName, {{a_funcInfo.funcName, a_funcInfo.callbackData}}});
            return true;
        }

        const auto funcIt = objIt->second.find(a_funcInfo.funcName);
        if (funcIt == objIt->second.cend())
        {
            objIt->second.insert({a_funcInfo.funcName, a_funcInfo.callbackData});
            return true;
        }

        funcIt->second = a_funcInfo.callbackData;
        return false;
    }

    bool JSFunctionStorage::RemoveFunctionCallback(const std::string& a_objectName, const std::string& a_funcName)
    {
        std::lock_guard lock(m_funcCallbackMapMutex);
        const auto objIt = m_funcCallbackMap.find(a_objectName);
        if (objIt == m_funcCallbackMap.end())
        {
            return false;
        }

        const auto funcIt = objIt->second.find(a_funcName);
        if (funcIt == objIt->second.end())
        {
            return false;
        }

        objIt->second.erase(funcIt);
        return true;
    }

    void JSFunctionStorage::ClearFunctionCallback()
    {
        std::lock_guard lock(m_funcCallbackMapMutex);
        m_funcCallbackMap.clear();
    }

    JSFuncCallbackData JSFunctionStorage::GetFunctionCallbackData(const std::string& a_objectName, const std::string& a_funcName)
    {
        std::lock_guard lock(m_funcCallbackMapMutex);
        const auto objIt = m_funcCallbackMap.find(a_objectName);
        if (objIt == m_funcCallbackMap.end())
        {
            return {};
        }

        const auto funcIt = objIt->second.find(a_funcName);
        if (funcIt == objIt->second.end())
        {
            return {};
        }

        return funcIt->second;
    }

    void JSFunctionStorage::ExecuteFunctionCallback(const std::string& a_objectName,
                                                    const std::string& a_funcName,
                                                    std::shared_ptr<std::vector<std::string>> a_funcArgs,
                                                    std::shared_ptr<JSFunctionStorage> a_storage)
    {
        std::lock_guard lock(m_funcCallbackMapMutex);
        const auto callbackData = GetFunctionCallbackData(a_objectName, a_funcName);
        if (callbackData.callback == nullptr)
        {
            spdlog::debug("{}: function callback is nullptr for {}.{}", NameOf(JSFunctionStorage), a_objectName.c_str(), a_funcName.c_str());
            return;
        }

        if (callbackData.executeInGameThread)
        {
            SKSE::GetTaskInterface()->AddTask([=]() {
                std::lock_guard lock(a_storage != nullptr ? a_storage->GetCallbackMutex() : m_funcCallbackMapMutex);
                const auto innerCallbackData = a_storage != nullptr ? a_storage->GetFunctionCallbackData(a_objectName, a_funcName) : GetFunctionCallbackData(a_objectName, a_funcName);

                if (innerCallbackData.callback == nullptr)
                {
                    spdlog::debug("{}: function callback is nullptr for {}.{}", NameOf(JSFunctionStorage), a_objectName.c_str(), a_funcName.c_str());
                    return;
                }

                auto argsCharArray = NL::Converters::CefValueToJSONConverter::ConvertToCharArray(a_funcArgs);
                innerCallbackData.callback(argsCharArray.data(), static_cast<int>(argsCharArray.size()));
            });
        }
        else
        {
            auto argsCharArray = NL::Converters::CefValueToJSONConverter::ConvertToCharArray(a_funcArgs);
            callbackData.callback(argsCharArray.data(), static_cast<int>(argsCharArray.size()));
        }
    }

    size_t JSFunctionStorage::GetSize()
    {
        size_t result = m_funcCallbackMap.size();
        for (const auto& map : m_funcCallbackMap)
        {
            result += map.second.size();
        }
        return result;
    }

    CefRefPtr<CefDictionaryValue> JSFunctionStorage::ConvertToCefDictionary()
    {
        std::lock_guard lock(m_funcCallbackMapMutex);
        const auto result = CefDictionaryValue::Create();

        for (const auto& obj : m_funcCallbackMap)
        {
            auto list = CefListValue::Create();
            list->SetSize(obj.second.size());
            auto funcIdx = 0;
            for (const auto& func : obj.second)
            {
                list->SetString(funcIdx++, func.first);
            }

            result->SetList(obj.first, list);
        }

        return result;
    }
}
