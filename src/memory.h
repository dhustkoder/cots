#ifndef COTS_MEMORY_H_
#define COTS_MEMORY_H_


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



#endif