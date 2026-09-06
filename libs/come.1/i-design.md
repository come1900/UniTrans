
# come_1设计

**设计说明**：
- `come_1.h` 作为基础消息定义，位于根目录，可被不同的序列化方式（JSON、protobuf 等）复用
- `json/` 目录包含 JSON 序列化的完整实现
- 未来可以添加 `protobuf/` 等其他序列化方式的目录

## 消息体设计
设计一套设备上线、上报消息、接收消息的系列消息体：
- 所有消息配以AckXXXX消息对应
- 所有消息用class表示
- 设备上线必须携带：id、key、类型、localIP等
- 数据类型可以用简单类型和stl类型
- 消息定义文件：come_1.h

## json解析器设计
主要文件说明：
- `come_1.h` - 消息定义
- `come.1.json.cpp` - 使用nlohmann/json.hpp对come_1.h中定义的消息体进行encode和decode
- `come.1.json.h` - 对应的头文件，主要用于定义序列化类
- `Makefile` - 编译测试程序
- `ut-come.1.json.cpp` - 测试程序
  - 测试结果分行打印，每行一个用例
  - 默认只打印：测试目的、结果
  - 支持命令行参数打印详情

## 实现要求

**一般要求**：
- 消息encode和decode对外接口需返回结果值
- 外部调用者可能不处理异常，内部需确保不因异常导致程序崩溃

**边界处理**：
- 考虑字段超范围问题（如：int型字段输入超过40亿）
- 处理数据内容异常情况

**依赖项**：
- **nlohmann/json.hpp** (header-only C++库)
  - 主要功能：对数据进行json封装和解封装
  - 编译参数：`-I${HOME}/libs/json-hpp`
  - 链接参数：无

## 协议层与实现分离原则

**核心思想**：对外协议不绑定具体实现，通过转换接口实现可插拔的内部配置格式。

**分层设计**：
1. **对外统一协议层**（稳定不变）
   - 使用 `ConfigUpdate_tunnelService` 作为标准透传隧道配置协议
   - 所有外部交互（JSON-RPC 消息）都使用此格式
   - 不暴露任何具体透传软件的细节

2. **内部转换层**（可扩展）
   - 提供 `toFrpcConfig()` 和 `fromFrpcConfig()` 双向转换接口
   - 将统一的 tunnelService 转换为具体软件的配置格式
   - 将来支持其他软件时，只需添加类似的转换接口

3. **具体实现配置层**（可替换）
   - `FrpcConfig` 专门用于 frpc 配置文件格式
   - 内部存储采用扁平结构，JSON 输出采用嵌套格式
   - 完全独立于对外协议

**扩展示例**（支持 ngrok 时）：
```cpp
// 新增 ngrok 配置结构
struct NgrokConfig { ... };

// 新增转换接口（保持对外协议不变）
static bool toNgrokConfig(const ConfigUpdate_tunnelService& msg, NgrokConfig& ngrokCfg);
static bool fromNgrokConfig(const NgrokConfig& ngrokCfg, ..., ConfigUpdate_tunnelService& msg);
```

**优势**：
- 外部调用者无需关心使用哪种透传软件
- 更换或新增透传软件时，对外接口保持不变
- 符合开闭原则：对扩展开放，对修改关闭
