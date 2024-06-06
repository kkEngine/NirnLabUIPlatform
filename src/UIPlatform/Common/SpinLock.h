#pragma once

namespace NL::Common
{
    class SpinLock
    {
      protected:
        std::atomic_flag m_locked = ATOMIC_FLAG_INIT;

      public:
        void Lock()
        {
            while (m_locked.test_and_set(std::memory_order_acquire))
            {
            }
        }

        void LockYield()
        {
            while (m_locked.test_and_set(std::memory_order_acquire))
            {
                std::this_thread::yield();
            }
        }

        void Unlock()
        {
            m_locked.clear(std::memory_order_release);
        }
    };
}
