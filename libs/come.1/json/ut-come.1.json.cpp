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

// 测试 DeviceOnline 编解码
int test_DeviceOnline() {
    if (g_details) {
        std::cout << "\n=== Test DeviceOnline ===" << std::endl;
    }
    
    // Create message
    DeviceOnline online("device001", "secret_key_123", "sensor", "abc123nonce456");
    
    // Encode
    std::string jsonStr = ComeJsonCodec::encode(online);
    if (g_details) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }
    
    // Decode
    DeviceOnline decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("DeviceOnline", false);
        return -1;
    }
    
    // Verify
    assert(decoded.id == "device001");
    assert(decoded.key == "secret_key_123");
    assert(decoded.type == "sensor");
    assert(decoded.nonce == "abc123nonce456");
    
    if (g_details) {
        std::cout << "Decode success: id=" << decoded.id 
                  << ", type=" << decoded.type 
                  << ", nonce=" << decoded.nonce << std::endl;
    }
    
    print_test_result("DeviceOnline", true);
    return 0;
}

// 测试 AckDeviceOnline 编解码
int test_AckDeviceOnline() {
    if (g_details) {
        std::cout << "\n=== Test AckDeviceOnline ===" << std::endl;
    }
    
    AckDeviceOnline ack(0, true, "Device online success", 
                        "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9", "Bearer", 3600);
    
    std::string jsonStr = ComeJsonCodec::encode(ack);
    if (g_details) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }
    
    AckDeviceOnline decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("AckDeviceOnline", false);
        return -1;
    }
    
    assert(decoded.code == 0);
    assert(decoded.msg == "Device online success");
    assert(decoded.success == true);
    assert(decoded.access_token == "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9");
    assert(decoded.token_type == "Bearer");
    assert(decoded.expires_in == 3600);
    
    if (g_details) {
        std::cout << "Decode success: code=" << decoded.code 
                  << ", msg=" << decoded.msg 
                  << ", success=" << decoded.success
                  << ", access_token=" << decoded.access_token
                  << ", token_type=" << decoded.token_type
                  << ", expires_in=" << decoded.expires_in << std::endl;
    }
    
    print_test_result("AckDeviceOnline", true);
    return 0;
}

// 测试 DeviceReport 编解码
int test_DeviceReport() {
    if (g_details) {
        std::cout << "\n=== Test DeviceReport ===" << std::endl;
    }
    
    DeviceReport report;
    report.id = "device001";
    report.access_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9";
    report.data["temperature"] = "25.5";
    report.data["humidity"] = "60.0";
    report.data["status"] = "normal";
    
    std::string jsonStr = ComeJsonCodec::encode(report);
    if (g_details) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }
    
    DeviceReport decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("DeviceReport", false);
        return -1;
    }
    
    assert(decoded.id == "device001");
    assert(decoded.access_token == "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9");
    assert(decoded.data["temperature"] == "25.5");
    assert(decoded.data["humidity"] == "60.0");
    
    if (g_details) {
        std::cout << "Decode success: id=" << decoded.id 
                  << ", access_token=" << decoded.access_token
                  << ", data size=" << decoded.data.size() << std::endl;
    }
    
    print_test_result("DeviceReport", true);
    return 0;
}

// 测试 ServerCommand 编解码
int test_ServerCommand() {
    if (g_details) {
        std::cout << "\n=== Test ServerCommand ===" << std::endl;
    }
    
    ServerCommand cmd;
    cmd.id = "device001";
    cmd.command_type = "reboot";
    cmd.payload["delay"] = "5";
    cmd.payload["reason"] = "maintenance";
    
    std::string jsonStr = ComeJsonCodec::encode(cmd);
    if (g_details) {
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
    
    if (g_details) {
        std::cout << "Decode success: id=" << decoded.id 
                  << ", command_type=" << decoded.command_type << std::endl;
    }
    
    print_test_result("ServerCommand", true);
    return 0;
}

// 测试 AckDeviceReport 编解码
int test_AckDeviceReport() {
    if (g_details) {
        std::cout << "\n=== Test AckDeviceReport ===" << std::endl;
    }
    
    AckDeviceReport ack(0, true, "device001", "Received successfully", "2024-01-01 12:00:00");
    
    std::string jsonStr = ComeJsonCodec::encode(ack);
    if (g_details) {
        std::cout << "Encoded result:\n" << jsonStr << std::endl;
    }
    
    AckDeviceReport decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("AckDeviceReport", false);
        return -1;
    }
    
    assert(decoded.code == 0);
    assert(decoded.msg == "Received successfully");
    assert(decoded.received == true);
    assert(decoded.id == "device001");
    
    if (g_details) {
        std::cout << "Decode success: code=" << decoded.code 
                  << ", msg=" << decoded.msg 
                  << ", received=" << decoded.received 
                  << ", id=" << decoded.id << std::endl;
    }
    
    print_test_result("AckDeviceReport", true);
    return 0;
}

// 测试 AckServerCommand 编解码
int test_AckServerCommand() {
    if (g_details) {
        std::cout << "\n=== Test AckServerCommand ===" << std::endl;
    }
    
    AckServerCommand ack(0, "device001", "reboot", true, "Command executed successfully", "Reboot completed");
    
    std::string jsonStr = ComeJsonCodec::encode(ack);
    if (g_details) {
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
    
    if (g_details) {
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
    if (g_details) {
        std::cout << "\n=== Test Code Overflow Handling ===" << std::endl;
    }
    
    // 创建一个包含超范围 code 值的 JSON（超过 int32_t 最大值 2147483647）
    // 使用 50 亿（5000000000），远超过 int32_t 的最大值
    std::string jsonStr = R"({"code":5000000000,"msg":"Test message","success":true,"access_token":"test_token","token_type":"Bearer","expires_in":3600})";
    
    if (g_details) {
        std::cout << "JSON with overflow code:\n" << jsonStr << std::endl;
    }
    
    AckDeviceOnline decoded;
    if (!ComeJsonCodec::decode(jsonStr, decoded)) {
        print_test_result("Code overflow handling", false);
        return -1;
    }
    
    // 验证：超范围的 code 应该被设置为默认值 0
    if (decoded.code != 0) {
        if (g_details) {
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
    
    if (g_details) {
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
    if (g_details) {
        std::cout << "\n=== Test Invalid JSON Handling ===" << std::endl;
    }
    
    // 测试无效的 JSON 字符串
    std::string invalidJson = R"({"code":0,"msg":"test"invalid})";  // 无效的 JSON
    
    if (g_details) {
        std::cout << "Invalid JSON string:\n" << invalidJson << std::endl;
    }
    
    AckDeviceOnline decoded;
    bool result = ComeJsonCodec::decode(invalidJson, decoded);
    
    // 验证：decode 应该返回 false，不应该抛出异常
    if (result != false) {
        if (g_details) {
            std::cerr << "Error: Expected decode to return false for invalid JSON" << std::endl;
        }
        print_test_result("Invalid JSON handling", false);
        return -1;
    }
    
    if (g_details) {
        std::cout << "Decode correctly returned false for invalid JSON (no exception thrown)" << std::endl;
    }
    
    print_test_result("Invalid JSON handling", true);
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
    
    ret += test_DeviceOnline();
    ret += test_AckDeviceOnline();
    ret += test_DeviceReport();
    ret += test_ServerCommand();
    ret += test_AckDeviceReport();
    ret += test_AckServerCommand();
    ret += test_CodeOverflow();
    ret += test_InvalidJsonHandling();
    
    if (ret == 0) {
        if (g_details) {
            std::cout << "\nAll tests passed!" << std::endl;
        }
    } else {
        std::cerr << "\nTest failed, error code: " << ret << std::endl;
    }
    
    return ret;
}

