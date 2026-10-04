/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * main.c - 导出 libezsocket 内置 CA 公钥证书到 PEM 文件
 *
 * Copyright (C) 2026 ezlibs.com, All Rights Reserved.
 *
 *  Explain:
 *     Touch manager 为 Python WS 客户端，无法直接调用 C 库的
 *     ez_ws_export_builtin_ca_pem()。本工具以 C 程序链接 libezsocket +
 *     OpenSSL，把内置 CA 公钥证书（PEM）导出到文件（如 ca.crt），
 *     供 manager 以 INGRESS_CA_FILE 信任 ingress 的 wss 证书。
 *     内置 CA 私钥永不导出。
 *
 *     Usage: touch_ca_export [输出文件]   (默认 ./ca.crt)
 *
 *  Update:
 *     2026-09-30  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ez_websocket.h"

/* 内置 CA 证书为固定 PEM，容量预留充足 */
#define CA_BUF_SIZE (16 * 1024)

int main(int argc, char *argv[])
{
    const char *out_path = (argc > 1) ? argv[1] : "ca.crt";
    char buf[CA_BUF_SIZE];

    int n = ez_ws_export_builtin_ca_pem(buf, sizeof(buf));
    if (n < 0)
    {
        fprintf(stderr, "ez_ws_export_builtin_ca_pem: 导出失败\n");
        return 1;
    }

    FILE *fp = fopen(out_path, "wb");
    if (!fp)
    {
        fprintf(stderr, "无法打开输出文件: %s\n", out_path);
        return 1;
    }

    /* n 含末尾 '\0'，只写 PEM 正文 */
    size_t written = fwrite(buf, 1, (size_t)n - 1, fp);
    fclose(fp);

    if (written != (size_t)(n - 1))
    {
        fprintf(stderr, "写入文件不完整\n");
        return 1;
    }

    printf("内置 CA 公钥证书已导出: %s (%zu 字节)\n", out_path, written);
    printf("将 manager 环境变量指向该文件:\n");
    printf("  INGRESS_CA_FILE=%s\n", out_path);
    return 0;
}
