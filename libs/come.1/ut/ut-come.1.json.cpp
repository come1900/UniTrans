// ut-come.1.json.cpp

#include <iostream>
#include <cassert>
#include <string>
#include <iomanip>
#include <getopt.h>
#include <cstdlib>

#include "come.1.json.h"

static bool g_details = false;

// Helper function to print test result with alignment
void print_test_result(const std::string& test_name, bool passed) {
    const int name_width = 50; // Width for test name alignment
    std::cout << "\t" << std::left << std::setw(name_width) 
              << ("" + test_name + " encode/decode:") 
              << (passed ? "PASSED" : "FAILED") << std::endl;
}

// 测试 EdgeOnline 编解码
int test_EdgeOnline() {
    bool showThisDetails = false;  // 设置为 true 可单独显示此测试详情
    if (g_details || showThisDetails) {
        std::cout << "\n=== Test EdgeOnline ===" << std::endl;
    }

    // Create message
    EdgeOnline online("edge001", "secret_key_123", "sensor", "abc123nonce456");

    // Encode
    std::string jsonStr = ComeJsonCodec::encode(online);
    if (g_details || showThisDetails) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }

    // Decode
    EdgeOnline decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("EdgeOnline", false);
        return -1;
    }

    // Verify
    assert(decoded.id == "edge001");
    assert(decoded.key == "secret_key_123");
    assert(decoded.type == "sensor");
    assert(decoded.nonce == "abc123nonce456");

    if (g_details || showThisDetails) {
        std::cout << "Decode success: id=" << decoded.id
                  << ", type=" << decoded.type
                  << ", nonce=" << decoded.nonce << std::endl;
    }

    print_test_result("EdgeOnline", true);
    return 0;
}

// 测试 AckEdgeOnline 编解码
int test_AckEdgeOnline() {
    bool showThisDetails = false;  // 设置为 true 可单独显示此测试详情
    if (g_details || showThisDetails) {
        std::cout << "\n=== Test AckEdgeOnline ===" << std::endl;
    }

    AckEdgeOnline ack(0, true, "Edge online success",
                      "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9", "Bearer", 3600);

    std::string jsonStr = ComeJsonCodec::encode(ack);
    if (g_details || showThisDetails) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }

    AckEdgeOnline decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("AckEdgeOnline", false);
        return -1;
    }

    assert(decoded.code == 0);
    assert(decoded.msg == "Edge online success");
    assert(decoded.success == true);
    assert(decoded.access_token == "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9");
    assert(decoded.token_type == "Bearer");
    assert(decoded.expires_in == 3600);

    if (g_details || showThisDetails) {
        std::cout << "Decode success: code=" << decoded.code
                  << ", msg=" << decoded.msg
                  << ", success=" << decoded.success
                  << ", access_token=" << decoded.access_token
                  << ", token_type=" << decoded.token_type
                  << ", expires_in=" << decoded.expires_in << std::endl;
    }

    print_test_result("AckEdgeOnline", true);
    return 0;
}

// 测试 EdgeReport 编解码
int test_EdgeReport() {
    bool showThisDetails = false;  // 设置为 true 可单独显示此测试详情
    if (g_details || showThisDetails) {
        std::cout << "\n=== Test EdgeReport ===" << std::endl;
    }

    EdgeReport report;
    report.id = "edge001";
    report.access_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9";
    report.data["temperature"] = "25.5";
    report.data["humidity"] = "60.0";
    report.data["status"] = "normal";

    std::string jsonStr = ComeJsonCodec::encode(report);
    if (g_details || showThisDetails) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }

    EdgeReport decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("EdgeReport", false);
        return -1;
    }

    assert(decoded.id == "edge001");
    assert(decoded.access_token == "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9");
    assert(decoded.data["temperature"] == "25.5");
    assert(decoded.data["humidity"] == "60.0");

    if (g_details || showThisDetails) {
        std::cout << "Decode success: id=" << decoded.id
                  << ", access_token=" << decoded.access_token
                  << ", data size=" << decoded.data.size() << std::endl;
    }

    print_test_result("EdgeReport", true);
    return 0;
}

// 测试 ServerCommand 编解码
int test_ServerCommand() {
    bool showThisDetails = false;  // 设置为 true 可单独显示此测试详情
    if (g_details || showThisDetails) {
        std::cout << "\n=== Test ServerCommand ===" << std::endl;
    }

    ServerCommand cmd;
    cmd.id = "device001";
    cmd.command_type = "reboot";
    cmd.payload["delay"] = "5";
    cmd.payload["reason"] = "maintenance";

    std::string jsonStr = ComeJsonCodec::encode(cmd);
    if (g_details || showThisDetails) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }
    
    ServerCommand decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("ServerCommand", false);
        return -1;
    }
    
    assert(decoded.id == "device001");
    assert(decoded.command_type == "reboot");
    assert(decoded.payload["delay"] == "5");

    if (g_details || showThisDetails) {
        std::cout << "Decode success: id=" << decoded.id
                  << ", command_type=" << decoded.command_type << std::endl;
    }

    print_test_result("ServerCommand", true);
    return 0;
}

// 测试 AckEdgeReport 编解码
int test_AckEdgeReport() {
    bool showThisDetails = false;  // 设置为 true 可单独显示此测试详情
    if (g_details || showThisDetails) {
        std::cout << "\n=== Test AckEdgeReport ===" << std::endl;
    }

    AckEdgeReport ack(0, true, "edge001", "Received successfully", "2024-01-01 12:00:00");

    std::string jsonStr = ComeJsonCodec::encode(ack);
    if (g_details || showThisDetails) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }

    AckEdgeReport decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("AckEdgeReport", false);
        return -1;
    }

    assert(decoded.code == 0);
    assert(decoded.msg == "Received successfully");
    assert(decoded.received == true);
    assert(decoded.id == "edge001");

    if (g_details || showThisDetails) {
        std::cout << "Decode success: code=" << decoded.code
                  << ", msg=" << decoded.msg
                  << ", received=" << decoded.received
                  << ", id=" << decoded.id << std::endl;
    }

    print_test_result("AckEdgeReport", true);
    return 0;
}

// 测试 AckServerCommand 编解码
int test_AckServerCommand() {
    bool showThisDetails = false;  // 设置为 true 可单独显示此测试详情
    if (g_details || showThisDetails) {
        std::cout << "\n=== Test AckServerCommand ===" << std::endl;
    }

    AckServerCommand ack(0, "device001", "reboot", true, "Command executed successfully", "Reboot completed");

    std::string jsonStr = ComeJsonCodec::encode(ack);
    if (g_details || showThisDetails) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }
    
    AckServerCommand decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("AckServerCommand", false);
        return -1;
    }
    
    assert(decoded.code == 0);
    assert(decoded.msg == "Command executed successfully");
    assert(decoded.id == "device001");
    assert(decoded.command_type == "reboot");
    assert(decoded.handled == true);

    if (g_details || showThisDetails) {
        std::cout << "Decode success: code=" << decoded.code
                  << ", msg=" << decoded.msg
                  << ", id=" << decoded.id
                  << ", command_type=" << decoded.command_type << std::endl;
    }

    print_test_result("AckServerCommand", true);
    return 0;
}

// 测试 code 字段超范围处理
int test_CodeOverflow() {
    bool showThisDetails = false;  // 设置为 true 可单独显示此测试详情
    if (g_details || showThisDetails) {
        std::cout << "\n=== Test Code Overflow Handling ===" << std::endl;
    }

    // 创建一个包含超范围 code 值的 JSON（超过 int32_t 最大值 2147483647）
    // 使用 50 亿（5000000000），远超过 int32_t 的最大值
    std::string jsonStr = R"({"code":5000000000,"msg":"Test message","success":true,"access_token":"test_token","token_type":"Bearer","expires_in":3600})";

    if (g_details || showThisDetails) {
        std::cout << "JSON with overflow code:\n" << jsonStr << std::endl;
    }

    AckEdgeOnline decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("Code overflow handling", false);
        return -1;
    }

    // 验证：超范围的 code 应该被设置为默认值 0
    if (decoded.code != 0) {
        if (g_details || showThisDetails) {
            std::cerr << "Error: Expected code=0 (default) for overflow value, but got code="
                      << decoded.code << std::endl;
        }
        print_test_result("Code overflow handling", false);
        return -1;
    }

    // 验证其他字段正常
    assert(decoded.msg == "Test message");
    assert(decoded.success == true);
    assert(decoded.access_token == "test_token");
    assert(decoded.token_type == "Bearer");
    assert(decoded.expires_in == 3600);

    if (g_details || showThisDetails) {
        std::cout << "Decode success: code=" << decoded.code
                  << " (default value due to overflow), msg=" << decoded.msg
                  << ", success=" << decoded.success
                  << ", access_token=" << decoded.access_token
                  << ", token_type=" << decoded.token_type
                  << ", expires_in=" << decoded.expires_in << std::endl;
    }

    print_test_result("Code overflow handling", true);
    return 0;
}

// 测试异常处理：无效 JSON 字符串
int test_InvalidJsonHandling() {
    bool showThisDetails = false;  // 设置为 true 可单独显示此测试详情
    if (g_details || showThisDetails) {
        std::cout << "\n=== Test Invalid JSON Handling ===" << std::endl;
    }

    // 测试无效的 JSON 字符串
    std::string invalidJson = R"({"code":0,"msg":"test"invalid})";  // 无效的 JSON

    if (g_details || showThisDetails) {
        std::cout << "Invalid JSON string:\n" << invalidJson << std::endl;
    }

    AckEdgeOnline decoded;
    bool result = ComeJsonCodec::decode(invalidJson, decoded);

    // 验证：decode 应该返回 false，不应该抛出异常
    if (result != false) {
        if (g_details || showThisDetails) {
            std::cerr << "Error: Expected decode to return false for invalid JSON" << std::endl;
        }
        print_test_result("Invalid JSON handling", false);
        return -1;
    }

    if (g_details || showThisDetails) {
        std::cout << "Decode correctly returned false for invalid JSON (no exception thrown)" << std::endl;
    }

    print_test_result("Invalid JSON handling", true);
    return 0;
}

// 测试 ConfigUpdate_tunnelService 编解码
int test_ConfigUpdate_tunnelService() {
    bool showThisDetails = false;  // 设置为 true 可单独显示此测试详情
    if (g_details || showThisDetails) {
        std::cout << "\n=== Test ConfigUpdate_tunnelService ===" << std::endl;
    }

    // 创建端点配置
    CEndpoint endpoint("10.220.42.139", 50400);
    
    // 创建安全配置
    CSecurity security("token", "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc", true);
    
    // 创建本地管理配置
    CLocalManagement localManagement("127.0.0.1", 17400);
    
    // 创建目标服务配置
    CTargetService targetService("127.0.0.1", 55555);
    
    // 创建访问策略
    CAccessPolicy policy1("w-tcp-51422", "tcp", targetService, 51422, "pss service");
    CAccessPolicy policy2("w-tcp-51422", "tcp", targetService, 51422, "tss service");

    // 第一个隧道服务配置
    std::vector<CAccessPolicy> policies1;
    policies1.push_back(policy1);
    CtunnelServiceCfg tunnelService1("1.0", endpoint, security, localManagement);

    // 第二个隧道服务配置
    std::vector<CAccessPolicy> policies2;
    policies2.push_back(policy2);
    CtunnelServiceCfg tunnelService2("2.0", endpoint, security, localManagement);

    // 创建服务配置（每个配置项包含 tunnelService 和 accessPolicies）
    CServiceConfig service1("tunnelService", tunnelService1, policies1);
    CServiceConfig service2("tunnelService2", tunnelService2, policies2);

    // 创建配置内容（数组，包含多个服务配置）
    std::vector<CServiceConfig> services;
    services.push_back(service1);
    services.push_back(service2);

    CConfigContent configContent(services);

    // 创建消息
    ConfigUpdate_tunnelService msg("edge001", configContent, 1, "test_access_token");

    // Encode
    std::string jsonStr = ComeJsonCodec::encode(msg);
    if (g_details || showThisDetails) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }

    // Decode
    ConfigUpdate_tunnelService decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("ConfigUpdate_tunnelService", false);
        return -1;
    }

    // Verify config_content 是数组
    assert(decoded.configContent.services.size() == 2);

    // Verify 第一个服务配置
    assert(decoded.configContent.services[0].serviceName == "tunnelService");
    assert(decoded.configContent.services[0].tunnelService.version == "1.0");
    assert(decoded.configContent.services[0].tunnelService.endpoint.host == "10.220.42.139");
    assert(decoded.configContent.services[0].tunnelService.endpoint.port == 50400);
    assert(decoded.configContent.services[0].tunnelService.security.authMethod == "token");
    assert(decoded.configContent.services[0].tunnelService.security.credential == "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc");
    assert(decoded.configContent.services[0].tunnelService.security.enableTls == true);
    assert(decoded.configContent.services[0].tunnelService.localManagement.bindAddress == "127.0.0.1");
    assert(decoded.configContent.services[0].tunnelService.localManagement.bindPort == 17400);
    assert(decoded.configContent.services[0].accessPolicies.size() == 1);
    assert(decoded.configContent.services[0].accessPolicies[0].policyId == "w-tcp-51422");
    assert(decoded.configContent.services[0].accessPolicies[0].targetService.ip == "127.0.0.1");
    assert(decoded.configContent.services[0].accessPolicies[0].targetService.port == 55555);
    assert(decoded.configContent.services[0].accessPolicies[0].exposedPort == 51422);
    assert(decoded.configContent.services[0].accessPolicies[0].description == "pss service");

    // Verify 第二个服务配置
    assert(decoded.configContent.services[1].serviceName == "tunnelService2");
    assert(decoded.configContent.services[1].tunnelService.version == "2.0");
    assert(decoded.configContent.services[1].accessPolicies.size() == 1);
    assert(decoded.configContent.services[1].accessPolicies[0].policyId == "w-tcp-51422");
    assert(decoded.configContent.services[1].accessPolicies[0].exposedPort == 51422);
    assert(decoded.configContent.services[1].accessPolicies[0].description == "tss service");

    // Verify 基类字段
    assert(decoded.edge_id == "edge001");
    assert(decoded.config_type == "tunnelService");
    assert(decoded.version == 1);
    assert(decoded.access_token == "test_access_token");

    if (g_details || showThisDetails) {
        std::cout << "Decode success:" << std::endl;
        std::cout << "  config_content is array with " << decoded.configContent.services.size() << " services" << std::endl;
        std::cout << "  Service[0].serviceName=" << decoded.configContent.services[0].serviceName << std::endl;
        std::cout << "  Service[0].tunnelService.version=" << decoded.configContent.services[0].tunnelService.version << std::endl;
        std::cout << "  Service[0].endpoint.host=" << decoded.configContent.services[0].tunnelService.endpoint.host << std::endl;
        std::cout << "  Service[0].accessPolicies.size=" << decoded.configContent.services[0].accessPolicies.size() << std::endl;
        std::cout << "  Service[1].serviceName=" << decoded.configContent.services[1].serviceName << std::endl;
        std::cout << "  Service[1].tunnelService.version=" << decoded.configContent.services[1].tunnelService.version << std::endl;
        std::cout << "  Service[1].accessPolicies.size=" << decoded.configContent.services[1].accessPolicies.size() << std::endl;
    }

    print_test_result("ConfigUpdate_tunnelService", true);
    return 0;
}

// 测试 toFrpcConfig/fromFrpcConfig 功能
int test_toFrpcConfig() {
    bool showThisDetails = true;  // 设置为 true 可单独显示此测试详情
    if (g_details || showThisDetails) {
        std::cout << "\n=== Test toFrpcConfig/fromFrpcConfig ===" << std::endl;
    }

    // 创建端点配置
    CEndpoint endpoint("10.220.42.139", 50400);

    // 创建安全配置
    CSecurity security("token", "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc", true);

    // 创建本地管理配置
    CLocalManagement localManagement("127.0.0.1", 17400);

    // 创建目标服务配置
    CTargetService targetService("127.0.0.1", 55555);

    // 创建访问策略
    CAccessPolicy policy1("w-tcp-51422", "tcp", targetService, 51422, "pss service");

    // 第一个隧道服务配置
    std::vector<CAccessPolicy> policies1;
    policies1.push_back(policy1);
    CtunnelServiceCfg tunnelService1("1.0", endpoint, security, localManagement);

    // 创建服务配置
    CServiceConfig service1("tunnelService", tunnelService1, policies1);

    // 创建配置内容（数组，包含多个服务配置）
    std::vector<CServiceConfig> services;
    services.push_back(service1);

    CConfigContent configContent(services);

    // 创建消息
    ConfigUpdate_tunnelService msg("edge001", configContent, 1, "test_access_token");

    std::string jsonStr = ComeJsonCodec::encode(msg);
    if (g_details || showThisDetails) {
        std::cout << "TunnelService Encoded result:\n" << jsonStr << std::endl;
    }

    // 使用 toFrpcConfig 转换为 FrpcConfig 结构
    FrpcConfig frpcCfg;
    if (!ComeJsonCodec::toFrpcConfig(msg, frpcCfg)) {
        print_test_result("toFrpcConfig", false);
        return -1;
    }

    // 使用 encode 将 FrpcConfig 编码为 JSON
    std::string frpcJson = ComeJsonCodec::encode(frpcCfg);
    if (g_details || showThisDetails) {
        std::cout << "Frpc config JSON:\n" << frpcJson << std::endl;
    }

    // Verify frpc 配置
    assert(frpcCfg.serverAddr == "10.220.42.139");
    assert(frpcCfg.serverPort == 50400);
    assert(frpcCfg.authMethod == "token");
    assert(frpcCfg.token == "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc");
    assert(frpcCfg.tlsEnable == true);
    assert(frpcCfg.webServerAddr == "127.0.0.1");
    assert(frpcCfg.webServerPort == 17400);
    assert(frpcCfg.proxies.size() == 1);
    assert(frpcCfg.proxies[0].name == "w-tcp-51422");
    assert(frpcCfg.proxies[0].type == "tcp");
    assert(frpcCfg.proxies[0].localIP == "127.0.0.1");
    assert(frpcCfg.proxies[0].localPort == 55555);
    assert(frpcCfg.proxies[0].remotePort == 51422);

    if (g_details || showThisDetails) {
        std::cout << "toFrpcConfig success:" << std::endl;
        std::cout << "  serverAddr=" << frpcCfg.serverAddr << std::endl;
        std::cout << "  serverPort=" << frpcCfg.serverPort << std::endl;
        std::cout << "  authMethod=" << frpcCfg.authMethod << std::endl;
        std::cout << "  token=" << frpcCfg.token << std::endl;
        std::cout << "  tlsEnable=" << frpcCfg.tlsEnable << std::endl;
        std::cout << "  webServerAddr=" << frpcCfg.webServerAddr << std::endl;
        std::cout << "  webServerPort=" << frpcCfg.webServerPort << std::endl;
        std::cout << "  proxies.size=" << frpcCfg.proxies.size() << std::endl;
        std::cout << "  proxies[0].name=" << frpcCfg.proxies[0].name << std::endl;
        std::cout << "  proxies[0].localIP=" << frpcCfg.proxies[0].localIP << std::endl;
        std::cout << "  proxies[0].remotePort=" << frpcCfg.proxies[0].remotePort << std::endl;
    }

    // 测试 fromFrpcConfig - 将 FrpcConfig 转换回 ConfigUpdate_tunnelService
    ConfigUpdate_tunnelService decoded;
    if (!ComeJsonCodec::fromFrpcConfig(frpcCfg, "edge001", 1, "test_access_token", decoded)) {
        print_test_result("fromFrpcConfig", false);
        return -1;
    }

    // 验证转换回来的数据
    assert(decoded.edge_id == "edge001");
    assert(decoded.config_type == "tunnelService");
    assert(decoded.version == 1);
    assert(decoded.access_token == "test_access_token");
    assert(decoded.configContent.services.size() == 1);

    const CServiceConfig& svc = decoded.configContent.services[0];
    assert(svc.serviceName == "tunnelService");
    assert(svc.tunnelService.endpoint.host == "10.220.42.139");
    assert(svc.tunnelService.endpoint.port == 50400);
    assert(svc.tunnelService.security.authMethod == "token");
    assert(svc.tunnelService.security.credential == "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc");
    assert(svc.tunnelService.security.enableTls == true);
    assert(svc.tunnelService.localManagement.bindAddress == "127.0.0.1");
    assert(svc.tunnelService.localManagement.bindPort == 17400);
    assert(svc.accessPolicies.size() == 1);
    assert(svc.accessPolicies[0].policyId == "w-tcp-51422");
    assert(svc.accessPolicies[0].protocol == "tcp");
    assert(svc.accessPolicies[0].targetService.ip == "127.0.0.1");
    assert(svc.accessPolicies[0].targetService.port == 55555);
    assert(svc.accessPolicies[0].exposedPort == 51422);

    if (g_details || showThisDetails) {
        std::cout << "fromFrpcConfig success:" << std::endl;
        std::cout << "  edge_id=" << decoded.edge_id << std::endl;
        std::cout << "  config_type=" << decoded.config_type << std::endl;
        std::cout << "  version=" << decoded.version << std::endl;
        std::cout << "  services.size=" << decoded.configContent.services.size() << std::endl;
        std::cout << "  tunnelService.endpoint.host=" << svc.tunnelService.endpoint.host << std::endl;
        std::cout << "  tunnelService.security.authMethod=" << svc.tunnelService.security.authMethod << std::endl;
        std::cout << "  accessPolicies[0].policyId=" << svc.accessPolicies[0].policyId << std::endl;
    }

    print_test_result("toFrpcConfig/fromFrpcConfig", true);
    return 0;
}

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -d, --details   Print detailed test information" << std::endl;
    std::cout << "  -h, --help       Show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments using getopt_long
    static struct option long_options[] = {
        {"details", no_argument, 0, 'd'},
        {"help",    no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    const char* optstring = "dh";

    while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1) {
        switch (opt) {
            case 'd':
                g_details = true;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case '?':
                // Unknown option
                print_usage(argv[0]);
                return 1;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Check for any remaining non-option arguments
    if (optind < argc) {
        std::cerr << "Unknown argument: " << argv[optind] << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    if (g_details) {
        std::cout << "Start testing come.1.json encode/decode functionality..." << std::endl;
    }

    int ret = 0;

    ret += test_EdgeOnline();
    ret += test_AckEdgeOnline();
    ret += test_EdgeReport();
    ret += test_ServerCommand();
    ret += test_AckEdgeReport();
    ret += test_AckServerCommand();
    ret += test_CodeOverflow();
    ret += test_InvalidJsonHandling();
    ret += test_ConfigUpdate_tunnelService();
    ret += test_toFrpcConfig();

    if (ret == 0) {
        if (g_details) {
            std::cout << "\nAll tests passed!" << std::endl;
        }
    } else {
        std::cerr << "\nTest failed, error code: " << ret << std::endl;
    }

    return ret;
}
