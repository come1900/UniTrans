// come.1.json.cpp

#include "come.1.json.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <limits>
#include <type_traits>

// 使用类型别名
using json = ::nlohmann::json;

// 辅助函数：安全获取字段值（bool类型专用）
template <typename T>
typename std::enable_if<std::is_same<T, bool>::value, bool>::type
get_field_value(const json& j, const std::string& field_name, const T& default_value) {
    if (!j.contains(field_name) || j[field_name].is_null()) {
        return default_value;
    }
    try {
        return j.at(field_name).get<bool>();
    } catch (const nlohmann::json::type_error& e) {
        return default_value;
    } catch (const nlohmann::json::exception& e) {
        return default_value;
    }
}

// 辅助函数：安全获取字段值（整数类型，支持范围检查）
template <typename T>
typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, T>::type
get_field_value(const json& j, const std::string& field_name, const T& default_value) {
    if (!j.contains(field_name) || j[field_name].is_null()) {
        return default_value;
    }
    try {
        const json& value = j.at(field_name);
        
        // 对于整数类型，先检查是否为数字
        if (!value.is_number()) {
            return default_value;
        }
        
        // 先获取为更大的整数类型进行检查
        if (value.is_number_integer()) {
            json::number_integer_t temp_val = value.get<json::number_integer_t>();
            
            // 检查是否在目标类型的范围内
            if (temp_val > static_cast<json::number_integer_t>(std::numeric_limits<T>::max()) ||
                temp_val < static_cast<json::number_integer_t>(std::numeric_limits<T>::min())) {
                // 超出范围，返回默认值
                return default_value;
            }
            
            return static_cast<T>(temp_val);
        } else if (value.is_number_float()) {
            // 浮点数，检查是否为整数且在范围内
            double d_val = value.get<double>();
            if (d_val != std::floor(d_val)) {
                // 有小数部分，不是整数
                return default_value;
            }
            
            json::number_integer_t temp_val = static_cast<json::number_integer_t>(d_val);
            if (temp_val > static_cast<json::number_integer_t>(std::numeric_limits<T>::max()) ||
                temp_val < static_cast<json::number_integer_t>(std::numeric_limits<T>::min())) {
                return default_value;
            }
            
            return static_cast<T>(temp_val);
        }
        
        return default_value;
    } catch (const nlohmann::json::type_error& e) {
        // 类型不匹配（如期望int但得到string）
        return default_value;
    } catch (const nlohmann::json::out_of_range& e) {
        // 值超出目标类型范围
        return default_value;
    } catch (const nlohmann::json::exception& e) {
        // 捕获其他JSON异常（兜底处理）
        return default_value;
    }
}

// 辅助函数：安全获取字段值（非整数类型的通用版本）
template <typename T>
typename std::enable_if<!std::is_integral<T>::value, T>::type
get_field_value(const json& j, const std::string& field_name, const T& default_value) {
    if (!j.contains(field_name) || j[field_name].is_null()) {
        return default_value;
    }
    try {
        return j.at(field_name).get<T>();
    } catch (const nlohmann::json::type_error& e) {
        // 类型不匹配
        return default_value;
    } catch (const nlohmann::json::out_of_range& e) {
        // 值超出目标类型范围
        return default_value;
    } catch (const nlohmann::json::exception& e) {
        // 捕获其他JSON异常（兜底处理）
        return default_value;
    }
}

// ==============================
// EdgeOnline 编解码
// ==============================

void to_json(json& j, const EdgeOnline& msg) {
    j = json{
        {"id", msg.id},
        {"key", msg.key},
        {"type", msg.type},
        {"nonce", msg.nonce},
        {"token", msg.token}
    };
}

void from_json(const json& j, EdgeOnline& msg) {
    msg.id = get_field_value<std::string>(j, "id", "");
    msg.key = get_field_value<std::string>(j, "key", "");
    msg.type = get_field_value<std::string>(j, "type", "");
    msg.nonce = get_field_value<std::string>(j, "nonce", "");
    msg.token = get_field_value<std::string>(j, "token", "");
}

std::string ComeJsonCodec::encode(const EdgeOnline& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        // 捕获所有异常，返回空字符串表示编码失败
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, EdgeOnline& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        // 捕获所有异常，返回 false 表示解码失败
        return false;
    }
}

// ==============================
// AckEdgeOnline 编解码
// ==============================

void to_json(json& j, const AckEdgeOnline& msg) {
    j = json{
        {"type", "ack_device_online"},  // 明确标识消息类型
        {"code", msg.code},
        {"msg", msg.msg},
        {"success", msg.success},
        {"access_token", msg.access_token},
        {"token_type", msg.token_type},
        {"expires_in", msg.expires_in}
    };
}

void from_json(const json& j, AckEdgeOnline& msg) {
    msg.code = get_field_value<int32_t>(j, "code", 0);
    msg.msg = get_field_value<std::string>(j, "msg", "");
    msg.success = get_field_value<bool>(j, "success", false);
    msg.access_token = get_field_value<std::string>(j, "access_token", "");
    msg.token_type = get_field_value<std::string>(j, "token_type", "Bearer");
    msg.expires_in = get_field_value<int32_t>(j, "expires_in", 0);
}

std::string ComeJsonCodec::encode(const AckEdgeOnline& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        // 捕获所有异常，返回空字符串表示编码失败
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckEdgeOnline& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        // 捕获所有异常，返回 false 表示解码失败
        return false;
    }
}

// ==============================
// EdgeReport 编解码
// ==============================

void to_json(json& j, const EdgeReport& msg) {
    j = json{
        {"id", msg.id},
        {"access_token", msg.access_token},
        {"data", msg.data}
    };
}

void from_json(const json& j, EdgeReport& msg) {
    msg.id = get_field_value<std::string>(j, "id", "");
    msg.access_token = get_field_value<std::string>(j, "access_token", "");
    if (j.contains("data") && j["data"].is_object()) {
        try {
            msg.data = j["data"].get<std::map<std::string, std::string>>();
        } catch (...) {
            // 如果转换失败，保持 data 为空 map
            msg.data.clear();
        }
    }
}

std::string ComeJsonCodec::encode(const EdgeReport& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        // 捕获所有异常，返回空字符串表示编码失败
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, EdgeReport& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        // 捕获所有异常，返回 false 表示解码失败
        return false;
    }
}

// ==============================
// AckEdgeReport 编解码
// ==============================

void to_json(json& j, const AckEdgeReport& msg) {
    j = json{
        {"code", msg.code},
        {"msg", msg.msg},
        {"received", msg.received},
        {"id", msg.id},
        {"timestamp", msg.timestamp}
    };
}

void from_json(const json& j, AckEdgeReport& msg) {
    msg.code = get_field_value<int32_t>(j, "code", 0);
    msg.msg = get_field_value<std::string>(j, "msg", "");
    msg.received = get_field_value<bool>(j, "received", false);
    msg.id = get_field_value<std::string>(j, "id", "");
    msg.timestamp = get_field_value<std::string>(j, "timestamp", "");
}

std::string ComeJsonCodec::encode(const AckEdgeReport& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        // 捕获所有异常，返回空字符串表示编码失败
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckEdgeReport& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        // 捕获所有异常，返回 false 表示解码失败
        return false;
    }
}

// ==============================
// ServerCommand 编解码
// ==============================

void to_json(json& j, const ServerCommand& msg) {
    j = json{
        {"id", msg.id},
        {"command_type", msg.command_type},
        {"payload", msg.payload}
    };
}

void from_json(const json& j, ServerCommand& msg) {
    msg.id = get_field_value<std::string>(j, "id", "");
    msg.command_type = get_field_value<std::string>(j, "command_type", "");
    if (j.contains("payload") && j["payload"].is_object()) {
        try {
            msg.payload = j["payload"].get<std::map<std::string, std::string>>();
        } catch (...) {
            // 如果转换失败，保持 payload 为空 map
            msg.payload.clear();
        }
    }
}

std::string ComeJsonCodec::encode(const ServerCommand& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        // 捕获所有异常，返回空字符串表示编码失败
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, ServerCommand& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        // 捕获所有异常，返回 false 表示解码失败
        return false;
    }
}

// ==============================
// AckServerCommand 编解码
// ==============================

void to_json(json& j, const AckServerCommand& msg) {
    j = json{
        {"code", msg.code},
        {"msg", msg.msg},
        {"id", msg.id},
        {"command_type", msg.command_type},
        {"handled", msg.handled},
        {"result", msg.result}
    };
}

void from_json(const json& j, AckServerCommand& msg) {
    msg.code = get_field_value<int32_t>(j, "code", 0);
    msg.msg = get_field_value<std::string>(j, "msg", "");
    msg.id = get_field_value<std::string>(j, "id", "");
    msg.command_type = get_field_value<std::string>(j, "command_type", "");
    msg.handled = get_field_value<bool>(j, "handled", false);
    msg.result = get_field_value<std::string>(j, "result", "");
}

std::string ComeJsonCodec::encode(const AckServerCommand& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        // 捕获所有异常，返回空字符串表示编码失败
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckServerCommand& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        // 捕获所有异常，返回 false 表示解码失败
        return false;
    }
}

// ==============================
// EdgeHeartbeat 编解码
// ==============================

void to_json(json& j, const EdgeHeartbeat& msg) {
    j = json{
        {"id", msg.id},
        {"access_token", msg.access_token},
        {"timestamp", msg.timestamp}
    };
}

void from_json(const json& j, EdgeHeartbeat& msg) {
    msg.id = get_field_value<std::string>(j, "id", "");
    msg.access_token = get_field_value<std::string>(j, "access_token", "");
    msg.timestamp = get_field_value<std::string>(j, "timestamp", "");
}

std::string ComeJsonCodec::encode(const EdgeHeartbeat& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, EdgeHeartbeat& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// AckEdgeHeartbeat 编解码
// ==============================

void to_json(json& j, const AckEdgeHeartbeat& msg) {
    j = json{
        {"type", "ack_device_heartbeat"},  // 明确标识消息类型
        {"code", msg.code},
        {"msg", msg.msg},
        {"id", msg.id},
        {"timestamp", msg.timestamp}
    };
}

void from_json(const json& j, AckEdgeHeartbeat& msg) {
    msg.code = get_field_value<int32_t>(j, "code", 0);
    msg.msg = get_field_value<std::string>(j, "msg", "");
    msg.id = get_field_value<std::string>(j, "id", "");
    msg.timestamp = get_field_value<std::string>(j, "timestamp", "");
}

std::string ComeJsonCodec::encode(const AckEdgeHeartbeat& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckEdgeHeartbeat& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// ConfigUpdate 编解码
// ==============================

void to_json(json& j, const ConfigUpdate& msg) {
    j = json{
        {"edge_id", msg.edge_id},
        {"config_type", msg.config_type},
        {"config_content", msg.config_content},
        {"version", msg.version},
        {"access_token", msg.access_token}
    };
}

void from_json(const json& j, ConfigUpdate& msg) {
    msg.edge_id = get_field_value<std::string>(j, "edge_id", "");
    msg.config_type = get_field_value<std::string>(j, "config_type", "");
    msg.config_content = get_field_value<std::string>(j, "config_content", "");
    msg.version = get_field_value<int32_t>(j, "version", 0);
    msg.access_token = get_field_value<std::string>(j, "access_token", "");
}

std::string ComeJsonCodec::encode(const ConfigUpdate& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, ConfigUpdate& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// AckConfigUpdate 编解码
// ==============================

void to_json(json& j, const AckConfigUpdate& msg) {
    j = json{
        {"code", msg.code},
        {"msg", msg.msg},
        {"edge_id", msg.edge_id},
        {"config_type", msg.config_type},
        {"applied", msg.applied},
        {"version", msg.version},
        {"result", msg.result}
    };
}

void from_json(const json& j, AckConfigUpdate& msg) {
    msg.code = get_field_value<int32_t>(j, "code", 0);
    msg.msg = get_field_value<std::string>(j, "msg", "");
    msg.edge_id = get_field_value<std::string>(j, "edge_id", "");
    msg.config_type = get_field_value<std::string>(j, "config_type", "");
    msg.applied = get_field_value<bool>(j, "applied", false);
    msg.version = get_field_value<int32_t>(j, "version", 0);
    msg.result = get_field_value<std::string>(j, "result", "");
}

std::string ComeJsonCodec::encode(const AckConfigUpdate& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckConfigUpdate& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// IngressLoadReport 编解码
// ==============================

void to_json(json& j, const IngressLoadReport& msg) {
    j = json{
        {"ingress_id", msg.ingress_id},
        {"connection_count", msg.connection_count},
        {"cpu_usage", msg.cpu_usage},
        {"memory_usage", msg.memory_usage},
        {"timestamp", msg.timestamp},
        {"access_token", msg.access_token}
    };
}

void from_json(const json& j, IngressLoadReport& msg) {
    msg.ingress_id = get_field_value<std::string>(j, "ingress_id", "");
    msg.connection_count = get_field_value<int32_t>(j, "connection_count", 0);
    msg.cpu_usage = get_field_value<double>(j, "cpu_usage", 0.0);
    msg.memory_usage = get_field_value<double>(j, "memory_usage", 0.0);
    msg.timestamp = get_field_value<std::string>(j, "timestamp", "");
    msg.access_token = get_field_value<std::string>(j, "access_token", "");
}

std::string ComeJsonCodec::encode(const IngressLoadReport& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, IngressLoadReport& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// AckIngressLoadReport 编解码
// ==============================

void to_json(json& j, const AckIngressLoadReport& msg) {
    j = json{
        {"code", msg.code},
        {"msg", msg.msg},
        {"ingress_id", msg.ingress_id},
        {"timestamp", msg.timestamp}
    };
}

void from_json(const json& j, AckIngressLoadReport& msg) {
    msg.code = get_field_value<int32_t>(j, "code", 0);
    msg.msg = get_field_value<std::string>(j, "msg", "");
    msg.ingress_id = get_field_value<std::string>(j, "ingress_id", "");
    msg.timestamp = get_field_value<std::string>(j, "timestamp", "");
}

std::string ComeJsonCodec::encode(const AckIngressLoadReport& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckIngressLoadReport& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// ManagerCommand 编解码
// ==============================

void to_json(json& j, const ManagerCommand& msg) {
    j = json{
        {"command_type", msg.command_type},
        {"target_id", msg.target_id},
        {"params", msg.params},
        {"access_token", msg.access_token}
    };
}

void from_json(const json& j, ManagerCommand& msg) {
    msg.command_type = get_field_value<std::string>(j, "command_type", "");
    msg.target_id = get_field_value<std::string>(j, "target_id", "");
    msg.access_token = get_field_value<std::string>(j, "access_token", "");
    if (j.contains("params") && j["params"].is_object()) {
        try {
            msg.params = j["params"].get<std::map<std::string, std::string>>();
        } catch (...) {
            msg.params.clear();
        }
    }
}

std::string ComeJsonCodec::encode(const ManagerCommand& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, ManagerCommand& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// AckManagerCommand 编解码
// ==============================

void to_json(json& j, const AckManagerCommand& msg) {
    j = json{
        {"code", msg.code},
        {"msg", msg.msg},
        {"command_type", msg.command_type},
        {"target_id", msg.target_id},
        {"executed", msg.executed},
        {"result", msg.result}
    };
}

void from_json(const json& j, AckManagerCommand& msg) {
    msg.code = get_field_value<int32_t>(j, "code", 0);
    msg.msg = get_field_value<std::string>(j, "msg", "");
    msg.command_type = get_field_value<std::string>(j, "command_type", "");
    msg.target_id = get_field_value<std::string>(j, "target_id", "");
    msg.executed = get_field_value<bool>(j, "executed", false);
    msg.result = get_field_value<std::string>(j, "result", "");
}

std::string ComeJsonCodec::encode(const AckManagerCommand& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckManagerCommand& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// EdgeRegisterRequest 编解码
// ==============================

void to_json(json& j, const EdgeRegisterRequest& msg) {
    j = json{
        {"edge_id", msg.edge_id},
        {"edge_type", msg.edge_type},
        {"edge_key", msg.edge_key}
    };
    if (!msg.local_ip.empty()) {
        j["local_ip"] = msg.local_ip;
    }
}

void from_json(const json& j, EdgeRegisterRequest& msg) {
    msg.edge_id = get_field_value<std::string>(j, "edge_id", "");
    msg.edge_type = get_field_value<std::string>(j, "edge_type", "");
    msg.edge_key = get_field_value<std::string>(j, "edge_key", "");
    msg.local_ip = get_field_value<std::string>(j, "local_ip", "");
}

std::string ComeJsonCodec::encode(const EdgeRegisterRequest& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, EdgeRegisterRequest& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// IngressInfo 编解码（嵌套结构）
// ==============================

void to_json(json& j, const IngressInfo& info) {
    j = json{
        {"host", info.host},
        {"port", info.port}
    };
}

void from_json(const json& j, IngressInfo& info) {
    info.host = get_field_value<std::string>(j, "host", "");
    info.port = get_field_value<int32_t>(j, "port", 0);
}

// ==============================
// AckEdgeRegister 编解码
// ==============================

void to_json(json& j, const AckEdgeRegister& msg) {
    j = json{
        {"code", msg.code},
        {"message", msg.msg},
        {"ingress", msg.ingress},
        {"token", msg.token},
        {"edge_public_ip", msg.edge_public_ip}
    };
}

void from_json(const json& j, AckEdgeRegister& msg) {
    msg.code = get_field_value<int32_t>(j, "code", 0);
    msg.msg = get_field_value<std::string>(j, "message", "");
    if (j.contains("ingress") && j["ingress"].is_object()) {
        try {
            from_json(j["ingress"], msg.ingress);
        } catch (...) {
            // 如果解析失败，保持默认值
        }
    }
    msg.token = get_field_value<std::string>(j, "token", "");
    msg.edge_public_ip = get_field_value<std::string>(j, "edge_public_ip", "");
}

std::string ComeJsonCodec::encode(const AckEdgeRegister& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckEdgeRegister& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// JsonValue 实现（封装 nlohmann::json）
// ==============================

JsonValue::JsonValue() : m_impl(nullptr) {
    m_impl = new json();
}

JsonValue::JsonValue(const JsonValue& other) : m_impl(nullptr) {
    if (other.m_impl) {
        m_impl = new json(*static_cast<json*>(other.m_impl));
    } else {
        m_impl = new json();
    }
}

JsonValue::JsonValue(JsonValue&& other) noexcept : m_impl(other.m_impl) {
    other.m_impl = nullptr;
}

JsonValue::~JsonValue() {
    if (m_impl) {
        delete static_cast<json*>(m_impl);
        m_impl = nullptr;
    }
}

JsonValue& JsonValue::operator=(const JsonValue& other) {
    if (this != &other) {
        if (m_impl) {
            delete static_cast<json*>(m_impl);
        }
        if (other.m_impl) {
            m_impl = new json(*static_cast<json*>(other.m_impl));
        } else {
            m_impl = new json();
        }
    }
    return *this;
}

JsonValue& JsonValue::operator=(JsonValue&& other) noexcept {
    if (this != &other) {
        if (m_impl) {
            delete static_cast<json*>(m_impl);
        }
        m_impl = other.m_impl;
        other.m_impl = nullptr;
    }
    return *this;
}

JsonValue::JsonValue(void* impl) : m_impl(impl) {
}

JsonValue JsonValue::parse(const std::string& jsonStr) {
    try {
        json* j = new json(json::parse(jsonStr));
        return JsonValue(j);
    } catch (...) {
        return JsonValue();
    }
}

std::string JsonValue::dump(int indent) const {
    if (!m_impl) {
        return "null";
    }
    try {
        json* j = static_cast<json*>(m_impl);
        if (indent >= 0) {
            return j->dump(indent);
        } else {
            return j->dump();
        }
    } catch (...) {
        return "null";
    }
}

bool JsonValue::isNull() const {
    if (!m_impl) return true;
    try {
        return static_cast<json*>(m_impl)->is_null();
    } catch (...) {
        return true;
    }
}

bool JsonValue::isEmpty() const {
    if (!m_impl || isNull()) return true;
    try {
        json* j = static_cast<json*>(m_impl);
        if (j->is_object()) {
            return j->empty();
        } else if (j->is_array()) {
            return j->empty();
        }
        return false;
    } catch (...) {
        return true;
    }
}

bool JsonValue::isObject() const {
    if (!m_impl) return false;
    try {
        return static_cast<json*>(m_impl)->is_object();
    } catch (...) {
        return false;
    }
}

bool JsonValue::isArray() const {
    if (!m_impl) return false;
    try {
        return static_cast<json*>(m_impl)->is_array();
    } catch (...) {
        return false;
    }
}

bool JsonValue::isString() const {
    if (!m_impl) return false;
    try {
        return static_cast<json*>(m_impl)->is_string();
    } catch (...) {
        return false;
    }
}

bool JsonValue::isNumber() const {
    if (!m_impl) return false;
    try {
        json* j = static_cast<json*>(m_impl);
        return j->is_number();
    } catch (...) {
        return false;
    }
}

bool JsonValue::isBoolean() const {
    if (!m_impl) return false;
    try {
        return static_cast<json*>(m_impl)->is_boolean();
    } catch (...) {
        return false;
    }
}

std::string JsonValue::getString(const std::string& key, const std::string& defaultVal) const {
    if (!m_impl) return defaultVal;
    try {
        json* j = static_cast<json*>(m_impl);
        if (j->contains(key)) {
            if ((*j)[key].is_string()) {
                return (*j)[key].get<std::string>();
            } else if ((*j)[key].is_number()) {
                // 数字转字符串
                return std::to_string((*j)[key].get<int64_t>());
            }
        }
        return defaultVal;
    } catch (...) {
        return defaultVal;
    }
}

std::string JsonValue::getString() const {
    if (!m_impl) return "";
    try {
        json* j = static_cast<json*>(m_impl);
        if (j->is_string()) {
            return j->get<std::string>();
        }
        return "";
    } catch (...) {
        return "";
    }
}

int JsonValue::getInt(const std::string& key, int defaultVal) const {
    if (!m_impl) return defaultVal;
    try {
        json* j = static_cast<json*>(m_impl);
        if (j->contains(key) && (*j)[key].is_number_integer()) {
            return (*j)[key].get<int>();
        }
        return defaultVal;
    } catch (...) {
        return defaultVal;
    }
}

int64_t JsonValue::getInt64(const std::string& key, int64_t defaultVal) const {
    if (!m_impl) return defaultVal;
    try {
        json* j = static_cast<json*>(m_impl);
        if (j->contains(key) && (*j)[key].is_number_integer()) {
            return (*j)[key].get<int64_t>();
        }
        return defaultVal;
    } catch (...) {
        return defaultVal;
    }
}

double JsonValue::getDouble(const std::string& key, double defaultVal) const {
    if (!m_impl) return defaultVal;
    try {
        json* j = static_cast<json*>(m_impl);
        if (j->contains(key) && (*j)[key].is_number()) {
            return (*j)[key].get<double>();
        }
        return defaultVal;
    } catch (...) {
        return defaultVal;
    }
}

bool JsonValue::getBool(const std::string& key, bool defaultVal) const {
    if (!m_impl) return defaultVal;
    try {
        json* j = static_cast<json*>(m_impl);
        if (j->contains(key) && (*j)[key].is_boolean()) {
            return (*j)[key].get<bool>();
        }
        return defaultVal;
    } catch (...) {
        return defaultVal;
    }
}

JsonValue JsonValue::getObject(const std::string& key) const {
    if (!m_impl) return JsonValue();
    try {
        json* j = static_cast<json*>(m_impl);
        if (j->contains(key)) {
            if ((*j)[key].is_object()) {
                json* obj = new json((*j)[key]);
                return JsonValue(obj);
            } else if ((*j)[key].is_null()) {
                return JsonValue();  // null
            }
        }
        return JsonValue();  // 不存在或类型不匹配，返回 null
    } catch (...) {
        return JsonValue();
    }
}

std::vector<JsonValue> JsonValue::getArray(const std::string& key) const {
    std::vector<JsonValue> result;
    if (!m_impl) return result;
    try {
        json* j = static_cast<json*>(m_impl);
        if (j->contains(key) && (*j)[key].is_array()) {
            for (const auto& item : (*j)[key]) {
                json* item_copy = new json(item);
                result.push_back(JsonValue(item_copy));
            }
        }
    } catch (...) {
        // 返回空数组
    }
    return result;
}

void JsonValue::setString(const std::string& key, const std::string& value) {
    if (!m_impl) return;
    try {
        (*static_cast<json*>(m_impl))[key] = value;
    } catch (...) {
    }
}

void JsonValue::setInt(const std::string& key, int value) {
    if (!m_impl) return;
    try {
        (*static_cast<json*>(m_impl))[key] = value;
    } catch (...) {
    }
}

void JsonValue::setInt64(const std::string& key, int64_t value) {
    if (!m_impl) return;
    try {
        (*static_cast<json*>(m_impl))[key] = value;
    } catch (...) {
    }
}

void JsonValue::setDouble(const std::string& key, double value) {
    if (!m_impl) return;
    try {
        (*static_cast<json*>(m_impl))[key] = value;
    } catch (...) {
    }
}

void JsonValue::setBool(const std::string& key, bool value) {
    if (!m_impl) return;
    try {
        (*static_cast<json*>(m_impl))[key] = value;
    } catch (...) {
    }
}

void JsonValue::setObject(const std::string& key, const JsonValue& value) {
    if (!m_impl || !value.m_impl) return;
    try {
        (*static_cast<json*>(m_impl))[key] = *static_cast<json*>(value.m_impl);
    } catch (...) {
    }
}

void JsonValue::setArray(const std::string& key, const std::vector<JsonValue>& value) {
    if (!m_impl) return;
    try {
        json arr = json::array();
        for (const auto& item : value) {
            if (item.m_impl) {
                arr.push_back(*static_cast<json*>(item.m_impl));
            }
        }
        (*static_cast<json*>(m_impl))[key] = arr;
    } catch (...) {
    }
}

bool JsonValue::contains(const std::string& key) const {
    if (!m_impl) return false;
    try {
        return static_cast<json*>(m_impl)->contains(key);
    } catch (...) {
        return false;
    }
}

void JsonValue::pushBack(const JsonValue& value) {
    if (!m_impl) return;
    try {
        json* j = static_cast<json*>(m_impl);
        if (!j->is_array()) {
            *j = json::array();
        }
        if (value.m_impl) {
            j->push_back(*static_cast<json*>(value.m_impl));
        }
    } catch (...) {
    }
}

size_t JsonValue::size() const {
    if (!m_impl) return 0;
    try {
        return static_cast<json*>(m_impl)->size();
    } catch (...) {
        return 0;
    }
}

JsonValue JsonValue::at(size_t index) const {
    if (!m_impl) return JsonValue();
    try {
        json* j = static_cast<json*>(m_impl);
        if (j->is_array() && index < j->size()) {
            json* item = new json((*j)[index]);
            return JsonValue(item);
        }
    } catch (...) {
    }
    return JsonValue();
}

JsonValue JsonValue::createObject() {
    json* j = new json(json::object());
    return JsonValue(j);
}

JsonValue JsonValue::createArray() {
    json* j = new json(json::array());
    return JsonValue(j);
}

JsonValue JsonValue::createInt64(int64_t value) {
    json* j = new json(value);
    return JsonValue(j);
}

JsonValue JsonValue::createString(const std::string& value) {
    json* j = new json(value);
    return JsonValue(j);
}

JsonValue JsonValue::fromImpl(void* impl) {
    return JsonValue(impl);
}

// ==============================
// JSON-RPC 2.0 实现
// ==============================

JsonRpcRequest::JsonRpcRequest() : jsonrpc("2.0") {
}

JsonRpcRequest::JsonRpcRequest(const std::string& _method, const JsonValue& _params, const JsonValue& _id)
    : jsonrpc("2.0"), method(_method), params(_params), id(_id) {
}

bool JsonRpcRequest::isNotification() const {
    return id.isNull();
}

std::string JsonRpcRequest::encode() const {
    JsonValue j = JsonValue::createObject();
    j.setString("jsonrpc", jsonrpc);
    j.setString("method", method);
    if (!params.isEmpty()) {
        j.setObject("params", params);
    }
    if (!id.isNull()) {
        // id 可能是 number 或 string，需要特殊处理
        json* j_impl = static_cast<json*>(j._impl());
        json* id_impl = static_cast<json*>(id._impl());
        if (id_impl) {
            (*j_impl)["id"] = *id_impl;
        }
    }
    return j.dump();
}

bool JsonRpcRequest::decode(const std::string& jsonStr, JsonRpcRequest& req) {
    try {
        JsonValue j = JsonValue::parse(jsonStr);
        if (j.isNull()) {
            return false;
        }
        
        if (j.getString("jsonrpc") != "2.0") {
            return false;
        }
        
        if (!j.contains("method")) {
            return false;
        }
        
        // 检查 method 是否为字符串
        if (!j.contains("method")) {
            return false;
        }
        
        // 直接获取 method 字段的值
        json* j_impl = static_cast<json*>(j._impl());
        if (!(*j_impl)["method"].is_string()) {
            return false;
        }
        req.method = (*j_impl)["method"].get<std::string>();
        
        req.jsonrpc = j.getString("jsonrpc");
        
        // params 可能是对象或不存在
        if (j.contains("params")) {
            JsonValue params_val = j.getObject("params");
            if (!params_val.isNull()) {
                req.params = params_val;
            } else {
                req.params = JsonValue::createObject();
            }
        } else {
            req.params = JsonValue::createObject();
        }
        
        // id 可能是 number、string 或 null（通知）
        if (j.contains("id")) {
            json* j_impl = static_cast<json*>(j._impl());
            json* id_copy = new json((*j_impl)["id"]);
            req.id = JsonValue::fromImpl(id_copy);
        } else {
            req.id = JsonValue();  // null
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

JsonRpcResponse::JsonRpcResponse() : jsonrpc("2.0") {
}

JsonRpcResponse::JsonRpcResponse(const JsonValue& _result, const JsonValue& _id)
    : jsonrpc("2.0"), result(_result), id(_id) {
}

std::string JsonRpcResponse::encode() const {
    JsonValue j = JsonValue::createObject();
    j.setString("jsonrpc", jsonrpc);
    j.setObject("result", result);
    // id 可能是 number 或 string
    json* j_impl = static_cast<json*>(j._impl());
    if (!id.isNull()) {
        json* id_impl = static_cast<json*>(id._impl());
        if (id_impl) {
            (*j_impl)["id"] = *id_impl;
        }
    } else {
        (*j_impl)["id"] = nullptr;
    }
    return j.dump();
}

bool JsonRpcResponse::decode(const std::string& jsonStr, JsonRpcResponse& resp) {
    try {
        JsonValue j = JsonValue::parse(jsonStr);
        if (j.isNull()) {
            return false;
        }
        
        if (j.getString("jsonrpc") != "2.0") {
            return false;
        }
        
        if (!j.contains("result")) {
            return false;
        }
        
        resp.jsonrpc = j.getString("jsonrpc");
        resp.result = j.getObject("result");
        
        // id 可能是 number、string 或 null
        if (j.contains("id")) {
            json* j_impl = static_cast<json*>(j._impl());
            json id_val = (*j_impl)["id"];
            resp.id = JsonValue::fromImpl(new json(id_val));
        } else {
            resp.id = JsonValue();  // null
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

JsonRpcErrorResponse::JsonRpcErrorResponse() : jsonrpc("2.0") {
}

JsonRpcErrorResponse::JsonRpcErrorResponse(const JsonRpcError& _error, const JsonValue& _id)
    : jsonrpc("2.0"), error(_error), id(_id) {
}

std::string JsonRpcErrorResponse::encode() const {
    JsonValue j = JsonValue::createObject();
    j.setString("jsonrpc", jsonrpc);
    
    JsonValue err = JsonValue::createObject();
    err.setInt("code", error.code);
    err.setString("message", error.message);
    if (!error.data.isEmpty()) {
        err.setObject("data", error.data);
    }
    j.setObject("error", err);
    
    // id 可能是 number 或 string
    json* j_impl = static_cast<json*>(j._impl());
    if (!id.isNull()) {
        json* id_impl = static_cast<json*>(id._impl());
        if (id_impl) {
            (*j_impl)["id"] = *id_impl;
        }
    } else {
        (*j_impl)["id"] = nullptr;
    }
    
    return j.dump();
}

bool JsonRpcErrorResponse::decode(const std::string& jsonStr, JsonRpcErrorResponse& resp) {
    try {
        JsonValue j = JsonValue::parse(jsonStr);
        if (j.isNull()) {
            return false;
        }
        
        if (j.getString("jsonrpc") != "2.0") {
            return false;
        }
        
        if (!j.contains("error")) {
            return false;
        }
        
        resp.jsonrpc = j.getString("jsonrpc");
        JsonValue err = j.getObject("error");
        resp.error.code = err.getInt("code", 0);
        resp.error.message = err.getString("message", "");
        resp.error.data = err.getObject("data");
        
        // id 可能是 number、string 或 null
        if (j.contains("id")) {
            nlohmann::json* j_impl = static_cast<nlohmann::json*>(j._impl());
            nlohmann::json id_val = (*j_impl)["id"];
            resp.id = JsonValue::fromImpl(new nlohmann::json(id_val));
        } else {
            resp.id = JsonValue();  // null
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// ComeJsonCodec 辅助函数
// ==============================

bool ComeJsonCodec::isJsonRpc2(const std::string& jsonStr) {
    try {
        JsonValue j = JsonValue::parse(jsonStr);
        if (j.isNull()) {
            return false;
        }
        return j.getString("jsonrpc") == "2.0";
    } catch (...) {
        return false;
    }
}

bool ComeJsonCodec::isRequest(const std::string& jsonStr) {
    try {
        JsonValue j = JsonValue::parse(jsonStr);
        if (j.isNull()) {
            return false;
        }
        // 请求必须有 jsonrpc、method 和 id 字段
        return j.getString("jsonrpc") == "2.0" && j.contains("method") && j.contains("id");
    } catch (...) {
        return false;
    }
}

bool ComeJsonCodec::isResponse(const std::string& jsonStr) {
    try {
        JsonValue j = JsonValue::parse(jsonStr);
        if (j.isNull()) {
            return false;
        }
        return j.getString("jsonrpc") == "2.0" && (j.contains("result") || j.contains("error"));
    } catch (...) {
        return false;
    }
}

std::string ComeJsonCodec::extractJsonRpcMethod(const std::string& jsonStr) {
    try {
        JsonValue j = JsonValue::parse(jsonStr);
        if (j.isNull()) {
            return "";
        }
        return j.getString("method", "");
    } catch (...) {
        return "";
    }
}

int64_t ComeJsonCodec::extractJsonRpcId(const std::string& jsonStr) {
    try {
        JsonValue j = JsonValue::parse(jsonStr);
        if (j.isNull()) {
            return 0;
        }
        return j.getInt64("id", 0);
    } catch (...) {
        return 0;
    }
}

bool ComeJsonCodec::isJsonRpcRequest(const std::string& jsonStr) {
    try {
        JsonValue j = JsonValue::parse(jsonStr);
        if (j.isNull()) {
            return false;
        }
        return j.getString("jsonrpc") == "2.0" && j.contains("method") && j.contains("id");
    } catch (...) {
        return false;
    }
}

bool ComeJsonCodec::isJsonRpcResponse(const std::string& jsonStr) {
    return isResponse(jsonStr);
}

// ==============================
// JSON-RPC 2.0 编码：将 come_1.h 中的数据结构编码为 JSON-RPC 2.0 格式
// ==============================

std::string ComeJsonCodec::encodeJsonRpcRequest(const EdgeOnline& msg, const std::string& method, int64_t id) {
    try {
        // 先编码 EdgeOnline 为 JSON 对象
        json device_json;
        to_json(device_json, msg);
        
        // 构建 JSON-RPC 2.0 请求
        json rpc_request = json{
            {"jsonrpc", "2.0"},
            {"method", method},
            {"params", device_json},
            {"id", id}
        };
        
        return rpc_request.dump();
    } catch (...) {
        return std::string();
    }
}

std::string ComeJsonCodec::encodeJsonRpcNotification(const EdgeHeartbeat& msg, const std::string& method) {
    try {
        // 先编码 EdgeHeartbeat 为 JSON 对象
        json heartbeat_json;
        to_json(heartbeat_json, msg);
        
        // 构建 JSON-RPC 2.0 通知（无 id）
        json rpc_notification = json{
            {"jsonrpc", "2.0"},
            {"method", method},
            {"params", heartbeat_json}
        };
        
        return rpc_notification.dump();
    } catch (...) {
        return std::string();
    }
}

// ==============================
// JSON-RPC 2.0 解码：从 JSON-RPC 2.0 响应中解码为 come_1.h 中的数据结构
// ==============================

bool ComeJsonCodec::decodeJsonRpcResponse(const std::string& jsonStr, AckEdgeOnline& msg) {
    try {
        json j = json::parse(jsonStr);
        
        // 检查是否为 JSON-RPC 2.0 响应
        if (!j.contains("jsonrpc") || j["jsonrpc"] != "2.0") {
            return false;
        }
        
        // 检查是否有 result 字段
        if (!j.contains("result") || !j["result"].is_object()) {
            return false;
        }
        
        // 从 result 中解码 AckEdgeOnline
        from_json(j["result"], msg);
        return true;
    } catch (...) {
        return false;
    }
}

bool ComeJsonCodec::decodeJsonRpcError(const std::string& jsonStr, int32_t& code, std::string& message) {
    try {
        json j = json::parse(jsonStr);

        // 检查是否为 JSON-RPC 2.0 错误响应
        if (!j.contains("jsonrpc") || j["jsonrpc"] != "2.0") {
            return false;
        }

        // 检查是否有 error 字段
        if (!j.contains("error") || !j["error"].is_object()) {
            return false;
        }

        json error_obj = j["error"];
        code = get_field_value<int32_t>(error_obj, "code", 0);
        message = get_field_value<std::string>(error_obj, "message", "");

        return true;
    } catch (...) {
        return false;
    }
}

// ==============================
// JSON-RPC 2.0 高级解析接口（业务代码使用）
// ==============================

bool ComeJsonCodec::parseJsonRpcRequest(const std::string& jsonStr, JsonRpcRequest& req) {
    return JsonRpcRequest::decode(jsonStr, req);
}

bool ComeJsonCodec::parseJsonRpcResponse(const std::string& jsonStr, JsonRpcResponse& resp, JsonRpcError& error) {
    try {
        json j = json::parse(jsonStr);

        if (!j.contains("jsonrpc") || j["jsonrpc"] != "2.0") {
            return false;
        }

        if (j.contains("result")) {
            // 成功响应
            JsonValue result_val = JsonValue::fromImpl(new json(j["result"]));
            JsonValue id_val;
            if (j.contains("id")) {
                id_val = JsonValue::fromImpl(new json(j["id"]));
            }
            resp = JsonRpcResponse(result_val, id_val);
            return true;
        } else if (j.contains("error")) {
            // 错误响应
            json error_obj = j["error"];
            error.code = get_field_value<int32_t>(error_obj, "code", 0);
            error.message = get_field_value<std::string>(error_obj, "message", "");
            if (error_obj.contains("data")) {
                error.data = JsonValue::fromImpl(new json(error_obj["data"]));
            }

            JsonValue id_val;
            if (j.contains("id")) {
                id_val = JsonValue::fromImpl(new json(j["id"]));
            }
            resp = JsonRpcResponse(JsonValue::createObject(), id_val);
            return true;
        }

        return false;
    } catch (...) {
        return false;
    }
}

std::string ComeJsonCodec::buildJsonRpcRequest(const std::string& method, const JsonValue& params, int64_t id) {
    JsonRpcRequest req(method, params, JsonValue::createInt64(id));
    return req.encode();
}

std::string ComeJsonCodec::buildJsonRpcNotification(const std::string& method, const JsonValue& params) {
    JsonRpcRequest req(method, params);
    return req.encode();
}

std::string ComeJsonCodec::buildJsonRpcSuccess(const JsonValue& result, const JsonValue& id) {
    JsonRpcResponse resp(result, id);
    return resp.encode();
}

std::string ComeJsonCodec::buildJsonRpcError(int code, const std::string& message, const JsonValue& id) {
    JsonRpcError error(code, message);
    JsonRpcErrorResponse resp(error, id);
    return resp.encode();
}

// ==============================
// JSON-RPC 2.0 直接消息编码接口（业务代码使用）
// ==============================

// 前向声明 to_json 函数（实际定义在后面）
static void to_json(json& j, const AckManagerEdgeList& msg);
static void to_json(json& j, const AckManagerEdgeOfflineList& msg);
static void to_json(json& j, const AckManagerEdgeKick& msg);
static void to_json(json& j, const IngressEdgeOnline& msg);
static void to_json(json& j, const IngressEdgeOffline& msg);
static void to_json(json& j, const ManagerConnect& msg);

std::string ComeJsonCodec::encodeJsonRpcResponse(const AckEdgeOnline& msg, int64_t id) {
    try {
        // 编码消息为 JSON 对象
        json msg_json;
        to_json(msg_json, msg);

        // 构建 JSON-RPC 2.0 成功响应
        json rpc_response = json{
            {"jsonrpc", "2.0"},
            {"result", msg_json},
            {"id", id}
        };

        return rpc_response.dump();
    } catch (...) {
        return std::string();
    }
}

std::string ComeJsonCodec::encodeJsonRpcResponse(const AckManagerEdgeList& msg, int64_t id) {
    try {
        json msg_json;
        to_json(msg_json, msg);

        json rpc_response = json{
            {"jsonrpc", "2.0"},
            {"result", msg_json},
            {"id", id}
        };

        return rpc_response.dump();
    } catch (...) {
        return std::string();
    }
}

std::string ComeJsonCodec::encodeJsonRpcResponse(const AckManagerEdgeOfflineList& msg, int64_t id) {
    try {
        json msg_json;
        to_json(msg_json, msg);

        json rpc_response = json{
            {"jsonrpc", "2.0"},
            {"result", msg_json},
            {"id", id}
        };

        return rpc_response.dump();
    } catch (...) {
        return std::string();
    }
}

std::string ComeJsonCodec::encodeJsonRpcResponse(const AckManagerEdgeKick& msg, int64_t id) {
    try {
        json msg_json;
        to_json(msg_json, msg);

        json rpc_response = json{
            {"jsonrpc", "2.0"},
            {"result", msg_json},
            {"id", id}
        };

        return rpc_response.dump();
    } catch (...) {
        return std::string();
    }
}

std::string ComeJsonCodec::encodeJsonRpcNotification(const IngressEdgeOnline& msg, const std::string& method) {
    try {
        json msg_json;
        to_json(msg_json, msg);

        json rpc_notification = json{
            {"jsonrpc", "2.0"},
            {"method", method},
            {"params", msg_json}
        };

        return rpc_notification.dump();
    } catch (...) {
        return std::string();
    }
}

std::string ComeJsonCodec::encodeJsonRpcNotification(const IngressEdgeOffline& msg, const std::string& method) {
    try {
        json msg_json;
        to_json(msg_json, msg);

        json rpc_notification = json{
            {"jsonrpc", "2.0"},
            {"method", method},
            {"params", msg_json}
        };

        return rpc_notification.dump();
    } catch (...) {
        return std::string();
    }
}

std::string ComeJsonCodec::encodeJsonRpcNotification(const ManagerConnect& msg, const std::string& method) {
    try {
        json msg_json;
        to_json(msg_json, msg);

        json rpc_notification = json{
            {"jsonrpc", "2.0"},
            {"method", method},
            {"params", msg_json}
        };

        return rpc_notification.dump();
    } catch (...) {
        return std::string();
    }
}

std::string ComeJsonCodec::encodeJsonRpcRequest(const IngressEdgeOnline& msg, const std::string& method, int64_t id) {
    try {
        json msg_json;
        to_json(msg_json, msg);

        json rpc_request = json{
            {"jsonrpc", "2.0"},
            {"method", method},
            {"params", msg_json},
            {"id", id}
        };

        return rpc_request.dump();
    } catch (...) {
        return std::string();
    }
}

std::string ComeJsonCodec::encodeJsonRpcRequest(const IngressEdgeOffline& msg, const std::string& method, int64_t id) {
    try {
        json msg_json;
        to_json(msg_json, msg);

        json rpc_request = json{
            {"jsonrpc", "2.0"},
            {"method", method},
            {"params", msg_json},
            {"id", id}
        };

        return rpc_request.dump();
    } catch (...) {
        return std::string();
    }
}

// ==============================
// JSON-RPC 2.0 直接解析接口（业务代码使用）
// ==============================

bool ComeJsonCodec::decodeJsonRpcRequest(const std::string& jsonStr, EdgeOnline& msg, std::string& method, int64_t& id) {
    JsonRpcRequest req;
    if (!JsonRpcRequest::decode(jsonStr, req)) return false;
    method = req.method;
    id = req.id.getInt64("", 0);
    return ComeJsonCodec::decode(req.params.dump(), msg);
}

bool ComeJsonCodec::decodeJsonRpcRequest(const std::string& jsonStr, EdgeHeartbeat& msg, std::string& method, int64_t& id) {
    JsonRpcRequest req;
    if (!JsonRpcRequest::decode(jsonStr, req)) return false;
    method = req.method;
    id = req.id.getInt64("", 0);
    return ComeJsonCodec::decode(req.params.dump(), msg);
}

bool ComeJsonCodec::decodeJsonRpcRequest(const std::string& jsonStr, ManagerConnect& msg, std::string& method, int64_t& id) {
    JsonRpcRequest req;
    if (!JsonRpcRequest::decode(jsonStr, req)) return false;
    method = req.method;
    id = req.id.getInt64("", 0);
    return ComeJsonCodec::decode(req.params.dump(), msg);
}

bool ComeJsonCodec::decodeJsonRpcRequest(const std::string& jsonStr, ManagerEdgeList& msg, std::string& method, int64_t& id) {
    JsonRpcRequest req;
    if (!JsonRpcRequest::decode(jsonStr, req)) return false;
    method = req.method;
    id = req.id.getInt64("", 0);
    return ComeJsonCodec::decode(req.params.dump(), msg);
}

bool ComeJsonCodec::decodeJsonRpcRequest(const std::string& jsonStr, ManagerEdgeOfflineList& msg, std::string& method, int64_t& id) {
    JsonRpcRequest req;
    if (!JsonRpcRequest::decode(jsonStr, req)) return false;
    method = req.method;
    id = req.id.getInt64("", 0);
    return ComeJsonCodec::decode(req.params.dump(), msg);
}

bool ComeJsonCodec::decodeJsonRpcRequest(const std::string& jsonStr, ManagerEdgeKick& msg, std::string& method, int64_t& id) {
    JsonRpcRequest req;
    if (!JsonRpcRequest::decode(jsonStr, req)) return false;
    method = req.method;
    id = req.id.getInt64("", 0);
    return ComeJsonCodec::decode(req.params.dump(), msg);
}

bool ComeJsonCodec::decodeJsonRpcNotification(const std::string& jsonStr, EdgeHeartbeat& msg, std::string& method, int64_t& id) {
    JsonRpcRequest req;
    if (!JsonRpcRequest::decode(jsonStr, req)) return false;
    method = req.method;
    id = req.id.getInt64("", 0);
    return ComeJsonCodec::decode(req.params.dump(), msg);
}

// ==============================
// Manager ↔ Ingress 消息编解码实现
// ==============================

// ManagerConnect
static void to_json(json& j, const ManagerConnect& msg) {
    j = json::object();
    j["manager_id"] = msg.manager_id;
    j["ingress_id"] = msg.ingress_id;
    if (!msg.timestamp.empty()) {
        j["timestamp"] = msg.timestamp;
    }
}

static void from_json(const json& j, ManagerConnect& msg) {
    msg.manager_id = get_field_value<std::string>(j, "manager_id", "");
    msg.ingress_id = get_field_value<std::string>(j, "ingress_id", "");
    msg.timestamp = get_field_value<std::string>(j, "timestamp", "");
}

std::string ComeJsonCodec::encode(const ManagerConnect& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, ManagerConnect& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// IngressEdgeOnline
static void to_json(json& j, const IngressEdgeOnline& msg) {
    j = json::object();
    j["edge_id"] = msg.edge_id;
    if (!msg.edge_type.empty()) j["edge_type"] = msg.edge_type;
    if (!msg.local_ip.empty()) j["local_ip"] = msg.local_ip;
    if (!msg.public_ip.empty()) j["public_ip"] = msg.public_ip;
    j["ingress_id"] = msg.ingress_id;
    j["online_since"] = msg.online_since;
}

static void from_json(const json& j, IngressEdgeOnline& msg) {
    msg.edge_id = get_field_value<std::string>(j, "edge_id", "");
    msg.edge_type = get_field_value<std::string>(j, "edge_type", "");
    msg.local_ip = get_field_value<std::string>(j, "local_ip", "");
    msg.public_ip = get_field_value<std::string>(j, "public_ip", "");
    msg.ingress_id = get_field_value<std::string>(j, "ingress_id", "");
    msg.online_since = get_field_value<int64_t>(j, "online_since", 0);
}

std::string ComeJsonCodec::encode(const IngressEdgeOnline& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, IngressEdgeOnline& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// IngressEdgeOffline
static void to_json(json& j, const IngressEdgeOffline& msg) {
    j = json::object();
    j["edge_id"] = msg.edge_id;
    if (!msg.ingress_id.empty()) j["ingress_id"] = msg.ingress_id;
}

static void from_json(const json& j, IngressEdgeOffline& msg) {
    msg.edge_id = get_field_value<std::string>(j, "edge_id", "");
    msg.ingress_id = get_field_value<std::string>(j, "ingress_id", "");
}

std::string ComeJsonCodec::encode(const IngressEdgeOffline& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, IngressEdgeOffline& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ManagerEdgeList
static void to_json(json& j, const ManagerEdgeList& msg) {
    j = json::object();
    j["offset"] = msg.offset;
    j["limit"] = msg.limit;
}

static void from_json(const json& j, ManagerEdgeList& msg) {
    msg.offset = get_field_value<int32_t>(j, "offset", 0);
    msg.limit = get_field_value<int32_t>(j, "limit", 50);
}

std::string ComeJsonCodec::encode(const ManagerEdgeList& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, ManagerEdgeList& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// AckManagerEdgeList
static void to_json(json& j, const AckManagerEdgeList& msg) {
    j = json::object();
    j["edge_ids"] = msg.edge_ids;
    j["total"] = msg.total;
    j["offset"] = msg.offset;
    j["limit"] = msg.limit;
    j["count"] = msg.count;
}

static void from_json(const json& j, AckManagerEdgeList& msg) {
    msg.edge_ids = get_field_value<std::vector<std::string>>(j, "edge_ids", {});
    msg.total = get_field_value<int32_t>(j, "total", 0);
    msg.offset = get_field_value<int32_t>(j, "offset", 0);
    msg.limit = get_field_value<int32_t>(j, "limit", 50);
    msg.count = get_field_value<int32_t>(j, "count", 0);
}

std::string ComeJsonCodec::encode(const AckManagerEdgeList& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckManagerEdgeList& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ManagerEdgeOfflineList
static void to_json(json& j, const ManagerEdgeOfflineList& msg) {
    j = json::object();
    j["offset"] = msg.offset;
    j["limit"] = msg.limit;
}

static void from_json(const json& j, ManagerEdgeOfflineList& msg) {
    msg.offset = get_field_value<int32_t>(j, "offset", 0);
    msg.limit = get_field_value<int32_t>(j, "limit", 50);
}

std::string ComeJsonCodec::encode(const ManagerEdgeOfflineList& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, ManagerEdgeOfflineList& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// OfflineEdgeInfo
static void to_json(json& j, const OfflineEdgeInfo& msg) {
    j = json::object();
    j["edge_id"] = msg.edge_id;
    j["offline_time"] = msg.offline_time;
    j["pending_confirm"] = msg.pending_confirm;
    if (!msg.edge_type.empty()) j["edge_type"] = msg.edge_type;
}

static void from_json(const json& j, OfflineEdgeInfo& msg) {
    msg.edge_id = get_field_value<std::string>(j, "edge_id", "");
    msg.offline_time = get_field_value<int64_t>(j, "offline_time", 0);
    msg.pending_confirm = get_field_value<bool>(j, "pending_confirm", false);
    msg.edge_type = get_field_value<std::string>(j, "edge_type", "");
}

// AckManagerEdgeOfflineList
static void to_json(json& j, const AckManagerEdgeOfflineList& msg) {
    j = json::object();
    j["offline_devices"] = msg.offline_devices;
    j["total"] = msg.total;
    j["offset"] = msg.offset;
    j["limit"] = msg.limit;
    j["count"] = msg.count;
}

static void from_json(const json& j, AckManagerEdgeOfflineList& msg) {
    msg.offline_devices = get_field_value<std::vector<OfflineEdgeInfo>>(j, "offline_devices", {});
    msg.total = get_field_value<int32_t>(j, "total", 0);
    msg.offset = get_field_value<int32_t>(j, "offset", 0);
    msg.limit = get_field_value<int32_t>(j, "limit", 50);
    msg.count = get_field_value<int32_t>(j, "count", 0);
}

std::string ComeJsonCodec::encode(const AckManagerEdgeOfflineList& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckManagerEdgeOfflineList& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// ManagerEdgeKick
static void to_json(json& j, const ManagerEdgeKick& msg) {
    j = json::object();
    j["edge_id"] = msg.edge_id;
    if (!msg.reason.empty()) j["reason"] = msg.reason;
}

static void from_json(const json& j, ManagerEdgeKick& msg) {
    msg.edge_id = get_field_value<std::string>(j, "edge_id", "");
    msg.reason = get_field_value<std::string>(j, "reason", "");
}

std::string ComeJsonCodec::encode(const ManagerEdgeKick& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, ManagerEdgeKick& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

// AckManagerEdgeKick
static void to_json(json& j, const AckManagerEdgeKick& msg) {
    j = json::object();
    j["edge_id"] = msg.edge_id;
    j["success"] = msg.success;
    if (msg.code != 0) j["code"] = msg.code;
    if (!msg.msg.empty()) j["message"] = msg.msg;
}

static void from_json(const json& j, AckManagerEdgeKick& msg) {
    msg.edge_id = get_field_value<std::string>(j, "edge_id", "");
    msg.success = get_field_value<bool>(j, "success", false);
    msg.code = get_field_value<int32_t>(j, "code", 0);
    msg.msg = get_field_value<std::string>(j, "message", "");
}

std::string ComeJsonCodec::encode(const AckManagerEdgeKick& msg) {
    try {
        json j;
        to_json(j, msg);
        return j.dump();
    } catch (...) {
        return std::string();
    }
}

bool ComeJsonCodec::decode(const std::string& jsonStr, AckManagerEdgeKick& msg) {
    try {
        json j = json::parse(jsonStr);
        from_json(j, msg);
        return true;
    } catch (...) {
        return false;
    }
}

