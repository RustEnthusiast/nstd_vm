#include <nstd/core/mem.h>
#include <nstd/core/slice.h>
#include <nstd/ext/vm.h>

/// No operation.
static constexpr const NSTDUInt16 OP_NOP{0x0000};

/// Reads the next opcode/value from the virtual machine's currently loaded program and advances
/// the cursor.
template <typename T>
static inline T vm_read(
    const NSTDEXTVM *const vm,
    const NSTDByte **const cursor,
    const NSTDByte *const end)
{
    const T *const ptr{(const T *)*cursor};
    *cursor += sizeof(T);
    if (*cursor > end)
        throw;
    return *ptr;
}

/// Creates a new instance of `NSTDEXTVM`.
///
/// # Returns
///
/// `NSTDEXTVM vm` - The new virtual machine.
NSTDAPI inline NSTDEXTVM nstd_ext_vm_new()
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
NSTDAPI void nstd_ext_vm_load(NSTDEXTVM *const vm, const NSTDSlice *const program)
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
/// - `const NSTDEXTVM *vm` - The virtual machine.
NSTDAPI void nstd_ext_vm_run(const NSTDEXTVM *vm)
{
    const NSTDByte *const end{vm->mem + vm->program_size};
    for (const NSTDByte *cursor{vm->mem}; cursor < end;)
    {
        // Read the next opcode.
        const NSTDUInt16 opcode{vm_read<NSTDUInt16>(vm, &cursor, end)};
        // Execute the next instruction.
        switch (opcode)
        {
        // No operation.
        case OP_NOP:
            break;
        // Incorrect operation code.
        default:
            throw;
        }
    }
}
