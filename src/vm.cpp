#include <nstd/core/mem.h>
#include <nstd/core/slice.h>
#include <nstd/ext/vm.h>

/// An enumeration of each operation code.
enum class Opcode : NSTDUInt16
{
    /// No operation.
    NOP,
    /// Exit operation.
    EXIT,
    /// Jump operation.
    JUMP,
    /// Move operation.
    MOVE,
    /// 8-bit move operation.
    MOVE8,
    /// 16-bit move operation.
    MOVE16,
    /// 32-bit move operation.
    MOVE32,
    /// 64-bit move operation.
    MOVE64
};

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

/// Returns a pointer to the value at location `pos` in the virtual machine's memory.
template <typename T>
static inline T *get(NSTDEXTVM *const vm, const NSTDUInt16 pos)
{
    return (T *)(vm->mem + pos);
}

/// Executes the move operation.
template <typename T>
static inline void move(NSTDEXTVM *const vm, Cursor &cursor)
{
    const NSTDUInt16 dest{cursor.next<NSTDUInt16>()};
    const NSTDUInt16 src{cursor.next<NSTDUInt16>()};
    *get<T>(vm, dest) = *get<T>(vm, src);
}

/// Creates a new instance of `NSTDEXTVM`.
///
/// # Returns
///
/// `NSTDEXTVM vm` - The new virtual machine.
NSTDAPI extern "C" inline NSTDEXTVM nstd_ext_vm_new()
{
    return NSTDEXTVM{};
}

/// Loads a byte slice of instructions into the virtual machine's memory.
///
/// This will have no affect in the following situations:
///
/// - `program`'s stride is not 1.
///
/// - `program`'s length is greater than `NSTD_EXT_VM_RAM`.
///
/// # Parameters:
///
/// - `NSTDEXTVM *vm` - The virtual machine.
///
/// - `const NSTDSlice *program` - The program to be run on the virtual machine.
///
/// # Safety
///
/// This operation may cause undefined behavior in the event that `program`'s data is invalid.
NSTDAPI extern "C" void nstd_ext_vm_load(NSTDEXTVM *const vm, const NSTDSlice *const program)
{
    if (vm)
    {
        if (program)
        {
            const NSTDUInt len{nstd_core_slice_len(program)};
            if (len <= NSTD_EXT_VM_RAM && nstd_core_slice_stride(program) == 1)
            {
                const NSTDAny pptr{nstd_core_slice_as_ptr(program)};
                nstd_core_mem_copy(vm->mem, static_cast<const NSTDByte *>(pptr), len);
                vm->program_size = len;
            }
        }
        else
            vm->program_size = 0;
    }
}

/// Executes the currently loaded program in a virtual machine.
///
/// # Parameters:
///
/// - `NSTDEXTVM *vm` - The virtual machine.
NSTDAPI extern "C" void nstd_ext_vm_run(NSTDEXTVM *const vm)
{
    if (!vm)
        return;
    Cursor cursor{vm};
    while (!cursor.finished())
    {
        // Read the next opcode.
        const Opcode opcode{cursor.next<Opcode>()};
        // Execute the next instruction.
        switch (opcode)
        {
        // No operation.
        case Opcode::NOP:
            break;
        // Exit operation.
        case Opcode::EXIT:
            return;
        // Jump operation.
        case Opcode::JUMP:
        {
            const NSTDUInt16 dest{cursor.next<NSTDUInt16>()};
            cursor.jump(vm, dest);
            break;
        }
        // Move operation.
        case Opcode::MOVE:
        {
            const NSTDUInt16 dest{cursor.next<NSTDUInt16>()};
            const NSTDUInt16 src{cursor.next<NSTDUInt16>()};
            const NSTDUInt16 num{cursor.next<NSTDUInt16>()};
            nstd_core_mem_copy(vm->mem + dest, vm->mem + src, num);
            break;
        }
        // 8-bit move operation.
        case Opcode::MOVE8:
        {
            const NSTDUInt16 dest{cursor.next<NSTDUInt16>()};
            const NSTDUInt16 src{cursor.next<NSTDUInt16>()};
            vm->mem[dest] = vm->mem[src];
            break;
        }
        // 16-bit move operation.
        case Opcode::MOVE16:
            move<NSTDUInt16>(vm, cursor);
            break;
        // 32-bit move operation.
        case Opcode::MOVE32:
            move<NSTDUInt32>(vm, cursor);
            break;
        // 64-bit move operation.
        case Opcode::MOVE64:
            move<NSTDUInt64>(vm, cursor);
            break;
        // Incorrect operation code.
        default:
            throw;
        }
    }
}
