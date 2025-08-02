#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>
#include <new>
#include <stdexcept>

class CRArena {
    std::vector<std::byte> _buf;
    size_t                 _off = 0;

public:
    explicit CRArena(size_t capacity_bytes)
      : _buf(capacity_bytes)
    {}

    void* allocate(size_t size,
                   size_t align = alignof(std::max_align_t)) {
        uintptr_t ptr = reinterpret_cast<uintptr_t>(_buf.data() + _off);
        size_t pad   = (align - (ptr % align)) % align;
        if (_off + pad + size > _buf.size())
            throw std::bad_alloc();
        _off += pad;
        void* mem = _buf.data() + _off;
        _off += size;
        return mem;
    }

    void reset() { _off = 0; }
};
