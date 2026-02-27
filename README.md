# STM32 CLI Control System based on FreeRTOS

## Overview
MCU Shell is a robust STM32-based embedded application designed to simulate a command-line interface for controlling and monitoring various hardware peripherals. The project implements an efficient, non-blocking communication architecture using UART with DMA and a custom Circular Buffer, ensuring reliable data reception and processing.

Built with scalability in mind, the system uses a centralized command registry to seamlessly map incoming UART packets to specific peripheral actions.

## Key Features
- **Efficient UART Communication:** Utilizes UART Rx/Tx with DMA and a custom Circular Buffer to prevent data loss and minimize CPU overhead.
- **RTOS Task Management:** Integrates FreeRTOS features (Semaphores, Message Queues, and Thread Flags) to synchronize UART reception and command execution tasks safely.
- **Custom Packet Protocol:** Ensures data integrity by implementing a structured packet format with header verification (`0xAA`), length validation, and checksum calculation.
- **Extensible Command Handler:** Uses a scalable registry pattern (`cmd_registry`) making it easy to add new peripherals and actions without modifying the core logic.

## Supported Peripherals
- **GPIO:** RGB LED control (Red, Green, Blue) with Set/Reset/Toggle operations.
- **ADC:** Potentiometer voltage reading with real-time UART feedback.
- **PWM:** Buzzer control (Play/Stop).
- **I2C:** AHT20 sensor integration for reading environmental Temperature and Humidity.

## Architecture & Protocol
### System Flow
1. **Reception:** Data is received continuously via UART DMA into a Circular Buffer.
2. **Parsing:** A dedicated RTOS task parses the buffer, verifying the `0xAA` header and validating the checksum.
3. **Dispatch:** Validated packets are pushed to an RTOS Message Queue.
4. **Execution:** The Command Handler task consumes the queue and triggers the corresponding peripheral API based on the `cmd_type` and `act_type`.

### Custom Packet Structure
| Header | Length | Command Type | Action Type | Value | Checksum |
| :---: | :---: | :---: | :---: | :---: | :---: |
| 1 Byte (`0xAA`) | 1 Byte | 1 Byte | 1 Byte | 1 Byte | 1 Byte |

*Checksum is calculated by summing all preceding bytes (Header + Length + Command + Action + Value).*

## Directory Structure
- `App/`: High-level application logic (Command routing, UART RTOS tasks).
- `Lib/`: Low-level peripheral drivers and middleware (Circular Buffer, AHT20, Potentiometer, Buzzer, GPIO).