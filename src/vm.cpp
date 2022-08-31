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
    /// Move operation.
    MOVE
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

    /// Returns `true` if the program has finished.
    inline bool finished() const
    {
        return pos >= end;
    }
};

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
    const NSTDUInt len{nstd_core_slice_len(program)};
    if (len <= NSTD_EXT_VM_RAM && nstd_core_slice_stride(program) == 1)
    {
        const NSTDByte *const pptr{static_cast<const NSTDByte *>(nstd_core_slice_as_ptr(program))};
        nstd_core_mem_copy(vm->mem, pptr, len);
        vm->program_size = len;
    }
}

/// Executes the currently loaded program in a virtual machine.
///
/// # Parameters:
///
/// - `NSTDEXTVM *vm` - The virtual machine.
NSTDAPI extern "C" void nstd_ext_vm_run(NSTDEXTVM *vm)
{
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
        // Move operation.
        case Opcode::MOVE:
        {
            const NSTDUInt16 dest{cursor.next<NSTDUInt16>()};
            const NSTDUInt16 src{cursor.next<NSTDUInt16>()};
            const NSTDUInt16 num{cursor.next<NSTDUInt16>()};
            nstd_core_mem_copy(vm->mem + dest, vm->mem + src, num);
            break;
        }
        // Incorrect operation code.
        default:
            throw;
        }
    }
}
