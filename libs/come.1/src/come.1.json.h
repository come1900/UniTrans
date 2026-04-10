// come.1.json.h

#ifndef COME_1_JSON_H
#define COME_1_JSON_H

#include "come_1.h"
#include <string>
#include <vector>
#include <map>

// 前向声明，避免暴露 nlohmann::json 到外部
// 注意：这里只做简单的前向声明，实际类型在 cpp 文件中使用
// 不在这里声明，避免与实际的 nlohmann::json 类型冲突

// JSON 值类型（封装 nlohmann::json，避免外部直接依赖）
class JsonValue {
public:
    JsonValue();
    JsonValue(const JsonValue& other);
    JsonValue(JsonValue&& other) noexcept;
    ~JsonValue();
    JsonValue& operator=(const JsonValue& other);
    JsonValue& operator=(JsonValue&& other) noexcept;

    // 从 JSON 字符串解析
    static JsonValue parse(const std::string& jsonStr);
    
    // 转换为 JSON 字符串
    std::string dump(int indent = -1) const;
    
    // 判断是否为空或无效
    bool isNull() const;
    bool isEmpty() const;
    
    // 类型判断
    bool isObject() const;
    bool isArray() const;
    bool isString() const;
    bool isNumber() const;
    bool isBoolean() const;
    
    // 获取值（带默认值）
    std::string getString(const std::string& key, const std::string& defaultVal = "") const;
    std::string getString() const;  // 直接获取字符串值（用于根级别）
    int getInt(const std::string& key, int defaultVal = 0) const;
    int64_t getInt64(const std::string& key, int64_t defaultVal = 0) const;
    double getDouble(const std::string& key, double defaultVal = 0.0) const;
    bool getBool(const std::string& key, bool defaultVal = false) const;
    JsonValue getObject(const std::string& key) const;
    std::vector<JsonValue> getArray(const std::string& key) const;
    
    // 设置值
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setInt64(const std::string& key, int64_t value);
    void setDouble(const std::string& key, double value);
    void setBool(const std::string& key, bool value);
    void setObject(const std::string& key, const JsonValue& value);
    void setArray(const std::string& key, const std::vector<JsonValue>& value);
    
    // 检查键是否存在
    bool contains(const std::string& key) const;
    
    // 数组操作
    void pushBack(const JsonValue& value);
    size_t size() const;
    JsonValue at(size_t index) const;
    
    // 创建对象和数组
    static JsonValue createObject();
    static JsonValue createArray();
    static JsonValue createInt64(int64_t value);
    static JsonValue createString(const std::string& value);
    
    // 内部实现（不对外暴露）
    void* _impl() const { return m_impl; }
    static JsonValue fromImpl(void* impl);

private:
    void* m_impl;  // 指向 nlohmann::json 的指针
    JsonValue(void* impl);
};

// JSON-RPC 2.0 错误对象
struct JsonRpcError {
    int code;
    std::string message;
    JsonValue data;  // 可选，额外的错误信息

    JsonRpcError() : code(0) {}
    JsonRpcError(int _code, const std::string& _message, const JsonValue& _data = JsonValue::createObject())
        : code(_code), message(_message), data(_data) {}
};

// JSON-RPC 2.0 请求
class JsonRpcRequest {
public:
    std::string jsonrpc;  // 固定为 "2.0"
    std::string method;
    JsonValue params;
    JsonValue id;  // 可以是 number 或 string，null 表示通知

    JsonRpcRequest();
    JsonRpcRequest(const std::string& _method, const JsonValue& _params = JsonValue::createObject(), const JsonValue& _id = JsonValue());
    
    // 判断是否为通知（无 id 或 id 为 null）
    bool isNotification() const;
    
    // 编码为 JSON 字符串
    std::string encode() const;
    
    // 从 JSON 字符串解码
    static bool decode(const std::string& jsonStr, JsonRpcRequest& req);
};

// JSON-RPC 2.0 响应（成功）
class JsonRpcResponse {
public:
    std::string jsonrpc;  // 固定为 "2.0"
    JsonValue result;
    JsonValue id;

    JsonRpcResponse();
    JsonRpcResponse(const JsonValue& _result, const JsonValue& _id);
    
    // 编码为 JSON 字符串
    std::string encode() const;
    
    // 从 JSON 字符串解码
    static bool decode(const std::string& jsonStr, JsonRpcResponse& resp);
};

// JSON-RPC 2.0 错误响应
class JsonRpcErrorResponse {
public:
    std::string jsonrpc;  // 固定为 "2.0"
    JsonRpcError error;
    JsonValue id;

    JsonRpcErrorResponse();
    JsonRpcErrorResponse(const JsonRpcError& _error, const JsonValue& _id);
    
    // 编码为 JSON 字符串
    std::string encode() const;
    
    // 从 JSON 字符串解码
    static bool decode(const std::string& jsonStr, JsonRpcErrorResponse& resp);
};

// JSON 编解码器类（原有接口保持不变）
class ComeJsonCodec {
public:
    // 编码：将消息对象转换为 JSON 字符串
    static std::string encode(const EdgeOnline& msg);
    static std::string encode(const AckEdgeOnline& msg);
    static std::string encode(const EdgeReport& msg);
    static std::string encode(const AckEdgeReport& msg);
    static std::string encode(const ServerCommand& msg);
    static std::string encode(const AckServerCommand& msg);
    static std::string encode(const EdgeHeartbeat& msg);
    static std::string encode(const AckEdgeHeartbeat& msg);
    static std::string encode(const ConfigUpdate& msg);
    static std::string encode(const AckConfigUpdate& msg);
    static std::string encode(const IngressLoadReport& msg);
    static std::string encode(const AckIngressLoadReport& msg);
    static std::string encode(const ManagerCommand& msg);
    static std::string encode(const AckManagerCommand& msg);
    static std::string encode(const EdgeRegisterRequest& msg);
    static std::string encode(const AckEdgeRegister& msg);

    // Manager ↔ Ingress 消息编码
    static std::string encode(const ManagerConnect& msg);
    static std::string encode(const IngressEdgeOnline& msg);
    static std::string encode(const IngressEdgeOffline& msg);
    static std::string encode(const ManagerEdgeList& msg);
    static std::string encode(const AckManagerEdgeList& msg);
    static std::string encode(const ManagerEdgeOfflineList& msg);
    static std::string encode(const AckManagerEdgeOfflineList& msg);
    static std::string encode(const ManagerEdgeKick& msg);
    static std::string encode(const AckManagerEdgeKick& msg);

    // 解码：将 JSON 字符串转换为消息对象
    static bool decode(const std::string& jsonStr, EdgeOnline& msg);
    static bool decode(const std::string& jsonStr, AckEdgeOnline& msg);
    static bool decode(const std::string& jsonStr, EdgeReport& msg);
    static bool decode(const std::string& jsonStr, AckEdgeReport& msg);
    static bool decode(const std::string& jsonStr, ServerCommand& msg);
    static bool decode(const std::string& jsonStr, AckServerCommand& msg);
    static bool decode(const std::string& jsonStr, EdgeHeartbeat& msg);
    static bool decode(const std::string& jsonStr, AckEdgeHeartbeat& msg);
    static bool decode(const std::string& jsonStr, ConfigUpdate& msg);
    static bool decode(const std::string& jsonStr, AckConfigUpdate& msg);
    static bool decode(const std::string& jsonStr, IngressLoadReport& msg);
    static bool decode(const std::string& jsonStr, AckIngressLoadReport& msg);
    static bool decode(const std::string& jsonStr, ManagerCommand& msg);
    static bool decode(const std::string& jsonStr, AckManagerCommand& msg);
    static bool decode(const std::string& jsonStr, EdgeRegisterRequest& msg);
    static bool decode(const std::string& jsonStr, AckEdgeRegister& msg);

    // Manager ↔ Ingress 消息解码
    static bool decode(const std::string& jsonStr, ManagerConnect& msg);
    static bool decode(const std::string& jsonStr, IngressEdgeOnline& msg);
    static bool decode(const std::string& jsonStr, IngressEdgeOffline& msg);
    static bool decode(const std::string& jsonStr, ManagerEdgeList& msg);
    static bool decode(const std::string& jsonStr, AckManagerEdgeList& msg);
    static bool decode(const std::string& jsonStr, ManagerEdgeOfflineList& msg);
    static bool decode(const std::string& jsonStr, AckManagerEdgeOfflineList& msg);
    static bool decode(const std::string& jsonStr, ManagerEdgeKick& msg);
    static bool decode(const std::string& jsonStr, AckManagerEdgeKick& msg);

    // 通用 JSON 操作（新增）
    // 判断是否为 JSON-RPC 2.0 格式
    static bool isJsonRpc2(const std::string& jsonStr);
    
    // 判断是否为请求（包含 method）
    static bool isRequest(const std::string& jsonStr);
    
    // 判断是否为响应（包含 result 或 error）
    static bool isResponse(const std::string& jsonStr);

    // 仅提取 method 字段（不解析 params）
    static std::string extractJsonRpcMethod(const std::string& jsonStr);

    // 仅提取 id 字段（不解析 params）
    static int64_t extractJsonRpcId(const std::string& jsonStr);

    // 判断是否为 JSON-RPC 2.0 请求（有 method 和 id 字段）
    static bool isJsonRpcRequest(const std::string& jsonStr);

    // 判断是否为 JSON-RPC 2.0 响应（有 result 或 error 字段）
    static bool isJsonRpcResponse(const std::string& jsonStr);

    // JSON-RPC 2.0 编码：将 come_1.h 中的数据结构编码为 JSON-RPC 2.0 请求
    // 将 EdgeOnline 编码为 JSON-RPC 2.0 请求
    static std::string encodeJsonRpcRequest(const EdgeOnline& msg, const std::string& method, int64_t id);
    
    // 将 EdgeHeartbeat 编码为 JSON-RPC 2.0 通知（无 id）
    static std::string encodeJsonRpcNotification(const EdgeHeartbeat& msg, const std::string& method);
    
    // JSON-RPC 2.0 解码：从 JSON-RPC 2.0 响应中解码为 come_1.h 中的数据结构
    // 从 JSON-RPC 2.0 响应中解码 AckEdgeOnline
    static bool decodeJsonRpcResponse(const std::string& jsonStr, AckEdgeOnline& msg);
    
    // 从 JSON-RPC 2.0 错误响应中解码错误信息
    static bool decodeJsonRpcError(const std::string& jsonStr, int32_t& code, std::string& message);

    // ==============================
    // JSON-RPC 2.0 高级解析接口（业务代码使用）
    // ==============================

    // 解析 JSON-RPC 2.0 请求（自动判断是请求还是通知）
    static bool parseJsonRpcRequest(const std::string& jsonStr, JsonRpcRequest& req);

    // 解析 JSON-RPC 2.0 响应（成功或错误）
    static bool parseJsonRpcResponse(const std::string& jsonStr, JsonRpcResponse& resp, JsonRpcError& error);

    // 构建 JSON-RPC 2.0 请求
    static std::string buildJsonRpcRequest(const std::string& method, const JsonValue& params, int64_t id);

    // 构建 JSON-RPC 2.0 通知（无 id）
    static std::string buildJsonRpcNotification(const std::string& method, const JsonValue& params);

    // 构建 JSON-RPC 2.0 成功响应
    static std::string buildJsonRpcSuccess(const JsonValue& result, const JsonValue& id);

    // 构建 JSON-RPC 2.0 错误响应
    static std::string buildJsonRpcError(int code, const std::string& message, const JsonValue& id);

    // ==============================
    // JSON-RPC 2.0 直接消息编码接口（业务代码使用）
    // ==============================

    // 直接将消息对象编码为 JSON-RPC 2.0 成功响应
    static std::string encodeJsonRpcResponse(const AckEdgeOnline& msg, int64_t id);
    static std::string encodeJsonRpcResponse(const AckManagerEdgeList& msg, int64_t id);
    static std::string encodeJsonRpcResponse(const AckManagerEdgeOfflineList& msg, int64_t id);
    static std::string encodeJsonRpcResponse(const AckManagerEdgeKick& msg, int64_t id);

    // 直接将消息对象编码为 JSON-RPC 2.0 通知（无 id）
    static std::string encodeJsonRpcNotification(const IngressEdgeOnline& msg, const std::string& method);
    static std::string encodeJsonRpcNotification(const IngressEdgeOffline& msg, const std::string& method);
    static std::string encodeJsonRpcNotification(const ManagerConnect& msg, const std::string& method);

    // 直接将消息对象编码为 JSON-RPC 2.0 请求（有 id）
    static std::string encodeJsonRpcRequest(const IngressEdgeOnline& msg, const std::string& method, int64_t id);
    static std::string encodeJsonRpcRequest(const IngressEdgeOffline& msg, const std::string& method, int64_t id);

    // ==============================
    // JSON-RPC 2.0 直接解析接口（业务代码使用）
    // ==============================

    // 直接从 JSON 字符串解析为消息对象 + method + id
    static bool decodeJsonRpcRequest(const std::string& jsonStr, EdgeOnline& msg, std::string& method, int64_t& id);
    static bool decodeJsonRpcRequest(const std::string& jsonStr, EdgeHeartbeat& msg, std::string& method, int64_t& id);
    static bool decodeJsonRpcRequest(const std::string& jsonStr, ManagerConnect& msg, std::string& method, int64_t& id);
    static bool decodeJsonRpcRequest(const std::string& jsonStr, ManagerEdgeList& msg, std::string& method, int64_t& id);
    static bool decodeJsonRpcRequest(const std::string& jsonStr, ManagerEdgeOfflineList& msg, std::string& method, int64_t& id);
    static bool decodeJsonRpcRequest(const std::string& jsonStr, ManagerEdgeKick& msg, std::string& method, int64_t& id);

    // 直接从 JSON 字符串解析为消息对象 + method（通知格式，无 id）
    static bool decodeJsonRpcNotification(const std::string& jsonStr, EdgeHeartbeat& msg, std::string& method, int64_t& id);
};

#endif // COME_1_JSON_H

