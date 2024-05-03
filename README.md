# Fast CRC32 Algoritms

CRC32A is the ITU I.363.5 algorithm popularised by BZIP2. CRC32B is the
ITU V.42 algorithm used in Ethernet and popularised by PKZip.
Fast algorithms are implemented based on [fast crc](https://arxiv.org/abs/1009.5949)
implementation.
Thanks to [Michaelangel007](https://github.com/Michaelangel007) for explanations
on CRC32 algorithm differences.

## License

This source code is released for free distribution under the terms of the
BSD Clause 2 License. For details check [LICENSE.md](LICENSE.md).

## Description

Description of CRC implementation is as follows:
 * Polynomial: 0x04C11DB7 or reverted 0xEDB88320
 * Initial value: -1
 * RefIn: False
 * RefOut: False



