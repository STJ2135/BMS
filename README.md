# STM32 BMS

基于 **STM32F103C8T6 + BQ769x0 + RT-Thread** 的 3～5 串锂电池管理系统（BMS）固件。工程采用 Cortex-M3 裸机外设 + RT-Thread 多线程架构，实现了电池采样、状态分析、软件/硬件保护、被动均衡、充放电控制、信息输出以及 CAN/RS485 底层通信驱动。

> 当前默认配置为 BQ76920 系列、最多 5 节电芯、1 路温度采样，默认电池类型为三元锂电池。实际部署前必须根据硬件参数重新校验保护阈值。

## 功能概览

| 模块 | 功能 |
| --- | --- |
| 电池监控 | 电芯电压、电池总压、电流、温度采样，数据排序及有效性统计 |
| 电池分析 | SOC、平均/最大/最小单体电压、最大压差、实时功率、剩余容量估算 |
| 电池保护 | 过压、欠压、充电过流、放电过流、短路、充放电高低温保护 |
| 能量管理 | 被动均衡、基于 SOC 和系统模式的充放电策略 |
| 功率控制 | 统一管理充电/放电 MOS，协调用户许可、策略请求和保护锁定 |
| 信息输出 | 周期性打印电池状态，可通过 FinSH 命令开关或立即输出 |
| 调试命令 | 通过 RT-Thread msh 控制充放电、均衡及信息打印 |
| 通信驱动 | 软件 I2C、CAN 中断/消息队列、RS485 半双工收发和方向控制 |
| 状态指示 | 1 路系统心跳灯和 4 路容量指示 LED |

当前未实现或仅保留接口的功能包括：SOH/SOP/SOE、循环次数统计、睡眠低功耗策略以及完整的 BMS 上位机通信协议。

## 硬件平台

| 项目 | 配置 |
| --- | --- |
| MCU | STM32F103C8T6，Cortex-M3，72 MHz，64 KB Flash，20 KB SRAM |
| AFE | BQ769x0 系列；当前按 BQ76920 的 3～5 串、1 路温度通道配置 |
| RTOS | RT-Thread 3.1.5（kernel、device、FinSH） |
| 工具链 | Keil MDK-ARM V5，ARM Compiler 5 |
| 配置工具 | STM32CubeMX，STM32Cube FW_F1 V1.8.4 |
| 调试器 | J-Link 或兼容 CMSIS-DAP/ST-Link 调试器 |

### 引脚分配

| 功能 | 引脚 | 说明 |
| --- | --- | --- |
| AFE I2C SDA | PB13 | GPIO 模拟 I2C |
| AFE I2C SCL | PB14 | GPIO 模拟 I2C |
| AFE ALERT | PB12 | EXTI 上升沿中断 |
| AFE TS1 控制 | PA15 | 温度采样相关控制 |
| FinSH / 日志串口 | PA9 / PA10 | USART1，115200-8-N-1 |
| RS485 串口 | PA2 / PA3 | USART2，115200-8-N-1 |
| RS485 方向控制 | PA4 | 低电平接收，高电平发送 |
| RS485 电源控制 | PA5 | GPIO 输出 |
| CAN | PA11 / PA12 | CAN1，500 kbit/s |
| CAN 电源控制 | PB10 | GPIO 输出 |
| 系统心跳灯 | PB5 | 高电平点亮 |
| 容量指示灯 1～4 | PB6～PB9 | 高电平点亮 |

## 软件架构

各层只向下依赖，应用业务不直接操作寄存器或 GPIO：

```text
App  应用业务：监控、保护、分析、能量、信息、命令、功率仲裁
  |
Mid  软件服务：通信协议占位层
  |
Int  外部硬件接口：BQ769x0、RS485、LED
  |
Dri  片上外设驱动：软件 I2C、CAN、板级初始化
  |
Com  基础层：公共类型、配置、日志与工具
```

主要数据流为：

```text
BQ769x0 采样 -> 监控模块汇总 -> 保护/分析/能量模块消费 -> 信息输出
                                      |
                                      +-> 功率控制统一下发 MOS 状态
```

`App_Power` 是充电和放电输出的唯一所有者。最终输出由“用户许可”“策略请求”“保护锁定”三级共同决定，其他模块只能提交请求或保护锁定，不能直接改写 MOS 状态。

## 线程模型

| 线程 | 周期 | 职责 |
| --- | --- | --- |
| `monitor` | 250 ms | 采样与数据整理 |
| `protect` | 200 ms | 软件保护判定与恢复 |
| `analysis` | 1000 ms | SOC、压差和功率计算，均衡触发 |
| `energy` | 200 ms | 充放电策略与均衡周期管理 |
| `info` | 2000 ms | 周期状态输出 |
| `comm` | 2000 ms | 通信协议占位线程 |
| `can_send` | 事件驱动 | 从 CAN 发送队列取消息并发送 |
| `main` | 1000 ms | 系统心跳、主循环 |

RT-Thread 中断接收路径仅负责完成硬件操作和投递消息，耗时业务处理放在线程中执行。

## 目录结构

```text
BMS/
├─ App/                        应用业务与运行入口
│  ├─ App_BMS.c                模块组装与初始化
│  ├─ App_Monitor.c            数据采集与监控
│  ├─ App_Protect.c            保护策略
│  ├─ App_Analysis.c           电池状态分析
│  ├─ App_Energy.c             均衡与充放电策略
│  ├─ App_Power.c              充放电 MOS 输出仲裁
│  ├─ App_Info.c               电池信息输出
│  ├─ App_Shell.c              FinSH/msh 调试命令
│  └─ App_Config.h             保护、SOC、均衡等业务参数
├─ Com/                        公共类型、配置、日志和工具
├─ Dri/                        软件 I2C、CAN、板级驱动
├─ Int/                        BQ769x0、RS485、LED 外部接口
├─ Mid/                        与硬件无关的软件服务
├─ Core/                       CubeMX 生成的内核与外设初始化
├─ Drivers/                    STM32 HAL、CMSIS
├─ Middlewares/                RT-Thread 第三方组件
├─ MDK-ARM/                    Keil 工程、启动文件、RT-Thread 配置
├─ BMS.ioc                     CubeMX 工程文件
└─ pic/                        工程图片资料
```

## 关键配置

| 文件 | 用途 |
| --- | --- |
| `App/App_Config.h` | 电池类型、保护阈值、SOC/均衡参数、额定容量 |
| `Com/Com_Config.h` | 电芯数量、温度通道数量、温度测量范围 |
| `BMS.ioc` | CubeMX 外设与引脚配置 |
| `MDK-ARM/BMS.uvprojx` | Keil 目标、源文件、宏和头文件路径 |
| `MDK-ARM/RTE/RTOS/rtconfig.h` | RT-Thread 功能裁剪与线程资源 |
| `Core/Src/main.c` | CubeMX 初始化入口与用户代码挂接点 |

默认额定容量为 `2.2 Ah`，该值尚未通过容量测试仪校准。默认保护参数面向三元锂电池；如使用磷酸铁锂或钛酸锂电池，应修改 `App/App_Config.h` 中的初始参数并重新验证。

## 构建与烧录

### 环境准备

1. 安装 Keil MDK-ARM V5 和 ARM Compiler 5。
2. 安装 STM32F1xx Device Family Pack。
3. 安装 J-Link、ST-Link 或 CMSIS-DAP 对应的调试驱动。
4. 如需重新生成外设代码，安装 STM32CubeMX 及 STM32Cube FW_F1 V1.8.4。

### Keil 构建

1. 打开 `MDK-ARM/BMS.uvprojx`。
2. 选择 `BMS` 目标。
3. 执行 `Rebuild`。
4. 构建成功后输出位于：
   - `MDK-ARM/BMS/BMS.axf`
   - `MDK-ARM/BMS/BMS.hex`
5. 连接调试器并执行 `Download`。

### VS Code 辅助开发

可直接打开 `MDK-ARM/BMS.code-workspace` 进行源码浏览和编辑。IntelliSense 配置位于 `.vscode/c_cpp_properties.json`，其中包含当前开发机的绝对路径；在其他电脑使用时需要按实际 SDK 路径调整。

## 常用 FinSH 命令

通过 USART1（115200-8-N-1）连接串口终端后，可使用以下命令：

| 命令 | 功能 |
| --- | --- |
| `open_chg` / `close_chg` | 允许 / 禁止充电 |
| `open_dsg` / `close_dsg` | 允许 / 禁止放电 |
| `open_balance` / `close_balance` | 允许 / 禁止均衡 |
| `load_detect` | 检测负载 |
| `open_info` / `close_info` | 打开 / 关闭周期信息输出 |
| `show_info` | 立即打印一次电池信息 |

命令修改的是“用户许可”，不会绕过保护锁定。关闭命令立即生效，打开命令还需要经过能量管理策略判定。

## 开发约定

- 新增业务模块时保持 `App -> Mid -> Int -> Dri -> Com` 的依赖方向。
- AFE 中断回调中只做必要的标志置位、锁定和消息投递，不执行阻塞操作。
- 修改 CubeMX 配置后保留 `USER CODE` 区域，并在生成代码后复核外设初始化。
- 新增源文件后需要同步加入 `MDK-ARM/BMS.uvprojx` 的对应分组。
- 参数单位统一写在变量名或注释中，电流为 A、电压为 V、温度为 ℃、时间为 ms/s。
- 提交前确认 `MDK-ARM/BMS/`、`.map`、`.lst`、`.uvguix.*` 等构建产物未被纳入版本库。

## 已知限制与安全说明

- 当前仅针对 3～5 串 BQ76920 配置，其他 BQ769x0 型号需要同步修改电芯数量、温度通道和 AFE 寄存器配置。
- 通信层目前为占位实现，CAN 和 RS485 只有底层收发能力，尚无统一协议。
- 睡眠模式、SOH、SOP、SOE 和循环寿命统计尚未完成。
- 默认参数不能替代完整的硬件测试。首次上电应使用限流电源和模拟电芯，逐项验证采样精度、保护阈值、延时、MOS 动作和恢复逻辑。
- BMS 参数错误可能导致电池过充、过放、过热、起火或爆炸。量产和商用前必须完成独立的安全评估与认证。

## 许可证

本工程应用层源码文件头使用 `SPDX-License-Identifier: CC BY-NC 4.0`，仅允许非商业使用。RT-Thread、STM32 HAL、CMSIS 及其他第三方组件遵循各自原始许可证，使用时请分别遵守对应条款。

许可协议详情：<https://creativecommons.org/licenses/by-nc/4.0/>
