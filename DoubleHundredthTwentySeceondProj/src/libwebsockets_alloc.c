#include "libwebsockets_private-libwebsockets.h"

#if defined(LWS_PLAT_OPTEE)

#define TEE_USER_MEM_HINT_NO_FILL_ZERO       0x80000000

void *__attribute__((weak))
	TEE_Malloc(uint32_t size, uint32_t hint)
{
	return NULL;
}
void *__attribute__((weak))
	TEE_Realloc(void *buffer, uint32_t newSize)
{
	return NULL;
}
void __attribute__((weak))
	TEE_Free(void *buffer)
{
}

void *lws_realloc(void *ptr, size_t size)
{
	return TEE_Realloc(ptr, size);
}

void *lws_malloc(size_t size)
{
	return TEE_Malloc(size, TEE_USER_MEM_HINT_NO_FILL_ZERO);
}

void lws_free(void *p)
{
	TEE_Free(p);
}

void *lws_zalloc(size_t size)
{
	void *ptr = TEE_Malloc(size, TEE_USER_MEM_HINT_NO_FILL_ZERO);
	if (ptr)
		memset(ptr, 0, size);
	return ptr;
}

void lws_set_allocator(void *(*cb)(void *ptr, size_t size))
{
	(void)cb;
}
#else

static void *_realloc(void *ptr, size_t size)
{
	if (size)
#if defined(LWS_PLAT_OPTEE)
		return (void *)TEE_Realloc(ptr, size);
#else
		return (void *)realloc(ptr, size);
#endif
	else if (ptr)
		free(ptr);
	return NULL;
}

void *(*_lws_realloc)(void *ptr, size_t size) = _realloc;

void *lws_realloc(void *ptr, size_t size)
{
	return _lws_realloc(ptr, size);
}

void *lws_zalloc(size_t size)
{
	void *ptr = _lws_realloc(NULL, size);
	if (ptr)
		memset(ptr, 0, size);
	return ptr;
}

void lws_set_allocator(void *(*cb)(void *ptr, size_t size))
{
	_lws_realloc = cb;
}
#endif
