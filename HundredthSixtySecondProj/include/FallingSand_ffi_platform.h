/* This contains assorted platform constants, defined in one place.
 *
 * The primary preference is config.h, generated by the buildsystem. There
 * are also fallbacks in place using different approaches, in order to
 * decouple the code from build system config as much as possible; the
 * goal is to make it buildable without any pre-configuration
 */

#ifndef PLATFORM_HH
#define PLATFORM_HH

/* OS; defined to be luajit compatible
 *
 * If undetected, it will still work, but the OS will be "Other"
 */

#include "FallingSand_core.hpp"

#define FFI_OS_OTHER 0
#define FFI_OS_WINDOWS 1
#define FFI_OS_LINUX 2
#define FFI_OS_OSX 3
#define FFI_OS_BSD 4
#define FFI_OS_POSIX 5

#if defined(_WIN32) && !defined(_XBOX_VER)
#define FFI_OS FFI_OS_WINDOWS
#define FFI_OS_NAME "Windows"
#elif defined(__linux__)
#define FFI_OS FFI_OS_LINUX
#define FFI_OS_NAME "Linux"
#elif defined(__MACH__) && defined(__APPLE__)
#define FFI_OS FFI_OS_OSX
#define FFI_OS_NAME "OSX"
#elif (defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)) && !defined(__ORBIS__)
#define FFI_OS FFI_OS_BSD
#define FFI_OS_NAME "BSD"
#elif (defined(__sun__) && defined(__svr4__)) || defined(__HAIKU__) || defined(__CYGWIN__)
#define FFI_OS FFI_OS_POSIX
#define FFI_OS_NAME "POSIX"
#ifdef __CYGWIN__
#define FFI_OS_CYGWIN 1
#endif
#else
#define FFI_OS FFI_OS_OTHER
#define FFI_OS_NAME "Other"
#endif

#if !defined(FFI_BIG_ENDIAN) && !defined(ME_LITTLE_ENDIAN)
#error "Unknown machine endianness"
#elif defined(FFI_BIG_ENDIAN) && defined(ME_LITTLE_ENDIAN)
#error "Choose just one endianness"
#endif

/* Arch; defined to be luajit compatible
 *
 * IF undetected, it will still work, but the arch will be "unknown"
 */

#define FFI_ARCH_UNKNOWN 0
#define FFI_ARCH_X86 1
#define FFI_ARCH_X64 2
#define FFI_ARCH_ARM 3
#define FFI_ARCH_ARM64 4
#define FFI_ARCH_PPC 5
#define FFI_ARCH_PPC64 6
#define FFI_ARCH_MIPS32 7
#define FFI_ARCH_MIPS64 8
/* these architectures are not defined in luajit */
#define FFI_ARCH_ALPHA 9
#define FFI_ARCH_HPPA 10
#define FFI_ARCH_IA64 11
#define FFI_ARCH_M68K 12
#define FFI_ARCH_MBLAZE 13
#define FFI_ARCH_OR1K 14
#define FFI_ARCH_RV32 15
#define FFI_ARCH_RV64 16
#define FFI_ARCH_SH4 17
#define FFI_ARCH_SPARC 18
#define FFI_ARCH_SPARC64 19
#define FFI_ARCH_S390 20

#define FFI_CPU(arch) (FFI_ARCH == FFI_ARCH_##arch)

#if defined(__i386) || defined(__i386__) || defined(_M_IX86)
#define FFI_ARCH FFI_ARCH_X86
#define FFI_ARCH_NAME "x86"
#elif defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#define FFI_ARCH FFI_ARCH_X64
#define FFI_ARCH_NAME "x64"
#elif defined(__arm__) || defined(__arm) || defined(__ARM__) || defined(__ARM)
#define FFI_ARCH FFI_ARCH_ARM
#if defined(FFI_BIG_ENDIAN)
#define FFI_ARCH_NAME "armeb"
#else
#define FFI_ARCH_NAME "arm"
#endif
#ifdef __SOFTFP__
#define FFI_ARCH_HAS_FPU 0
#endif
#if !__ARM_PCS_VFP
#define FFI_ARCH_SOFTFP 1
#endif
#elif defined(__aarch64__)
#define FFI_ARCH FFI_ARCH_ARM64
#if defined(FFI_BIG_ENDIAN)
#define FFI_ARCH_NAME "arm64be"
#else
#define FFI_ARCH_NAME "arm64"
#endif
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(__powerpc64) || defined(__ppc64) || defined(__PPC64) || defined(__POWERPC64__) || defined(__POWERPC64) || \
        defined(_M_PPC64)
#define FFI_ARCH FFI_ARCH_PPC64
#if defined(_CALL_ELF) && (_CALL_ELF == 2)
#define FFI_ARCH_PPC64_ELFV2 1
#endif
#if defined(ME_LITTLE_ENDIAN)
#define FFI_ARCH_NAME "ppc64le"
#else
#define FFI_ARCH_NAME "ppc64"
#endif
#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__) || defined(__powerpc) || defined(__ppc) || defined(__PPC) || defined(__POWERPC__) || defined(__POWERPC) || defined(_M_PPC)
#define FFI_ARCH FFI_ARCH_PPC
#if defined(ME_LITTLE_ENDIAN)
#define FFI_ARCH_NAME "ppcle"
#else
#define FFI_ARCH_NAME "ppc"
#endif
#elif defined(__mips64__) || defined(__mips64) || defined(__MIPS64__) || defined(__MIPS64)
#define FFI_ARCH FFI_ARCH_MIPS64
#if __mips_isa_rev >= 6
#if defined(ME_LITTLE_ENDIAN)
#define FFI_ARCH_NAME "mips64r6el"
#else
#define FFI_ARCH_NAME "mips64r6"
#endif
#else
#if defined(ME_LITTLE_ENDIAN)
#define FFI_ARCH_NAME "mips64el"
#else
#define FFI_ARCH_NAME "mips64"
#endif
#endif
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__) || defined(__MIPS)
#define FFI_ARCH FFI_ARCH_MIPS32
#if __mips_isa_rev >= 6
#if defined(ME_LITTLE_ENDIAN)
#define FFI_ARCH_NAME "mips32r6el"
#else
#define FFI_ARCH_NAME "mips32r6"
#endif
#else
#if defined(ME_LITTLE_ENDIAN)
#define FFI_ARCH_NAME "mipsel"
#else
#define FFI_ARCH_NAME "mips"
#endif
#endif
#ifdef __mips_soft_float
#define FFI_ARCH_SOFTFP 1
#define FFI_ARCH_HAS_FPU 0
#else
#define FFI_ARCH_SOFTFP 0
#define FFI_ARCH_HAS_FPU 1
#endif
#elif defined(__alpha__) || defined(__alpha)
#define FFI_ARCH FFI_ARCH_ALPHA
#define FFI_ARCH_NAME "alpha"
#elif defined(__hppa__) || defined(__HPPA__) || defined(__hppa)
#define FFI_ARCH FFI_ARCH_HPPA
#define FFI_ARCH_NAME "hppa"
#elif defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(_M_IA64) || defined(__itanium__)
#define FFI_ARCH FFI_ARCH_IA64
#define FFI_ARCH_NAME "ia64"
#elif defined(__m68k__) || defined(__MC68K__)
#define FFI_ARCH FFI_ARCH_M68K
#define FFI_ARCH_NAME "m68k"
#elif defined(__MICROBLAZE__)
#define FFI_ARCH FFI_ARCH_MBLAZE
#if defined(__MICROBLAZEEL__)
#define FFI_ARCH_NAME "microblazeel"
#else
#define FFI_ARCH_NAME "microblaze"
#endif
#elif defined(__OR1K__)
#define FFI_ARCH FFI_ARCH_OR1K
#define FFO_ARCH_NAME "or1k"
#elif defined(__riscv) || defined(__riscv__)
#if __riscv_xlen == 32
#define FFI_ARCH FFI_ARCH_RV32
#define FFI_ARCH_NAME "riscv32"
#else
#define FFI_ARCH FFI_ARCH_RV64
#define FFI_ARCH_NAME "riscv64"
#endif
#ifdef __riscv_float_abi_soft
#define FFI_ARCH_SOFTFP 1
#define FFI_ARCH_HAS_FPU 0
#else
#define FFI_ARCH_SOFTFP 0
#define FFI_ARCH_HAS_FPU 1
#endif
#elif defined(__sh__) && defined(__SH4__)
#define FFI_ARCH FFI_ARCH_SH4
#if defined(FFI_BIG_ENDIAN)
#define FFI_ARCH_NAME "sh4eb"
#else
#define FFI_ARCH_NAME "sh4"
#endif
#elif defined(__sparc__) || defined(__sparc)
#if defined(__sparc_v9__) || defined(__sparcv9) || defined(__arch64__)
#define FFI_ARCH FFI_ARCH_SPARC64
#define FFI_ARCH_NAME "sparc64"
#else
#define FFI_ARCH FFI_ARCH_SPARC
#define FFI_ARCH_NAME "sparc"
#endif
#elif defined(__s390__) || defined(__s390x__) || defined(__zarch__)
#define FFI_ARCH FFI_ARCH_S390
#if defined(__s390x__)
#define FFI_ARCH_NAME "s390x"
#else
#define FFI_ARCH_NAME "s390"
#endif
#else
#define FFI_ARCH FFI_ARCH_UNKNOWN
#define FFI_ARCH_NAME "unknown"
#endif

#if FFI_ARCH == FFI_ARCH_ARM && defined(__ARM_EABI__)
#define FFI_ARM_EABI 1
#endif

#if FFI_ARCH == FFI_ARCH_PPC && defined(__NO_FPRS__) && !defined(_SOFT_FLOAT)
#define FFI_PPC_SPE 1
#endif

#ifndef FFI_ARCH_HAS_FPU
#if defined(_SOFT_FLOAT) || defined(_SOFT_DOUBLE)
#define FFI_ARCH_HAS_FPU 0
#else
#define FFI_ARCH_HAS_FPU 1
#endif
#endif

#ifndef FFI_ARCH_SOFTFP
#if defined(_SOFT_FLOAT) || defined(_SOFT_DOUBLE)
#define FFI_ARCH_SOFTFP 1
#else
#define FFI_ARCH_SOFTFP 0
#endif
#endif

#if FFI_OS == FFI_OS_WINDOWS || defined(FFI_OS_CYGWIN)
#define FFI_WINDOWS_ABI 1
#endif

#ifdef _UWP
#define FFI_WINDOWS_UWP 1
#error "UWP support not yet implemented"
#endif

#if FFI_OS != FFI_OS_WINDOWS
#define FFI_USE_DLFCN 1
#endif

/* abi-specific features */

/* passing unions by value:
 *
 * - all 32-bit x86 except windows fastcall passes values on the stack
 * - windows fastcall may pass some in regs but always the same ones
 * - windows x64 ABI doesn't care about union contents for passing
 * - arm and aarch64 - composite types go in GPRs or on the stack
 * - ppc and ppc64 - composite types go in GPRs or on the stack
 * - mips - unions are like structs, structs go in GPRs or on the stack
 *
 * every other ABI is for now forbidden from passing unions by value since
 * it is not known whether it is safe to do so; usually this would need some
 * manual handling as the type of register used for passing may depend on the
 * type being passed (i.e. same-size same-alignment unions with different
 * fields may use different registers)
 *
 * aarch64 and ppc64le have a concept of homogenous aggregates, which means
 * unions may occasionally go in FPRs/VRRs; this is handled explicitly in
 * our implementation
 *
 * ppc64 and aarch64 currently disabled - buggy
 */
#if FFI_CPU(X86) || FFI_CPU(ARM) || FFI_CPU(PPC) || FFI_CPU(MIPS32) || FFI_CPU(MIPS64)
#define FFI_ABI_UNIONVAL 1
#elif defined(FFI_WINDOWS_ABI) && (FFI_ARCH == FFI_ARCH_X64)
#define FFI_ABI_UNIONVAL 1
#endif

/* some compiler bits */

#if defined(__GNUC__)
#if (__GNUC__ >= 5) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))
#define FFI_DIAGNOSTIC_PRAGMA_GCC 1
#endif
#define WARN_UNUSED_RET __attribute__((warn_unused_result))
#define RESTRICT __restrict__
#else
#define WARN_UNUSED_RET
#define RESTRICT
#endif

#if defined(__clang__)
#define FFI_DIAGNOSTIC_PRAGMA_CLANG 1
#endif

/* MSVC warnings we don't care about/are irrelevant to us */

#ifdef _MSC_VER
/* conditional expression is constant */
#pragma warning(disable : 4127)
/* unsafe CRT, used only once */
#pragma warning(disable : 4996)
#endif

/* MSVC and clang */

#define _CRT_SECURE_NO_WARNINGS 1

#endif /* PLATFORM_HH */
