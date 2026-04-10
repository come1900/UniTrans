
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
