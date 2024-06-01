#pragma once

namespace Utils
{
    template<typename T>
    void PushFront(RE::BSTArray<T*>& arr, T* newItem)
    {
        arr.push_back(newItem);
        auto arrSize = arr.size();
        if (arrSize == 1)
            return;

        for (RE::BSTArrayBase::size_type idx = arr.size() - 1; idx > 0; --idx)
        {
            arr[idx] = arr[idx - 1];
        }
        arr[0] = newItem;
    }
}
