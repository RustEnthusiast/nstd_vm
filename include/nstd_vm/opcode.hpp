#ifndef NSTD_VM_OPCODE_HPP
#define NSTD_VM_OPCODE_HPP
#include <nstd/nstd.h>

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

#endif
