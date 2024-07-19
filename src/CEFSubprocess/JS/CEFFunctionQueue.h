#pragma once

#include "PCH.h"

namespace NL::JS
{
    class CEFFunctionQueue
    {
      public:
        struct Item
        {
            CefString objectName = "";
            CefString functionName = "";
        };

      protected:
        std::mutex m_syncMutex;
        std::deque<std::shared_ptr<Item>> m_funcDeque;

      public:
        void AddFunction(const CefString& a_objectName, const CefString& a_functionName);
        std::shared_ptr<Item> PopNext();
    };
}
