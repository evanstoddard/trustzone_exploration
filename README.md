# TrustZone Exploration

Companion code for the [TrustZone Demystified](https://evanstoddard.com/posts/trustzone-demystified_part_1/) blog series on [evanstoddard.com](https://evanstoddard.com).  Each directory corresponds to a part in the series and contains the full source for the firmware projects built and discussed in that post.

## Blog Posts

- [TrustZone Demystified: Part 1](https://evanstoddard.com/posts/trustzone-demystified_part_1/)
- [TrustZone Demystified: Part 1 (Appendix)](https://evanstoddard.com/posts/trustzone-demystified_part_1_appendix/)

## Hardware

All projects target the **NUCLEO-H563ZI** development board, which features an STM32H563 Cortex-M33 MCU with TrustZone support.

## Toolchain

- `arm-none-eabi-gcc`
- CMake 3.22+
- ST's fork of OpenOCD (required for flashing STM32H5 targets — see individual project READMEs)

## Structure

```
part_one/
  ns_pg/    Baseline Hello World project, TrustZone disabled
  tz_pg/    TrustZone-enabled project with Secure and NonSecure images
```
