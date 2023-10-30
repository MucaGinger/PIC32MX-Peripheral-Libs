# 📑 Table of Contents

- [Table of Contents](#-table-of-contents)
- [Introduction to Configuration Registers on PIC32MX Microcontroller](#-introduction-to-configuration-registers-on-pic32mx-microcontroller)
- [Features of the Driver](#-features-of-the-driver)
- [API Documentation and Usage](#-api-documentation-and-usage)
  - [Driver Functions](#driver-functions)

# 📘 Introduction to Configuration Registers on PIC32MX Microcontroller

A PIC32 family device includes several nonvolatile (programmable) Configuration Words that define device behavior. The PIC32 Configuration Words are located in Boot Flash memory and are programmed when the PIC32 Boot Flash region is programmed.

# ✨ Features of the Driver

The Configuration Registers driver currently supports:
- Lock/Unlock sequence of specific system registers
- Lock/Unlock sequence of PPS (Peripheral Pin Select) registers

# 📖 API Documentation and Usage

It's important to note that the `Cfg.c` source file is thoroughly annotated with quality comment blocks for your convenience.

## Driver Functions

### `CFG_UnlockSystemAccess()`
```cpp
INLINE volatile uint32_t CFG_UnlockSystemAccess(void);
```
This function performs system unlock sequence (temporarily disables interrupts and DMA).

### `CFG_LockSystemAccess()`
```cpp
INLINE volatile void CFG_LockSystemAccess(uint32_t intrStatus);
```
This function performs system lock sequence (re-enables interrupts if needed and DMA).

### `CFG_UnlockPpsAccess()`
```cpp
INLINE volatile uint32_t CFG_UnlockPpsAccess(void);
```
This function performs PPS unlock (temporarily disables interrupts and DMA).

### `CFG_LockPpsAccess()`
```cpp
INLINE volatile void CFG_LockPpsAccess(uint32_t intrStatus);
```
This function performs PPS lock (re-enables interrupts if needed and DMA).

# 

&copy; Luka Gacnik, 2023
