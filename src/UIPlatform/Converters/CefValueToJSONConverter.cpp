#include "CefValueToJSONConverter.h"

namespace NL::Converters
{
    void CefValueToJSONConverter::WriteJsonArray(nlohmann::json& a_json, const CefRefPtr<CefListValue>& a_value)
    {
        using namespace nlohmann::literals;

        a_json = nlohmann::json::array();

        for (size_t i = 0; i < a_value->GetSize(); ++i)
        {
            switch (a_value->GetType(i))
            {
            case CefValueType::VTYPE_NULL:
                a_json += nullptr;
                break;
            case CefValueType::VTYPE_BOOL:
                a_json += a_value->GetBool(i);
                break;
            case CefValueType::VTYPE_INT:
                a_json += a_value->GetInt(i);
                break;
            case CefValueType::VTYPE_DOUBLE:
                a_json += a_value->GetDouble(i);
                break;
            case CefValueType::VTYPE_STRING:
                a_json += a_value->GetString(i);
                break;
            case CefValueType::VTYPE_DICTIONARY: {
                nlohmann::json object;
                WriteJsonObject(object, a_value->GetDictionary(i));
                a_json += object;
                break;
            }
            case CefValueType::VTYPE_LIST: {
                nlohmann::json list;
                WriteJsonArray(list, a_value->GetList(i));
                a_json += list;
                break;
            }
            case CefValueType::VTYPE_INVALID:
            case CefValueType::VTYPE_BINARY:
                break;
            default:
                break;
            }
        }
    }

    void CefValueToJSONConverter::WriteJsonObject(nlohmann::json& a_json, const CefRefPtr<CefDictionaryValue>& a_value)
    {
        using namespace nlohmann::literals;

        a_json = nlohmann::json::object();

        CefDictionaryValue::KeyList keys;
        if (!a_value->GetKeys(keys))
        {
            return;
        }

        for (const auto& key : keys)
        {
            switch (a_value->GetType(key))
            {
            case CefValueType::VTYPE_NULL:
                a_json[key] = nullptr;
                break;
            case CefValueType::VTYPE_BOOL:
                a_json[key] = a_value->GetBool(key);
                break;
            case CefValueType::VTYPE_INT:
                a_json[key] = a_value->GetInt(key);
                break;
            case CefValueType::VTYPE_DOUBLE:
                a_json[key] = a_value->GetDouble(key);
                break;
            case CefValueType::VTYPE_STRING:
                a_json[key] = a_value->GetString(key);
                break;
            case CefValueType::VTYPE_DICTIONARY: {
                nlohmann::json object;
                WriteJsonObject(object, a_value->GetDictionary(key));
                a_json[key] = object;
                break;
            }
            case CefValueType::VTYPE_LIST: {
                nlohmann::json list;
                WriteJsonArray(list, a_value->GetList(key));
                a_json[key] = list;
                break;
            }
            case CefValueType::VTYPE_INVALID:
            case CefValueType::VTYPE_BINARY:
                break;
            default:
                break;
            }
        }
    }

    std::shared_ptr<std::vector<std::string>> CefValueToJSONConverter::ConvertToJSONStringArgs(const CefRefPtr<CefListValue>& a_value)
    {
        auto result = std::make_shared<std::vector<std::string>>();
        result->reserve(a_value->GetSize());

        nlohmann::json json;
        for (size_t i = 0; i < a_value->GetSize(); ++i)
        {
            json.clear();
            switch (a_value->GetType(i))
            {
            case CefValueType::VTYPE_NULL:
                json = nullptr;
                break;
            case CefValueType::VTYPE_BOOL:
                json = a_value->GetBool(i);
                break;
            case CefValueType::VTYPE_INT:
                json = a_value->GetInt(i);
                break;
            case CefValueType::VTYPE_DOUBLE:
                json = a_value->GetDouble(i);
                break;
            case CefValueType::VTYPE_STRING:
                json = a_value->GetString(i);
                break;
            case CefValueType::VTYPE_DICTIONARY: {
                WriteJsonObject(json, a_value->GetDictionary(i));
                break;
            }
            case CefValueType::VTYPE_LIST: {
                WriteJsonArray(json, a_value->GetList(i));
                break;
            }
            case CefValueType::VTYPE_INVALID:
            case CefValueType::VTYPE_BINARY:
                break;
            default:
                break;
            }

            result->push_back(json.dump());
        }

        return result;
    }

    std::vector<const char*> CefValueToJSONConverter::ConvertToCharArray(const std::shared_ptr<std::vector<std::string>>& a_strings)
    {
        std::vector<const char*> result;
        result.reserve(a_strings->size());

        for (const auto& str : *a_strings.get())
        {
            result.push_back(str.data());
        }
        
        return result;
    }
}
