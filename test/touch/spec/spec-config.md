# Touch System - 配置参数文档

## 概述

本文档描述 Touch 系统三个网元的所有配置参数。

---

## touchEdge 配置

**配置文件**: `build/local/cfg-touchEdge.cfg`

### General 部分

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| 13_levelLog | int | `37` | 日志级别和输出方式（详见下方说明） |
| 14_LogFilePath | string | `/tmp/logs` | 日志文件路径 |
| 15_LogFileName | string | `log-touchEdge.log` | 日志文件名 |
| 17_Pid | string | `touchEdge.pid` | PID 文件名 |

**13_levelLog 配置说明**:

该参数是一个组合值，格式为：`<输出方式><日志等级>`（十位数 + 个位数）

**输出方式（十位数）**:
| 值 | 宏定义 | 说明 |
|----|--------|------|
| 1 | `EZ_LOG_TO_STDOUT` | 输出到标准输出 |
| 2 | `EZ_LOG_TO_FILE` | 输出到文件 |
| 3 | `EZ_LOG_TO_FILE_AND_STDOUT` | 同时输出到标准输出和文件 |

**日志等级（个位数）**:
| 值 | 宏定义 | 说明 |
|----|--------|------|
| 0 | `EZ_LOG_LEVEL_EMERG` | 系统不可用 |
| 1 | `EZ_LOG_LEVEL_ALERT` | 必须马上采取行动的事件 |
| 2 | `EZ_LOG_LEVEL_CRIT` | 关键的事件 |
| 3 | `EZ_LOG_LEVEL_ERR` | 错误事件 |
| 4 | `EZ_LOG_LEVEL_WARNING` | 警告事件 |
| 5 | `EZ_LOG_LEVEL_NOTICE` | 普通但重要的事件 |
| 6 | `EZ_LOG_LEVEL_INFO` | 有用的信息 |
| 7 | `EZ_LOG_LEVEL_DEBUG` | 调试信息 |
| 8 | `EZ_LOG_LEVEL_NONE` | 不记日志 |

**常用配置示例**:
- `37` = 输出到文件 + 标准输出，调试等级（推荐测试时使用）
- `26` = 仅输出到文件，信息等级
- `13` = 仅输出到标准输出，错误等级
- `27` = 仅输出到文件，调试等级

**配置示例**:
```json
{
   "General" : [
      {
         "13_levelLog" : 37,
         "14_LogFilePath" : "/tmp/logs",
         "15_LogFileName" : "log-touchEdge.log",
         "17_Pid" : "touchEdge.pid"
      }
   ]
}
```

### TouchEdge 部分

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| EdgeId | string | `G1` + HUID | 边缘唯一标识（根据设备硬件唯一标识自动生成） |
| EdgeKey | string | `key001` | 边缘认证密钥 |
| EdgeType | string | `touch` | 边缘类型 |
| IngressHost | string | `127.0.0.1` | Ingress 主机地址（Edge 连接的目标 Ingress 服务器 IP） |
| IngressPort | int | `54321` | Ingress 端口 |
| ConnectTimeout | int | `10` | 连接超时（秒） |
| HeartbeatInterval | int | `30` | 心跳间隔（秒） |
| ReconnectInterval | int | `30` | 重连间隔（秒） |
| ReconnectMaxRetries | int | `-1` | 最大重连次数（-1 表示无限重试） |

**IngressHost 配置说明**:
- 用途：指定 Edge 要连接的 Ingress 服务器地址
- 测试环境：设置为 `127.0.0.1`（本机）
- 生产环境：设置为实际 Ingress 服务器的 IP 地址或域名
- 修改后：需要重启 touchEdge 才能生效
- 示例：
  - 本机测试：`"IngressHost" : "127.0.0.1"`
  - 远程 Ingress：`"IngressHost" : "192.168.1.100"`

**EdgeId 生成规则**:
- 如果配置文件中 `EdgeId` 长度不足，程序会自动生成：`"G1" + g_Solar.GetHuid()`
- 生成后会保存回配置文件，下次启动时直接使用
- 示例：`G1509057776`

---

## touchIngress 配置

**配置文件**: `build/local/cfg-touchIngress.cfg`

### General 部分

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| 13_levelLog | int | `37` | 日志级别和输出方式（详见下方说明） |
| 14_LogFilePath | string | `/tmp/logs` | 日志文件路径 |
| 15_LogFileName | string | `log-touchIngress.log` | 日志文件名 |

**13_levelLog 配置说明**:

该参数是一个组合值，格式为：`<输出方式><日志等级>`（十位数 + 个位数）

**输出方式（十位数）**:
| 值 | 宏定义 | 说明 |
|----|--------|------|
| 1 | `EZ_LOG_TO_STDOUT` | 输出到标准输出 |
| 2 | `EZ_LOG_TO_FILE` | 输出到文件 |
| 3 | `EZ_LOG_TO_FILE_AND_STDOUT` | 同时输出到标准输出和文件 |

**日志等级（个位数）**:
| 值 | 宏定义 | 说明 |
|----|--------|------|
| 0 | `EZ_LOG_LEVEL_EMERG` | 系统不可用 |
| 1 | `EZ_LOG_LEVEL_ALERT` | 必须马上采取行动的事件 |
| 2 | `EZ_LOG_LEVEL_CRIT` | 关键的事件 |
| 3 | `EZ_LOG_LEVEL_ERR` | 错误事件 |
| 4 | `EZ_LOG_LEVEL_WARNING` | 警告事件 |
| 5 | `EZ_LOG_LEVEL_NOTICE` | 普通但重要的事件 |
| 6 | `EZ_LOG_LEVEL_INFO` | 有用的信息 |
| 7 | `EZ_LOG_LEVEL_DEBUG` | 调试信息 |
| 8 | `EZ_LOG_LEVEL_NONE` | 不记日志 |

**常用配置示例**:
- `37` = 输出到文件 + 标准输出，调试等级（推荐测试时使用）
- `26` = 仅输出到文件，信息等级
- `13` = 仅输出到标准输出，错误等级
- `27` = 仅输出到文件，调试等级

**配置示例**:
```json
{
   "General" : [
      {
         "13_levelLog" : 37,
         "14_LogFilePath" : "/tmp/logs",
         "15_LogFileName" : "log-touchIngress.log"
      }
   ]
}
```

### TouchIngress 部分

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| Host | string | `0.0.0.0` | 监听地址 |
| Port | int | `54321` | 监听端口 |
| Protocol | string | `come.1` | WebSocket 协议 |
| PathPrefix | string | `/come` | WebSocket 路径前缀 |
| CheckPeriod | int | `30` | 心跳检测周期（秒，0=不检测） |
| PeerTimeout | int | `60` | 对端超时（秒） |
| MaxConnections | int | `1000` | 最大连接数 |
| EdgeReportMode | int | `1` | Edge 上报模式（见下方说明） |

### EdgeReportMode 说明

| 值 | 模式 | 说明 |
|----|------|------|
| 0 | 不上报 | Manager 连接后不上报任何 Edge |
| 1 | 上报所有 | Manager 连接后上报所有已连接的 Edge（默认） |
| 2 | 增量上报 | Manager 连接后仅上报有变化的 Edge |

### Edge 离线缓存特性

当 `EdgeReportMode` 为 1 或 2 时，Ingress 会：
- 记录没有 Manager 连接时下线的 Edge
- Manager 上线后根据上报模式上报这些 Edge 的离线状态
- 确保 Manager 能够获取完整的 Edge 状态变化历史

---

## touch_manager 配置

**配置方式**: 环境变量

| 变量 | 默认值 | 说明 |
|------|--------|------|
| `PORT` | `18050` | HTTP 服务端口 |
| `DATABASE_NAME` | `touch_manager.db` | 数据库文件名（相对于 `man/touch_manager/` 目录） |
| `DATABASE_PATH` | 无 | 数据库完整路径（优先级高于 `DATABASE_NAME`） |
| `DEFAULT_INGRESS_ID` | `lili-47.100.49.48` | 默认 Ingress 实例 ID |
| `DEFAULT_INGRESS_HOST` | `47.100.49.48` | 默认 Ingress 主机地址 |
| `DEFAULT_INGRESS_PORT` | `54321` | 默认 Ingress 端口 |
| `INGRESS_CONNECT_TIMEOUT` | `5` | Ingress 连接超时（秒） |
| `INGRESS_RECONNECT_INTERVAL` | `10` | Ingress 重连间隔（秒） |
| `WEBSOCKET_PING_INTERVAL` | `15` | WebSocket ping 间隔（秒） |
| `WEBSOCKET_PING_TIMEOUT` | `5` | WebSocket ping 超时（秒） |
| `EDGE_LIST_BATCH_SIZE` | `50` | 每批次获取的边缘数量 |
| `EDGE_LIST_QUERY_TIMEOUT` | `10` | 查询超时时间（秒） |
| `LOG_LEVEL` | `DEBUG` | 日志级别 |
| `LOG_PATH` | `/tmp/logs` | 日志路径 |
| `LOG_FILE` | `touch_manager.log` | 日志文件名 |

**测试用数据库**: `t-touch_manager.db`
- 启动方式：`DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py`

---

## 配置文件示例

### touchEdge 配置示例

```json
{
   "General" : [
      {
         "13_levelLog" : 37,
         "14_LogFilePath" : "/tmp/logs",
         "15_LogFileName" : "log-touchEdge.log",
         "17_Pid" : "touchEdge.pid"
      }
   ],
   "TouchEdge" : [
      {
         "EdgeId" : "G1509057776",
         "EdgeKey" : "key001",
         "EdgeType" : "touch",
         "IngressHost" : "127.0.0.1",
         "IngressPort" : 54321,
         "ConnectTimeout" : 10,
         "HeartbeatInterval" : 30,
         "ReconnectInterval" : 30,
         "ReconnectMaxRetries" : -1
      }
   ]
}
```

### touchIngress 配置示例

```json
{
   "General" : [
      {
         "13_levelLog" : 37,
         "14_LogFilePath" : "/tmp/logs",
         "15_LogFileName" : "log-touchIngress.log"
      }
   ],
   "TouchIngress" : [
      {
         "Host" : "0.0.0.0",
         "Port" : 54321,
         "Protocol" : "come.1",
         "PathPrefix" : "/come",
         "CheckPeriod" : 30,
         "PeerTimeout" : 60,
         "MaxConnections" : 1000,
         "EdgeReportMode" : 1
      }
   ]
}
```
