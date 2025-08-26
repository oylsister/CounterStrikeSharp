using System;
using System.Runtime.CompilerServices;
using CounterStrikeSharp.API.Modules.Memory;

namespace CounterStrikeSharp.API.Modules.Utils;

public partial class matrix3x4_t : DisposableMemory
{
    public matrix3x4_t(IntPtr pointer) : base(pointer)
    {
    }

    public unsafe ref float this[int row, int column] => ref Unsafe.Add(ref *(float*)Handle, row * 4 + column);
}