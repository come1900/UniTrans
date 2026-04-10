## 目录结构

```
come.1.json/
├── come_1.h              # 基础消息定义（可被不同序列化方式复用）
├── json/                 # JSON 序列化实现
│   ├── come.1.json.h     # JSON 编解码器接口
│   ├── come.1.json.cpp   # JSON 编解码器实现
│   ├── ut-come.1.json.cpp # JSON 测试程序
│   └── Makefile          # JSON 版本的构建文件
├── README.md             # 本文档
└── i-design.md           # 设计文档
```


## 消息一览
| 序号 | 说明                     | 消息名称           | 返回消息名称         |
|------|--------------------------|--------------------|----------------------|
| 1    | 基础消息类               | `MsgCome`          | —                    |
| 2    | 应答消息基类             | `AckMsgCome`       | —                    |
| 3    | 设备上线消息             | `DeviceOnline`     | `AckDeviceOnline`    |
| 4    | 设备上报消息             | `DeviceReport`     | `AckDeviceReport`    |
| 5    | 服务器下发命令           | `ServerCommand`    | `AckServerCommand`   |

## 消息交互时序图

```plantuml
@startuml 设备管理消息交互时序图

title 设备管理消息交互时序图

participant "设备\n(Device)" as Device
participant "服务器\n(Server)" as Server

== 1. 设备上线流程 ==
Device -> Server: DeviceOnline\n{id, key, type, localIP}
activate Server
Server -> Server: 验证设备身份
Server --> Device: AckDeviceOnline\n{code, msg, success}
deactivate Server
note right: code=0 表示成功\nsuccess=true 表示上线成功

== 2. 设备上报数据流程 ==
Device -> Server: DeviceReport\n{id, data}
activate Server
Server -> Server: 处理上报数据
Server --> Device: AckDeviceReport\n{code, msg, received, id, timestamp}
deactivate Server
note right: received=true 表示接收成功

== 3. 服务器下发命令流程 ==
Server -> Device: ServerCommand\n{id, command_type, payload}
activate Device
Device -> Device: 执行命令
Device --> Server: AckServerCommand\n{code, msg, id, command_type, handled, result}
deactivate Device
note right: handled=true 表示处理成功

@enduml
```
