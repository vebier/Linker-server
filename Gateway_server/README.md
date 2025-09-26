# Gateway Server 项目

基于 C++14 和 MSVC2022 开发的高性能网关服务器，提供 HTTP 路由注册和处理功能。

## 项目结构

text

```
gateway/
│  config.ini                    # 配置文件
│  Gateway_server.cpp            # 主程序入口
│  Gateway_server.vcxproj        # VS项目文件
│  Gateway_server.vcxproj.filters
│  Gateway_server.vcxproj.user
│  message.proto                 # Protobuf定义
│  PropertySheet.props           # 编译配置
├─grpc/                          # gRPC相关文件
│      message.grpc.pb.cc
│      message.grpc.pb.h
│      message.pb.cc
│      message.pb.h
│      RPConPool.cpp             # gRPC连接池
│      RPConPool.h
├─include/                       # 头文件目录
│      AsioIOContextPool.h       # IO上下文池
│      const.h                   # 常量定义
│      CServer.h                 # 服务器主类
│      HttpConnection.h          # HTTP连接处理
│      LogicSystem.h             # 业务逻辑系统
│      SectionInfo.cpp           # 配置解析
│      SectionInfo.h
│      Singleton.h               # 单例模板
│      ThreadPool.h              # 线程池
│      VarifyGrpcClient.h        # 验证服务gRPC客户端
├─src/                           # 源文件目录
│      AsioIOContextPool.cpp
│      CServer.cpp
│      HttpConnection.cpp
│      LogicSystem.cpp
│      VarifyGrpcClient.cpp
```

## 功能特性

- 🚀 高性能 HTTP 请求处理
- 📋 基于 I/O 完成端口的异步网络模型
- 🔌 模块化设计，易于扩展
- 🛡️ 异常处理和错误日志
- 📊 请求监控和统计功能
- 🔗 gRPC客户端支持

## 技术栈

- **编译器**: MSVC2022 (C++14 标准)
- **开发环境**: Visual Studio 2022
- **网络模型**: Windows I/O 完成端口(IOCP)
- **并发处理**: 多线程异步IO
- **通信协议**: HTTP, gRPC

## 快速开始

### 系统要求

1. Visual Studio 2022
2. Windows10 SDK
3. C++14 兼容的 MSVC 编译器

### 第三方库

1. boost v1.89.0
2. jsoncpp
3. grpc v1.34.0

### 构建项目

1. 使用 Visual Studio 2022 打开 `Gateway_server.sln` 解决方案文件
2. 选择构建配置（Debug/Release）
3. 构建项目

### 运行项目

1. 配置 `config.ini` 文件中的服务器参数
2. 运行生成的 `Gateway_server.exe` 可执行文件
3. 服务器将监听指定端口，接收HTTP请求

## 核心组件

### CServer - 服务器主类

负责初始化网络环境、管理连接和调度工作线程。

**主要方法:**

- `run()` - 启动服务器
- `Stop()` - 停止服务器

### HttpConnection - HTTP 连接处理

处理单个 HTTP 连接的读写操作和协议解析。

**功能:**

- HTTP 请求解析
- 响应生成和发送
- 连接状态管理

### LogicSystem - 业务逻辑系统

处理业务逻辑和路由分发。

**功能:**

- 路由注册和管理
- 请求处理分发
- 业务逻辑执行

### AsioIOContextPool - IO上下文池

管理异步IO操作上下文，提高并发处理能力。

### VarifyGrpcClient - gRPC客户端

提供与验证服务的gRPC通信能力。

### Singleton - 单例模式模板

提供线程安全的单例对象管理。

## 配置说明

项目使用 `config.ini` 文件进行配置，包括:

- 服务器监听端口
- 线程池大小
- gRPC服务地址
- 连接超时设置
- 缓冲区大小

## 开发指南

### 添加新的路由处理

1. 在 LogicSystem 中添加路由处理函数
2. 注册路由到处理函数映射
3. 实现对应的业务逻辑

### 添加新的gRPC服务

1. 更新 `message.proto` 文件定义新的服务
2. 重新生成gRPC代码
3. 在对应客户端类中实现服务调用

## 性能优化建议

1. **连接池管理**: 重用 HTTP 连接对象减少内存分配
2. **缓冲区优化**: 使用预分配缓冲区减少内存碎片
3. **线程池调优**: 根据 CPU 核心数调整工作线程数量
4. **内存池**: 实现自定义内存池管理频繁分配的小对象
5. **gRPC连接复用**: 使用连接池管理gRPC通道

## 故障排除

### 常见问题

1. **端口占用错误**
   - 检查端口是否被其他程序占用
   - 修改配置文件中的端口号
2. **内存泄漏**
   - 使用 Visual Studio 诊断工具检测
   - 确保正确释放所有分配的资源
3. **gRPC连接失败**
   - 检查目标服务是否正常运行
   - 验证gRPC服务地址配置
4. **性能问题**
   - 调整连接池大小
   - 检查网络配置和硬件限制

## 扩展功能

可以考虑添加以下功能增强网关能力:

1. **认证授权**: 添加 JWT 或 OAuth 认证支持
2. **限流熔断**: 实现请求限流和故障熔断机制
3. **监控指标**: 添加 Prometheus 指标导出
4. **配置热更新**: 支持运行时配置更新
5. **负载均衡**: 集成负载均衡算法

## 许可证

MIT License - 详见项目 LICENSE 文件

## 支持

如有问题，请留言或提交Issue