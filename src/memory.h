#ifndef COTS_MEMORY_H_
#define COTS_MEMORY_H_
#include <stdint.h>

static inline uint8_t memread_u8(const void* const ptr)
{
	const uint8_t* const m = ptr;
	return *m;
}

static inline uint16_t memread_u16(const void* const ptr)
{
	const uint8_t* const m = ptr;
	const uint16_t r = (*m)|((*(m + 1))<<8);
	return r;
}

static inline uint32_t memread_u32(const void* const ptr)
{
	const uint8_t* const m = ptr;
	const uint32_t r = ((*(m + 3))<<24)|
	                   ((*(m + 2))<<16)|
	                   ((*(m + 1))<<8)|
	                   (*m);
	return r;
}

static inline void memwrite_u8(void* const ptr, const uint8_t data)
{
	*((uint8_t*)ptr) = data;
}

static inline void memwrite_u16(void* const ptr, const uint16_t data)
{
	uint8_t* const m = ptr;
	*m = data&0x00FF;
	*(m + 1) = (data&0xFF00)>>8;
}

static inline void memwrite_u32(void* const ptr, const uint32_t data)
{
	uint8_t* const m = ptr;
	*m = data&0x00FF;
	*(m + 1) = (data&0xFF00)>>8;
	*(m + 2) = (data&0xFF0000)>>16;
	*(m + 3) = (data&0xFF000000)>>24;
}


#endif