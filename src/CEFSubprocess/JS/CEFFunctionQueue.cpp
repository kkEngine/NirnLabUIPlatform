#include "CEFFunctionQueue.h"

namespace NL::JS
{
    void CEFFunctionQueue::AddFunction(const CefString& a_objectName, const CefString& a_functionName)
    {
        std::lock_guard<std::mutex> lock(m_syncMutex);
        m_funcDeque.push_back(std::make_shared<CEFFunctionQueue::Item>(a_objectName, a_functionName));
    }

    void CEFFunctionQueue::Clear()
    {
        std::lock_guard<std::mutex> lock(m_syncMutex);
        m_funcDeque.clear();
    }

    std::shared_ptr<CEFFunctionQueue::Item> CEFFunctionQueue::PopNext()
    {
        std::lock_guard<std::mutex> lock(m_syncMutex);
        if (m_funcDeque.empty())
        {
            return nullptr;
        }

        const auto result = m_funcDeque.front();
        m_funcDeque.pop_front();
        return result;
    }

    size_t CEFFunctionQueue::GetSize()
    {
        return m_funcDeque.size();
    }
}
