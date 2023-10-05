#pragma once
#include <stdint.h>

namespace ns3
{
    struct BigEndian
{
	static uint16_t constexpr Uint16( uint8_t fst , uint8_t snd)
	{
		return uint16_t(snd) | uint16_t( fst)<<8;
	}
    constexpr static uint16_t Uint16(uint8_t b[2])
    {
        return uint16_t(b[1]) | uint16_t(b[0]) << 8;
    }
    static void PutUint64(uint8_t * b,  uint64_t v)
    {
	// _ = b[7] // early bounds check to guarantee safety of writes below
	b[0] = uint8_t(v >> 56);
	b[1] = uint8_t(v >> 48);
	b[2] = uint8_t(v >> 40);
	b[3] = uint8_t(v >> 32);
	b[4] = uint8_t(v >> 24);
	b[5] = uint8_t(v >> 16);
	b[6] = uint8_t(v >> 8);
	b[7] = uint8_t(v);
    }

};
}