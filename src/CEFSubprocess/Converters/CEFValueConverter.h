#pragma once

#include "PCH.h"

namespace NL::Converters
{
    class CEFValueConverter
    {
    public:
        // Thanks to https://www.magpcss.org/ceforum/viewtopic.php?f=6&t=11104 (fanfeilong)
        static void CefV8Array2ListValue(CefRefPtr<CefV8Value> source, CefRefPtr<CefListValue> target);
        static void CefListValue2V8Array(CefRefPtr<CefListValue> source, CefRefPtr<CefV8Value> target);
        static void CefV8JsonObject2DictionaryValue(CefRefPtr<CefV8Value> source, CefRefPtr<CefDictionaryValue> target);
        static void CefDictionaryValue2V8JsonObject(CefRefPtr<CefDictionaryValue> source, CefRefPtr<CefV8Value> target);

    public:
        /// <summary>
        /// Helper function to convert a CefDictionaryValue to a CefV8Object
        /// </summary>
        /// <param name="dictionary"></param>
        /// <returns></returns>
        static CefRefPtr<CefV8Value> to_v8object(CefRefPtr<CefDictionaryValue> const& dictionary);

    public:
        static CefRefPtr<CefValue> ConvertValue(const CefRefPtr<CefV8Value>& a_v8Value,
                                                std::vector<CefRefPtr<CefV8Value>>& a_objectRefs,
                                                std::unordered_map<std::string, std::uint32_t>& a_warnMap,
                                                CefString& a_exception);
    };
}
