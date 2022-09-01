#include <nstd_vm/cursor.hpp>
#include <nstd_vm/opcode.hpp>
#include <nstd/core/mem.h>
#include <nstd/core/slice.h>
#include <nstd/ext/vm.h>

/// Returns a pointer to the value at location `pos` in the virtual machine's memory.
template <typename T>
static inline T *get(NSTDVM *const vm, const NSTDUInt16 pos)
{
    return (T *)(vm->mem + pos);
}

/// Executes the move operation.
template <typename T>
static inline void move(NSTDVM *const vm, Cursor &cursor)
{
    const NSTDUInt16 dest{cursor.next<NSTDUInt16>()};
    const NSTDUInt16 src{cursor.next<NSTDUInt16>()};
    *get<T>(vm, dest) = *get<T>(vm, src);
}

/// Creates a new instance of `NSTDVM`.
///
/// # Returns
///
/// `NSTDVM vm` - The new virtual machine.
NSTDAPI extern "C" inline NSTDVM nstd_ext_vm_new()
{
    return NSTDVM{};
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
/// - `NSTDVM *vm` - The virtual machine.
///
/// - `const NSTDSlice *program` - The program to be run on the virtual machine.
///
/// # Safety
///
/// This operation may cause undefined behavior in the event that `program`'s data is invalid.
NSTDAPI extern "C" void nstd_ext_vm_load(NSTDVM *const vm, const NSTDSlice *const program)
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
/// - `NSTDVM *vm` - The virtual machine.
NSTDAPI extern "C" void nstd_ext_vm_run(NSTDVM *const vm)
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
