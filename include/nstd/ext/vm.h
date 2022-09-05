#ifndef NSTD_EXT_VM_H
#define NSTD_EXT_VM_H
#include "../core/def.h"
#include "../core/slice.h"
#include "../nstd.h"

/// The default number of bytes an `NSTDVM` has for random access memory.
#define NSTD_EXT_VM_RAM 1024 * 64

/// A little virtual machine to run alongside your app.
typedef struct {
    /// The size in bytes of the currently loaded program.
    NSTDUInt program_size;
    /// The virtual machine's memory.
    NSTDByte mem[NSTD_EXT_VM_RAM];
} NSTDVM;

/// Creates a new instance of `NSTDVM`.
///
/// # Returns
///
/// `NSTDVM vm` - The new virtual machine.
NSTDAPI NSTDVM nstd_ext_vm_new();

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
NSTDAPI void nstd_ext_vm_load(NSTDVM *vm, const NSTDSlice *program);

/// Executes the currently loaded program in a virtual machine.
///
/// # Parameters:
///
/// - `NSTDVM *vm` - The virtual machine.
NSTDAPI void nstd_ext_vm_run(NSTDVM *vm);

#endif
