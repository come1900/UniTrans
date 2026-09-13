/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Main.cpp - touch_edge main program (v0.1 MVP)
 *
 * Copyright (C) 2025 - ezlibs.com -, All Rights Reserved.
 *
 * $Id: Main.cpp $
 *
 *  Explain:
 *     touch_edge: Connect to default touch_ingress via WebSocket and register device.
 *
 *  Update:
 *     2025-02-24 Update for v0.1 MVP
 *     2026-03-29 Use formal implementation from src/Function/Touch/Edge
 *     2026-03-31 Use getopt_long for command line argument parsing
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <string>
#include <ezutil/ez_def_devel_debug.h>  // 定义 ARG_USED 宏
#include "FunRegisterCli.h"
#include "EZThread.h"
#include "EZTimer.h"

#include <ezutil/ez_log.h>

// 默认参数宏定义
#define DEFAULT_INGRESS_HOST    "127.0.0.1"
// #define DEFAULT_INGRESS_HOST    "47.100.49.48"
#define DEFAULT_INGRESS_PORT    54321
#define DEFAULT_EDGE_ID         "edge001"
#define DEFAULT_EDGE_KEY        "key001"
#define DEFAULT_EDGE_TYPE       "touch"
#define DEFAULT_LOCAL_IP        ""

// arglist 参数定义（支持长短参数名）
static const char *arg_host = NULL;           // -H, --host
static const char *arg_port = NULL;           // -P, --port
static const char *arg_edge_id = NULL;        // -i, --edge-id
static const char *arg_edge_key = NULL;       // -k, --edge-key
static const char *arg_edge_type = NULL;      // -t, --edge-type
static const char *arg_local_ip = NULL;       // -L, --local-ip
static int arg_help = 0;                      // -h, --help

// 位置参数列表（arglist 规范）
static char *arglist[1024];
static int arglist_count = 0;

// 长选项定义
static struct option long_options[] = {
    {"host",        required_argument, 0, 'H'},
    {"port",        required_argument, 0, 'P'},
    {"edge-id",     required_argument, 0, 'i'},
    {"edge-key",    required_argument, 0, 'k'},
    {"edge-type",   required_argument, 0, 't'},
    {"local-ip",    required_argument, 0, 'L'},
    {"help",        no_argument,       0, 'h'},
    {0, 0, 0, 0}
};

static bool g_running = true;

void sigint_handler(int sig)
{
    (void)sig;
    g_running = false;
}

void on_register_result(bool success, const std::string& msg, void *user_data)
{
    (void)user_data;
    printf("Ingress register result: %s, msg: %s\n",
           success ? "SUCCESS" : "FAILED", msg.c_str());
}

void print_usage(const char *prog)
{
    printf("Usage: %s [OPTIONS] [arglist]\n", prog);
    printf("\n");
    printf("Connect to touch_ingress via WebSocket and register edge.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -H, --host <host>        Ingress host address (default: %s)\n", DEFAULT_INGRESS_HOST);
    printf("  -P, --port <port>        Ingress port (default: %d)\n", DEFAULT_INGRESS_PORT);
    printf("  -i, --edge-id <id>       Edge ID (default: %s)\n", DEFAULT_EDGE_ID);
    printf("  -k, --edge-key <key>     Edge key (default: %s)\n", DEFAULT_EDGE_KEY);
    printf("  -t, --edge-type <type>   Edge type (default: %s)\n", DEFAULT_EDGE_TYPE);
    printf("  -L, --local-ip <ip>      Local IP address (default: %s)\n", DEFAULT_LOCAL_IP);
    printf("  -h, --help               Show this help message\n");
    printf("\n");
    printf("Positional arguments (arglist, in order):\n");
    printf("  <host> <port> <edge_id> <edge_key> <edge_type> <local_ip>\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s -H 192.168.1.100 -P 54321 -i myedge -k mykey\n", prog);
    printf("  %s --host 192.168.1.100 --port 54321 --edge-id myedge --edge-key mykey\n", prog);
    printf("  %s 192.168.1.100 54321 myedge mykey touch 192.168.1.50\n", prog);
    printf("  %s --host 127.0.0.1 --port 54321 --edge-id myedge --edge-key mykey\n", prog);
}

int main(int argc, char *argv[])
{
    int opt;
    int option_index = 0;
    
    // 使用 getopt_long 解析命令行参数
    while ((opt = getopt_long(argc, argv, "H:P:i:k:t:L:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'H':
                arg_host = optarg;
                break;
            case 'P':
                arg_port = optarg;
                break;
            case 'i':
                arg_edge_id = optarg;
                break;
            case 'k':
                arg_edge_key = optarg;
                break;
            case 't':
                arg_edge_type = optarg;
                break;
            case 'L':
                arg_local_ip = optarg;
                break;
            case 'h':
                arg_help = 1;
                break;
            default:
                arg_help = 1;
                break;
        }
    }
    
    // 收集位置参数到 arglist
    while (optind < argc && arglist_count < 1023) {
        arglist[arglist_count++] = argv[optind++];
    }

    // 显示帮助
    if (arg_help) {
        print_usage(argv[0]);
        return 0;
    }

    const char *fname = "_edge.log";
    const int iLevel = EZ_LOG_TO_FILE_AND_STDOUT*10+EZ_LOG_LEVEL_DEBUG;
    ez_log_structure(fname, 3, 3*1024*1024);
    ez_log_set_level(iLevel);

    // 应用参数（选项参数优先，位置参数次之）
    const char *ingress_host = arg_host ? arg_host :
                               (arglist_count > 0 ? arglist[0] : DEFAULT_INGRESS_HOST);
    unsigned short ingress_port = arg_port ? (unsigned short)atoi(arg_port) :
                                  (arglist_count > 1 ? (unsigned short)atoi(arglist[1]) : DEFAULT_INGRESS_PORT);
    const char *edge_id = arg_edge_id ? arg_edge_id :
                            (arglist_count > 2 ? arglist[2] : DEFAULT_EDGE_ID);
    const char *edge_key = arg_edge_key ? arg_edge_key :
                            (arglist_count > 3 ? arglist[3] : DEFAULT_EDGE_KEY);
    const char *edge_type = arg_edge_type ? arg_edge_type :
                              (arglist_count > 4 ? arglist[4] : DEFAULT_EDGE_TYPE);
    const char *local_ip = arg_local_ip ? arg_local_ip :
                           (arglist_count > 5 ? arglist[5] : DEFAULT_LOCAL_IP);
    ARG_USED(local_ip);

    printf("touch_edge: Connecting to ingress %s:%d\n", ingress_host, ingress_port);
    printf("Edge ID: %s, Type: %s\n", edge_id, edge_type);

    // 初始化 ezThread
    g_TimerManager.Start();
    g_ThreadManager.RegisterMainThread(ThreadGetID());

    // 注册信号处理
    signal(SIGINT, sigint_handler);

    // 创建注册客户端对象，直接连接默认/指定的 ingress
    CFunRegisterCli register_cli(edge_id, edge_key, edge_type);
    register_cli.SetRegisterCallback(on_register_result, NULL);

    // 启动（连接到 ingress 并注册）
    // reconnect_max_retries=-1：无限重连，与正式 touchEdge-linux 行为一致
    register_cli.Start(ingress_host, ingress_port, -1);

    // 主循环：仅等待退出信号，重连由 ezsocket 内部处理
    printf("touch_edge running, press Ctrl+C to exit...\n");
    while (g_running) {
        sleep(1);
    }

    // 停止
    printf("Shutting down...\n");
    register_cli.Stop();

    printf("touch_edge exited\n");
    return 0;
}
