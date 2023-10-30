# 📘 Introduction to PIC32MX-Peripheral-Libs

![Language](https://img.shields.io/badge/language-C-brightgreen) ![Platform](https://img.shields.io/badge/platform-PIC32-blue) ![License](https://img.shields.io/badge/license-MIT-green) ![Contributions](https://img.shields.io/badge/contributions-welcome-brightgreen.svg)

This project offers a low-level API that provides control over some of PIC32MX microcontroller peripheral modules. Despite the fact that library was developed using PIC32MX family, it could be ported to any other of PIC32 family of MCUs.

Primary intention of developing my own set of libraries was to optimize low-level driver performance by excluding overhead provided by generic vendor libraries for PIC32 MCUs. Additionally, as I was developing these libraries I was working on drivers for some other, external devices whose repositories you can find on my [main page](https://github.com/MucaGinger).

All the modules, whose library was developed so far, are covered in the following sub-folders:
- [Serial Peripheral Interface](Spi)
- [Programmable Inputs Outputs](Pio)
- [Timers](Tmr)
- [Oscillator](Osc)
- [Interrupt Controller](Ic)
- [Configuration Registers](Cfg)

Check the links above for an extensive explanation of each module's driver.

# :bookmark_tabs: Table of Contents

- [📘 Introduction to PIC32MX-Peripheral-Libs](#-introduction-to-pic32mx-peripheral-libs)
- [:bookmark\_tabs: Table of Contents](#bookmark_tabs-table-of-contents)
- [🛠️ Setting Up Your Environment](#️-setting-up-your-environment)
  - [Software and Build Process](#software-and-build-process)
  - [Emulating the Microcontroller](#emulating-the-microcontroller)
- [📚 General Dependencies](#-general-dependencies)
- [🚀 Future Development](#-future-development)
- [📞 Getting in Touch and Contributions](#-getting-in-touch-and-contributions)
- [©️ License and Usage Terms](#️-license-and-usage-terms)

# 🛠️ Setting Up Your Environment

The development, testing, and API validation of the individual driver utilized the following setup.

## Software and Build Process

The project development utilized MPLAB X (v6.05), paired with Microchip's XC32 (v4.21) toolchain for building the project. For detailed information on required libraries for using individual driver, please refer to the [Library Dependencies](#-general-dependencies) section.

## Emulating the Microcontroller

Microchip's PicKit4 served as the emulator and debugger for this project, offering a cost-effective solution for successful debugging and code uploading from MPLAB X IDE to the target MCU. The specific MCU used was the PIC32MX170F256B, accompanied by its essential external components.

# 📚 General Dependencies

Each module requires one or more of the following dependencies:
- Standard Libraries
- XC32 compiler libraries: `xc.h`, `cp0defs.h`, and `attribs.h`

Compiler libraries are mainly used for interrupt handler semantics, interrupt control, and accessing coprocessor registers *CP0* for some specialized tasks.

> [!NOTE]\
> It should be mentioned that the necessary startup file <code>.s</code>, linker file <code>.ld</code> (or <code>.sct</code> for ARM compilers), and MCU configuration bits (aka. fuses) are not provided in this project as these are toolchain and platform specifics and should be handled by the user.

# 🚀 Future Development

Looking ahead of the PIC32MX peripheral libraries development the remaining peripheral module driver's libraries would need to be implemented, tested, and their documentation provided.

# 📞 Getting in Touch and Contributions

If you encounter any bugs or have suggestions, please [raise an issue](https://github.com/MucaGinger/PIC32MX-Peripheral-Libs/issues). Alternatively, feel free to contact me directly via <a href="mailto:lgacnik97@gmail.com">email</a> if you're interested in contributing or taking this project to the next level.

# ©️ License and Usage Terms

This project is licensed under the MIT License - see the [LICENCE](LICENCE) file for details.

#

&copy; Luka Gacnik, 2023
