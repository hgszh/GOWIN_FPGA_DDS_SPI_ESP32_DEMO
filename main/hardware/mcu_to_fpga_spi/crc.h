// vim: ts=4 sw=4 expandtab

// THIS IS GENERATED C CODE.
// https://bues.ch/h/crcgen
// 
// This code is Public Domain.
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
// RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
// NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
// USE OR PERFORMANCE OF THIS SOFTWARE.

#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>

// CRC polynomial coefficients: x^8 + x^2 + x + 1
//                              0x7 (hex)
// CRC width:                   8 bits
// CRC shift direction:         left (big endian)
// Input word width:            32 bits

#ifdef b
# undef b
#endif
#define b(x, b) (((x) >> (b)) & 1u)

uint8_t crc(uint8_t crc_in, uint32_t data)
{
    uint8_t ret;
    ret  = (uint8_t)(b(crc_in, 4) ^ b(crc_in, 6) ^ b(crc_in, 7) ^ b(data, 0) ^ b(data, 6) ^ b(data, 7) ^ b(data, 8) ^ b(data, 12) ^ b(data, 14) ^ b(data, 16) ^ b(data, 18) ^ b(data, 19) ^ b(data, 21) ^ b(data, 23) ^ b(data, 28) ^ b(data, 30) ^ b(data, 31)) << 0;
    ret |= (uint8_t)(b(crc_in, 0) ^ b(crc_in, 4) ^ b(crc_in, 5) ^ b(crc_in, 6) ^ b(data, 0) ^ b(data, 1) ^ b(data, 6) ^ b(data, 9) ^ b(data, 12) ^ b(data, 13) ^ b(data, 14) ^ b(data, 15) ^ b(data, 16) ^ b(data, 17) ^ b(data, 18) ^ b(data, 20) ^ b(data, 21) ^ b(data, 22) ^ b(data, 23) ^ b(data, 24) ^ b(data, 28) ^ b(data, 29) ^ b(data, 30)) << 1;
    ret |= (uint8_t)(b(crc_in, 0) ^ b(crc_in, 1) ^ b(crc_in, 4) ^ b(crc_in, 5) ^ b(data, 0) ^ b(data, 1) ^ b(data, 2) ^ b(data, 6) ^ b(data, 8) ^ b(data, 10) ^ b(data, 12) ^ b(data, 13) ^ b(data, 15) ^ b(data, 17) ^ b(data, 22) ^ b(data, 24) ^ b(data, 25) ^ b(data, 28) ^ b(data, 29)) << 2;
    ret |= (uint8_t)(b(crc_in, 1) ^ b(crc_in, 2) ^ b(crc_in, 5) ^ b(crc_in, 6) ^ b(data, 1) ^ b(data, 2) ^ b(data, 3) ^ b(data, 7) ^ b(data, 9) ^ b(data, 11) ^ b(data, 13) ^ b(data, 14) ^ b(data, 16) ^ b(data, 18) ^ b(data, 23) ^ b(data, 25) ^ b(data, 26) ^ b(data, 29) ^ b(data, 30)) << 3;
    ret |= (uint8_t)(b(crc_in, 0) ^ b(crc_in, 2) ^ b(crc_in, 3) ^ b(crc_in, 6) ^ b(crc_in, 7) ^ b(data, 2) ^ b(data, 3) ^ b(data, 4) ^ b(data, 8) ^ b(data, 10) ^ b(data, 12) ^ b(data, 14) ^ b(data, 15) ^ b(data, 17) ^ b(data, 19) ^ b(data, 24) ^ b(data, 26) ^ b(data, 27) ^ b(data, 30) ^ b(data, 31)) << 4;
    ret |= (uint8_t)(b(crc_in, 1) ^ b(crc_in, 3) ^ b(crc_in, 4) ^ b(crc_in, 7) ^ b(data, 3) ^ b(data, 4) ^ b(data, 5) ^ b(data, 9) ^ b(data, 11) ^ b(data, 13) ^ b(data, 15) ^ b(data, 16) ^ b(data, 18) ^ b(data, 20) ^ b(data, 25) ^ b(data, 27) ^ b(data, 28) ^ b(data, 31)) << 5;
    ret |= (uint8_t)(b(crc_in, 2) ^ b(crc_in, 4) ^ b(crc_in, 5) ^ b(data, 4) ^ b(data, 5) ^ b(data, 6) ^ b(data, 10) ^ b(data, 12) ^ b(data, 14) ^ b(data, 16) ^ b(data, 17) ^ b(data, 19) ^ b(data, 21) ^ b(data, 26) ^ b(data, 28) ^ b(data, 29)) << 6;
    ret |= (uint8_t)(b(crc_in, 3) ^ b(crc_in, 5) ^ b(crc_in, 6) ^ b(data, 5) ^ b(data, 6) ^ b(data, 7) ^ b(data, 11) ^ b(data, 13) ^ b(data, 15) ^ b(data, 17) ^ b(data, 18) ^ b(data, 20) ^ b(data, 22) ^ b(data, 27) ^ b(data, 29) ^ b(data, 30)) << 7;
    return ret;
}
#undef b

#endif /* CRC_H_ */