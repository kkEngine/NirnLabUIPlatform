#pragma once

#include "PCH.h"

namespace NL::Converters
{
    class CefValueToJSONConverter final
    {
      public:
        static void WriteJsonArray(nlohmann::json& a_json, const CefRefPtr<CefListValue>& a_value);
        static void WriteJsonObject(nlohmann::json& a_json, const CefRefPtr<CefDictionaryValue>& a_value);
        static std::shared_ptr<std::vector<std::string>> ConvertToJSONStringArgs(const CefRefPtr<CefListValue>& a_value);
        static std::vector<const char*> ConvertToCharArray(const std::shared_ptr<std::vector<std::string>>& a_strings);
    };
}
