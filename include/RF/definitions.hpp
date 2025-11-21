#pragma once

#if defined(__OpenBSD__)     \
 || defined(__FreeBSD__)     \
 || defined(__NetBSD__)      \
 || defined(__TrueOS__)      \
 || defined(__GhostBSD__)    \
 || defined(__DragonFly__)   \
 || defined(__MidnightBSD__) \
 || defined(__MACH__)        \
 || defined(__APPLE__)
#define __BSD__
#endif

#if defined (__MACH__) && defined (__APPLE__)
#define __DARWIN__
#endif

#if !defined (__MACH__) && defined (__BSD__)
#define __BSD_KERNEL__
#endif

#if defined(__linux__)
#define __LINUX__
#endif

#if defined(__LINUX__) || defined (__BSD__)
#define __UNIX_LIKE__
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#define __WINDOWS__
#endif

#if defined(__clang__)
#define __COMPILER_CLANG__
#elif defined(__GNUC__) || defined(__GNUG__)
#define __COMPILER_GCC__
#elif defined(_MSC_VER)
#define __COMPILER_MSVC__
#elif defined(__INTEL_COMPILER)
#define __COMPILER_INTEL__
#elif defined(__MINGW32__) || defined(__MINGW64__)
#define __COMPILER_MINGW__
#elif defined(__EMSCRIPTEN__)
#define __COMPILER_EMSCRIPTEN__
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define __ARCH_X86_64__
#elif defined(__i386__) || defined(_M_IX86)
#define __ARCH_X86__
#elif defined(__aarch64__) || defined(_M_ARM64)
#define __ARCH_ARM64__
#elif defined(__arm__) || defined(_M_ARM)
#define __ARCH_ARM__
#elif defined(__powerpc64__) || defined(__ppc64__)
#define __ARCH_PPC64__
#elif defined(__powerpc__) || defined(__ppc__)
#define __ARCH_PPC__
#elif defined(__riscv) || defined(__riscv__)
#define __ARCH_RISCV__
#endif

#if defined(_LIBCPP_VERSION)
#define __STDLIB_LIBCXX__  // LLVM's libc++
#elif defined(__GLIBCXX__)
#define __STDLIB_GLIBCXX__ // GNU libstdc++
#elif defined(_MSC_VER)
#define __STDLIB_MSVCXX__  // MSVC STL
#endif

#include <cstdint>

namespace RF::def
{
	struct version_info
	{
		std::size_t major;
		std::size_t minor;
		std::size_t patch;
	};

	RF::def::version_info RF_version();
} // namespace RF::def
