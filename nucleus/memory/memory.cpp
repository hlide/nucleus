/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "memory.h"
#include "nucleus/common.h"
#include "nucleus/logger/logger.h"

#ifdef NUCLEUS_TARGET_WINDOWS
#include <Windows.h>
#endif
#ifdef NUCLEUS_TARGET_LINUX
#include <sys/mman.h>
#endif
#ifdef NUCLEUS_TARGET_OSX
#include <sys/mman.h>
#define MAP_ANONYMOUS MAP_ANON
#endif

namespace mem {

Memory::Memory() {
    // Reserve 4 GB of memory for any 32-bit pointer in the PS3 memory
#if defined(NUCLEUS_TARGET_UWP)
    m_base = nullptr;
#elif defined(NUCLEUS_TARGET_WINDOWS)
    m_base = VirtualAlloc(nullptr, 0x100000000ULL, MEM_RESERVE, PAGE_NOACCESS);
#elif defined(NUCLEUS_TARGET_LINUX) || defined(NUCLEUS_TARGET_OSX)
    m_base = ::mmap(nullptr, 0x100000000ULL, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
#endif

    // Check errors
    if (m_base == 0 || m_base == (void*)-1) {
        logger.error(LOG_MEMORY, "Could not reserve memory");
    }

    // Initialize segments
    m_segments[SEG_MAIN_MEMORY].init(this, 0x00010000, 0x2FFF0000);
    m_segments[SEG_USER_MEMORY].init(this, 0x10000000, 0x10000000);
    m_segments[SEG_RSX_MAP_MEMORY].init(this, 0x40000000, 0x10000000);
    m_segments[SEG_MMAPPER_MEMORY].init(this, 0xB0000000, 0x10000000);
    m_segments[SEG_RSX_LOCAL_MEMORY].init(this, 0xC0000000, 0x10000000);
    m_segments[SEG_STACK].init(this, 0xD0000000, 0x10000000);
    m_segments[SEG_SPU].init(this, 0xF0000000, 0x10000000);

    // Allocate SPU-related memory
    m_segments[SEG_SPU].alloc(0x10000000);
}

Memory::~Memory() {
    bool success;
#if defined(NUCLEUS_TARGET_UWP)
    success = false;
#elif defined(NUCLEUS_TARGET_WINDOWS)
    success = SUCCEEDED(VirtualFree(m_base, 0, MEM_RELEASE));
#elif defined(NUCLEUS_TARGET_LINUX) || defined(NUCLEUS_TARGET_OSX)
    success = munmap(m_base, 0x100000000ULL);
#endif
    if (!success) {
        logger.error(LOG_MEMORY, "Could not release memory");
    }
}

U32 Memory::alloc(U32 size, U32 align) {
    return m_segments[SEG_USER_MEMORY].alloc(size, align);
}

void Memory::free(U32 addr) {
    m_segments[SEG_USER_MEMORY].free(addr);
}

bool Memory::check(U32 addr) {
    return true;
}

/**
 * Read memory reversing endianness if necessary
 */
U08 Memory::read8(U32 addr) {
    return *(U08*)((U64)m_base + addr);
}
U16 Memory::read16(U32 addr) {
    return SE16(*(U16*)((U64)m_base + addr));
}
U32 Memory::read32(U32 addr) {
    return SE32(*(U32*)((U64)m_base + addr));
}
U64 Memory::read64(U32 addr) {
    return SE64(*(U64*)((U64)m_base + addr));
}
U128 Memory::read128(U32 addr) {
    return SE128(*(U128*)((U64)m_base + addr));
}
void Memory::readLeft(U08* dst, U32 src, U32 size) {
    for (U32 i = 0; i < size; i++) {
        dst[size - 1 - i] = read8(src + i);
    }
}
void Memory::readRight(U08* dst, U32 src, U32 size) {
    for (U32 i = 0; i < size; i++) {
        dst[i] = read8(src + (size - 1 - i));
    }
}

/**
 * Write memory reversing endianness if necessary
 */
void Memory::write8(U32 addr, U08 value) {
    *(U08*)((U64)m_base + addr) = value;
}
void Memory::write16(U32 addr, U16 value) {
    *(U16*)((U64)m_base + addr) = SE16(value);
}
void Memory::write32(U32 addr, U32 value) {
    *(U32*)((U64)m_base + addr) = SE32(value);
}
void Memory::write64(U32 addr, U64 value) {
    *(U64*)((U64)m_base + addr) = SE64(value);
}
void Memory::write128(U32 addr, U128 value) {
    *(U128*)((U64)m_base + addr) = SE128(value);
}
void Memory::writeLeft(U32 dst, U08* src, U32 size) {
    for (U32 i = 0; i < size; i++) {
        write8(dst + i, src[size - 1 - i]);
    }
}
void Memory::writeRight(U32 dst, U08* src, U32 size) {
    for (U32 i = 0; i < size; i++) {
        write8(dst + (size - 1 - i), src[i]);
    }
}

}  // namespace mem
