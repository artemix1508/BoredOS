// Copyright (c) 2023-2026 Christiaan (chris@boreddev.nl)
// GPL v3.0 — BoredOS mlibc sysdep tag declarations (sysdeps.hpp)
//
// Declares which mlibc sysdep "tags" the BoredOS backend implements.
// mlibc uses these to select which standard library functions are available.
#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct BoredOSSysdepTags :
    // --- Mandatory sysdeps ---
    LibcPanic,
    LibcLog,
    Exit,
    FutexWait,
    FutexWake,
    TcbSet,
    AnonAllocate,
    AnonFree,
    VmMap,
    VmUnmap,
    ClockGet,
    Sleep,
    // --- File / IO ---
    Open,
    Read,
    Write,
    Writev,
    Seek,
    Close,
    Isatty,
    // --- POSIX ---
    Dup,
    Dup2,
    Pipe,
    Fcntl,
    Ioctl,
    GetCwd,
    Chdir,
    Mkdir,
    Rmdir,
    Unlinkat,
    Stat,
    Access,
    Tcgetattr,
    Tcsetattr,
    Tcgetwinsize,
    Tcsetwinsize,
    Pselect,
    Poll,
    // --- Sockets ---
    Socket,
    Bind,
    Listen,
    Connect,
    Accept,
    Sendto,
    Recvfrom,
    GetSockopt,
    SetSockopt,
    Sockname,
    // --- Process ---
    Fork,
    Execve,
    Waitpid,
    Kill,
    Sigaction,
    Sigprocmask,
    GetPid,
    GetPpid,
    GetUid,
    GetEuid,
    GetGid,
    GetEgid,
    GetPgid,
    SetPgid,
    Uname,
    Umask,
    Chmod,
    // --- Directory ---
    OpenDir,
    ReadEntries,
    // --- Hostname ---
    GetHostname
{};

template<typename Tag>
using Sysdeps = SysdepOf<BoredOSSysdepTags, Tag>;

} // namespace mlibc
