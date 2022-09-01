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
    Cursor(const NSTDEXTVM *const vm) : pos{vm->mem}, end{pos + vm->program_size} {}

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
    inline void jump(const NSTDEXTVM *const vm, const NSTDUInt16 new_pos)
    {
        if (new_pos < vm->program_size)
            pos = vm->mem + new_pos;
        else
            throw;
    }

    /// Returns `true` if the program has finished.
    inline bool finished() const
    {
        return pos >= end;
    }
};

#endif
