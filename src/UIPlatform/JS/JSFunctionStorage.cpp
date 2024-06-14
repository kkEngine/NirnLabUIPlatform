#include "JSFunctionStorage.h"

namespace NL::JS
{
    void JSFunctionStorage::QueueAddFunctionCallback(const std::string& a_objectName, const std::string& a_funcName, JSFuncCallbackData a_callbackData)
    {
        {
            std::lock_guard<std::mutex> lock(m_dequeMutex);
            const auto newItem = std::make_shared<QueueItem>(QueueItem::OpCodes::AddOrReplace, a_objectName, a_funcName, a_callbackData);
            m_deque.push_back(newItem);
        }
        OnQueueItemAdded();
    }

    void JSFunctionStorage::QueueRemoveFunctionCallback(const std::string& a_objectName, const std::string& a_funcName)
    {
        std::lock_guard<std::mutex> lock(m_dequeMutex);
        for (auto it = m_deque.begin(); it != m_deque.end(); ++it)
        {
            if (it->get()->objectName == a_objectName && it->get()->functionName == a_funcName)
            {
                it->get()->opCode = JSFunctionStorage::QueueItem::OpCodes::Remove;
                std::shared_ptr<QueueItem> item;
                m_deque.erase(it)->swap(item);
                m_deque.push_back(item);
            }
        }
    }

    std::shared_ptr<JSFunctionStorage::QueueItem> JSFunctionStorage::GetNextQueueItem()
    {
        std::lock_guard<std::mutex> lock(m_dequeMutex);
        if (m_deque.empty())
        {
            return nullptr;
        }

        const auto item = m_deque.front();
        m_deque.pop_front();
        return item;
    }

    void JSFunctionStorage::ProcessQueue()
    {
        auto qItem = GetNextQueueItem();
        while (qItem != nullptr)
        {
            switch (qItem->opCode)
            {
            case QueueItem::OpCodes::AddOrReplace:
                AddFunctionCallback(qItem->objectName, qItem->functionName, qItem->functionCallbackData);
                break;
            case QueueItem::OpCodes::Remove:
                RemoveFunctionCallback(qItem->objectName, qItem->functionName);
                break;
            case QueueItem::OpCodes::Clear:
                ClearFunctionCallback();
                break;
            default:
                break;
            }

            qItem = GetNextQueueItem();
        }
    }

    bool JSFunctionStorage::AddFunctionCallback(const std::string& a_objectName, const std::string& a_funcName, JSFuncCallbackData a_callbackData)
    {
        std::lock_guard<std::mutex> lock(m_funcCallbackMapMutex);
        const auto objIt = m_funcCallbackMap.find(a_objectName);
        if (objIt == m_funcCallbackMap.end())
        {
            m_funcCallbackMap.insert({a_objectName, {{a_funcName, a_callbackData}}});
            return true;
        }

        const auto funcIt = objIt->second.find(a_funcName);
        if (funcIt == objIt->second.end())
        {
            objIt->second.insert({a_funcName, a_callbackData});
            return true;
        }

        funcIt->second = a_callbackData;
        return false;
    }

    bool JSFunctionStorage::RemoveFunctionCallback(const std::string& a_objectName, const std::string& a_funcName)
    {
        std::lock_guard<std::mutex> lock(m_funcCallbackMapMutex);
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
        std::lock_guard<std::mutex> lock(m_funcCallbackMapMutex);
        m_funcCallbackMap.clear();
    }

    JSFuncCallbackData JSFunctionStorage::GetFunctionCallbackData(const std::string& a_objectName, const std::string& a_funcName)
    {
        std::lock_guard<std::mutex> lock(m_funcCallbackMapMutex);
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

    void JSFunctionStorage::ExecuteFunctionCallback(const std::string& a_objectName, const std::string& a_funcName, const std::shared_ptr<char*[]> a_args, const size_t a_argsCount)
    {
        const auto callbackData = GetFunctionCallbackData(a_objectName, a_funcName);
        if (callbackData.callback == nullptr)
        {
            spdlog::error("{}: function callback is nullptr for {}.{}", NameOf(JSFunctionStorage), a_objectName.c_str(), a_funcName.c_str());
            return;
        }

        if (callbackData.executeInGameThread)
        {
            SKSE::GetTaskInterface()->AddTask([=]() {
                callbackData.callback(a_args.get(), a_argsCount);
            });
        }
        else
        {
            std::lock_guard<std::mutex> lock(m_funcCallbackMapMutex);
            callbackData.callback(a_args.get(), a_argsCount);
        }
    }

    CefRefPtr<CefDictionaryValue> JSFunctionStorage::ConvertToCefDictionary()
    {
        std::lock_guard<std::mutex> lock(m_funcCallbackMapMutex);
        const auto result = CefDictionaryValue::Create();

        for (const auto& obj : m_funcCallbackMap)
        {
            CefRefPtr<CefListValue> list(CefListValue::Create());
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
