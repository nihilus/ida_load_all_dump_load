#include "stdafx.h"

void *x_alloc(size_t size)
{
	return ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

void x_free(void *memory)
{
	::HeapFree(::GetProcessHeap(), 0, memory);
}


void * x_realloc(void *memory, size_t size)
{
	return ::HeapReAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, memory, size);
}
