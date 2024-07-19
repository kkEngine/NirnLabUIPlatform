#include "CEFFunctionQueue.h"

namespace NL::JS
{
    void CEFFunctionQueue::AddFunction(const CefString& a_objectName, const CefString& a_functionName)
    {
        std::lock_guard<std::mutex> lock(m_syncMutex);
        m_funcDeque.push_back(std::make_shared<CEFFunctionQueue::Item>(a_objectName, a_functionName));
    }

    std::shared_ptr<CEFFunctionQueue::Item> CEFFunctionQueue::PopNext()
    {
        if (m_funcDeque.empty())
        {
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(m_syncMutex);
        const auto result = m_funcDeque.front();
        m_funcDeque.pop_front();
        return result;
    }
}
