/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Main.cpp - touch_ingress main program
 *
 * Copyright (C) 2013 - ezlibs.com -, All Rights Reserved.
 *
 * $Id: Main.cpp $
 *
 *  Explain:
 *     touch_ingress: WebSocket server for device registration.
 *
 *  Update:
 *     2013-10-30 20:55:22 Create
 *     2026-03-29 Use formal implementation from src/Function/Touch/Ingress
 *     2026-03-31 Use getopt_long for command line argument parsing
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include "FunRegisterSvr.h"
#include "EZThread.h"
#include "EZTimer.h"

// 默认参数
#define DEFAULT_PORT    54321

// arglist 参数定义（支持长短参数名）
static const char *arg_port = NULL;           // -p, --port
static const char *arg_bind_addr = NULL;      // -b, --bind
static int arg_help = 0;                      // -h, --help

// 位置参数列表（arglist 规范）
static char *arglist[1024];
static int arglist_count = 0;

// 长选项定义
static struct option long_options[] = {
    {"port",      required_argument, 0, 'p'},
    {"bind",      required_argument, 0, 'b'},
    {"help",      no_argument,       0, 'h'},
    {0, 0, 0, 0}
};

static bool g_running = true;

void sigint_handler(int sig)
{
    (void)sig;
    g_running = false;
}

void on_device_register(int client_id, const std::string& device_id, bool success, void *user_data)
{
    (void)user_data;

    // 根据 device_id 区分 manager 和 edge
    if (device_id == "manager") {
        printf("Manager connected: client_id=%d, success=%d\n", client_id, success);
    } else {
        printf("Edge device register: client_id=%d, device_id=%s, success=%d\n",
               client_id, device_id.c_str(), success);
    }
}

void print_usage(const char *prog)
{
    printf("Usage: %s [OPTIONS] [arglist]\n", prog);
    printf("\n");
    printf("WebSocket server for device registration.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -p, --port <port>       Server port (default: %d)\n", DEFAULT_PORT);
    printf("  -b, --bind <address>    Bind address (default: 0.0.0.0)\n");
    printf("  -h, --help              Show this help message\n");
    printf("\n");
    printf("Positional arguments (arglist, in order):\n");
    printf("  <port> <bind_address>\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s -p 8080\n", prog);
    printf("  %s --port 8080 --bind 192.168.1.100\n", prog);
    printf("  %s 8080 192.168.1.100\n", prog);
}

int main(int argc, char *argv[])
{
    int opt;
    int option_index = 0;
    
    // 使用 getopt_long 解析命令行参数
    while ((opt = getopt_long(argc, argv, "p:b:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'p':
                arg_port = optarg;
                break;
            case 'b':
                arg_bind_addr = optarg;
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
    
    // 应用参数（选项参数优先，位置参数次之）
    unsigned short port = arg_port ? (unsigned short)atoi(arg_port) : 
                          (arglist_count > 0 ? (unsigned short)atoi(arglist[0]) : DEFAULT_PORT);
    const char *bind_addr = arg_bind_addr ? arg_bind_addr : 
                            (arglist_count > 1 ? arglist[1] : "0.0.0.0");
    
    // 验证端口
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Invalid port: %d (must be 1-65535)\n", port);
        return 1;
    }

    printf("touch_ingress: Starting server on port %d, bind: %s\n", port, bind_addr);

    // 初始化 ezThread
    g_TimerManager.Start();
    g_ThreadManager.RegisterMainThread(ThreadGetID());

    // 注册信号处理
    signal(SIGINT, sigint_handler);

    // 创建注册功能对象
    CFunRegisterSvr register_svr;
    register_svr.SetRegisterCallback(on_device_register, NULL);

    // 启动
    register_svr.Start(port);

    // 主循环：等待中断
    printf("touch_ingress running, press Ctrl+C to exit...\n");
    while (g_running) {
        sleep(1);
    }

    // 停止
    printf("Shutting down...\n");

    // 检查是否有活跃连接（在 Stop() 之前检查）
    int client_count = g_DevWsRegisterSvr.GetClientCount();
    if (client_count > 0) {
        printf("Closing %d active client connection(s)...\n", client_count);
    }

    // 停止服务
    register_svr.Stop();

    // 等待连接关闭和资源清理
    if (client_count > 0) {
        printf("Waiting for connections to close...\n");
        usleep(100000);  // 等待 100ms 让连接关闭
    }

    printf("touch_ingress exited\n");
    return 0;
}
