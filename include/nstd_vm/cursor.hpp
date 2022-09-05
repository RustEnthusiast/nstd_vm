#ifndef NSTD_VM_CURSOR_HPP
#define NSTD_VM_CURSOR_HPP
#include <nstd/core/def.h>
#include <nstd/ext/vm.h>
#include <nstd/nstd.h>

/// Wraps the currently loaded program's in-vm-memory buffer.
class Cursor
{
    /// The current position in the program.
    const NSTDByte *pos;
    /// A pointer to (one byte past) the end of the program.
    const NSTDByte *end;

public:
    /// Wraps the currently loaded program.
    Cursor(const NSTDVM *const vm) : pos{vm->mem}, end{pos + vm->program_size} {}

    /// Returns the value at the current position in the program and advances the cursor.
    template <typename T>
    inline T next()
    {
        const T *const ptr{(const T *)pos};
        pos += sizeof(T);
        if (pos > end)
            throw;
        return *ptr;
    }

    /// Sets the cursor's position.
    inline void jump(const NSTDVM *const vm, const NSTDVMUInt new_pos)
    {
        if (new_pos > vm->program_size)
            throw;
        pos = vm->mem + new_pos;
    }

    /// Returns `true` if the program has finished.
    inline bool finished() const
    {
        return pos >= end;
    }
};

#endif
