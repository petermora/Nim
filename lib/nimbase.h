/*

            Nim's Runtime Library
        (c) Copyright 2015 Andreas Rumpf

    See the file "copying.txt", included in this
    distribution, for details about the copyright.
*/

/* compiler symbols:
__BORLANDC__
_MSC_VER
__WATCOMC__
__LCC__
__GNUC__
__DMC__
__POCC__
__TINYC__
__clang__
*/


#ifndef NIMBASE_H
#define NIMBASE_H

#if defined(__GNUC__)
#  define _GNU_SOURCE 1
#endif

#if defined(__TINYC__)
/*#  define __GNUC__ 3
#  define GCC_MAJOR 4
#  define __GNUC_MINOR__ 4
#  define __GNUC_PATCHLEVEL__ 5 */
#  define __DECLSPEC_SUPPORTED 1
#endif

/* calling convention mess ----------------------------------------------- */
#if defined(__GNUC__) || defined(__LCC__) || defined(__POCC__) \
                      || defined(__TINYC__)
  /* these should support C99's inline */
  /* the test for __POCC__ has to come before the test for _MSC_VER,
     because PellesC defines _MSC_VER too. This is brain-dead. */
#  define N_INLINE(rettype, name) inline rettype name
#elif defined(__BORLANDC__) || defined(_MSC_VER)
/* Borland's compiler is really STRANGE here; note that the __fastcall
   keyword cannot be before the return type, but __inline cannot be after
   the return type, so we do not handle this mess in the code generator
   but rather here. */
#  define N_INLINE(rettype, name) __inline rettype name
#elif defined(__DMC__)
#  define N_INLINE(rettype, name) inline rettype name
#elif defined(__WATCOMC__)
#  define N_INLINE(rettype, name) __inline rettype name
#else /* others are less picky: */
#  define N_INLINE(rettype, name) rettype __inline name
#endif

#if defined(__POCC__)
#  define NIM_CONST /* PCC is really picky with const modifiers */
#  undef _MSC_VER /* Yeah, right PCC defines _MSC_VER even if it is
                     not that compatible. Well done. */
#elif defined(__cplusplus)
#  define NIM_CONST /* C++ is picky with const modifiers */
#else
#  define NIM_CONST  const
#endif

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#  define NIM_THREADVAR __declspec(thread)
#else
#  define NIM_THREADVAR __thread
#endif

/* --------------- how int64 constants should be declared: ----------- */
#if defined(__GNUC__) || defined(__LCC__) || \
    defined(__POCC__) || defined(__DMC__) || defined(_MSC_VER)
#  define IL64(x) x##LL
#else /* works only without LL */
#  define IL64(x) ((NI64)x)
#endif

/* ---------------- casting without correct aliasing rules ----------- */

#if defined(__GNUC__)
#  define NIM_CAST(type, ptr) (((union{type __x__;}*)(ptr))->__x__)
#else
#  define NIM_CAST(type, ptr) ((type)(ptr))
#endif

/* ------------------------------------------------------------------- */

#if defined(WIN32) || defined(_WIN32) /* only Windows has this mess... */
#  define N_CDECL(rettype, name) rettype __cdecl name
#  define N_STDCALL(rettype, name) rettype __stdcall name
#  define N_SYSCALL(rettype, name) rettype __syscall name
#  define N_FASTCALL(rettype, name) rettype __fastcall name
#  define N_SAFECALL(rettype, name) rettype __safecall name
/* function pointers with calling convention: */
#  define N_CDECL_PTR(rettype, name) rettype (__cdecl *name)
#  define N_STDCALL_PTR(rettype, name) rettype (__stdcall *name)
#  define N_SYSCALL_PTR(rettype, name) rettype (__syscall *name)
#  define N_FASTCALL_PTR(rettype, name) rettype (__fastcall *name)
#  define N_SAFECALL_PTR(rettype, name) rettype (__safecall *name)

#  ifdef __cplusplus
#    define N_LIB_EXPORT  extern "C" __declspec(dllexport)
#  else
#    define N_LIB_EXPORT  extern __declspec(dllexport)
#  endif
#  define N_LIB_IMPORT  extern __declspec(dllimport)
#else
#  define N_CDECL(rettype, name) rettype name
#  define N_STDCALL(rettype, name) rettype name
#  define N_SYSCALL(rettype, name) rettype name
#  define N_FASTCALL(rettype, name) rettype name
#  define N_SAFECALL(rettype, name) rettype name
/* function pointers with calling convention: */
#  define N_CDECL_PTR(rettype, name) rettype (*name)
#  define N_STDCALL_PTR(rettype, name) rettype (*name)
#  define N_SYSCALL_PTR(rettype, name) rettype (*name)
#  define N_FASTCALL_PTR(rettype, name) rettype (*name)
#  define N_SAFECALL_PTR(rettype, name) rettype (*name)

#  ifdef __cplusplus
#    define N_LIB_EXPORT  extern "C"
#  else
#    define N_LIB_EXPORT  extern
#  endif
#  define N_LIB_IMPORT  extern
#endif

#define N_NOCONV(rettype, name) rettype name
/* specify no calling convention */
#define N_NOCONV_PTR(rettype, name) rettype (*name)

#if defined(__GNUC__) || defined(__ICC__)
#  define N_NOINLINE(rettype, name) rettype __attribute__((noinline)) name
#elif defined(_MSC_VER)
#  define N_NOINLINE(rettype, name) __declspec(noinline) rettype name
#else
#  define N_NOINLINE(rettype, name) rettype name
#endif

#define N_NOINLINE_PTR(rettype, name) rettype (*name)

#if defined(__BORLANDC__) || defined(__WATCOMC__) || \
    defined(__POCC__) || defined(_MSC_VER) || defined(WIN32) || defined(_WIN32)
/* these compilers have a fastcall so use it: */
#  define N_NIMCALL(rettype, name) rettype __fastcall name
#  define N_NIMCALL_PTR(rettype, name) rettype (__fastcall *name)
#  define N_RAW_NIMCALL __fastcall
#else
#  define N_NIMCALL(rettype, name) rettype name /* no modifier */
#  define N_NIMCALL_PTR(rettype, name) rettype (*name)
#  define N_RAW_NIMCALL
#endif

#define N_CLOSURE(rettype, name) N_NIMCALL(rettype, name)
#define N_CLOSURE_PTR(rettype, name) N_NIMCALL_PTR(rettype, name)

/* ----------------------------------------------------------------------- */

#include <limits.h>
#include <stddef.h>

/* C99 compiler? */
#if (defined(__STD_VERSION__) && (__STD_VERSION__ >= 199901))
#  define HAVE_STDINT_H
#endif

#if defined(__LCC__) || defined(__DMC__) || defined(__POCC__)
#  define HAVE_STDINT_H
#endif

/* bool types (C++ has it): */
#ifdef __cplusplus
#  ifndef NIM_TRUE
#    define NIM_TRUE true
#  endif
#  ifndef NIM_FALSE
#    define NIM_FALSE false
#  endif
#  define NIM_BOOL bool
#  define NIM_NIL 0
struct NimException
{
  NimException(struct Exception* exp, const char* msg): exp(exp), msg(msg) {}

  struct Exception* exp;
  const char* msg;
};
#else
#  ifdef bool
#    define NIM_BOOL bool
#  else
  typedef unsigned char NIM_BOOL;
#  endif
#  ifndef NIM_TRUE
#    define NIM_TRUE ((NIM_BOOL) 1)
#  endif
#  ifndef NIM_FALSE
#    define NIM_FALSE ((NIM_BOOL) 0)
#  endif
#  define NIM_NIL ((void*)0) /* C's NULL is fucked up in some C compilers, so
                              the generated code does not rely on it anymore */
#endif

#if defined(__BORLANDC__) || defined(__DMC__) \
   || defined(__WATCOMC__) || defined(_MSC_VER)
typedef signed char NI8;
typedef signed short int NI16;
typedef signed int NI32;
/* XXX: Float128? */
typedef unsigned char NU8;
typedef unsigned short int NU16;
typedef unsigned __int64 NU64;
typedef __int64 NI64;
typedef unsigned int NU32;
#elif defined(HAVE_STDINT_H)
#  include <stdint.h>
typedef int8_t NI8;
typedef int16_t NI16;
typedef int32_t NI32;
typedef int64_t NI64;
typedef uint64_t NU64;
typedef uint8_t NU8;
typedef uint16_t NU16;
typedef uint32_t NU32;
#else
typedef signed char NI8;
typedef signed short int NI16;
typedef signed int NI32;
/* XXX: Float128? */
typedef unsigned char NU8;
typedef unsigned short int NU16;
typedef unsigned long long int NU64;
typedef long long int NI64;
typedef unsigned int NU32;
#endif

#ifdef NIM_INTBITS
#  if NIM_INTBITS == 64
typedef NI64 NI;
typedef NU64 NU;
#  elif NIM_INTBITS == 32
typedef NI32 NI;
typedef NU32 NU;
#  elif NIM_INTBITS == 16
typedef NI16 NI;
typedef NU16 NU;
#  elif NIM_INTBITS == 8
typedef NI8 NI;
typedef NU8 NU;
#  else
#    error "invalid bit width for int"
#  endif
#endif

extern NI nim_program_result;

typedef float NF32;
typedef double NF64;
typedef double NF;

typedef char NIM_CHAR;
typedef char* NCSTRING;

#ifdef NIM_BIG_ENDIAN
#  define NIM_IMAN 1
#else
#  define NIM_IMAN 0
#endif

static N_INLINE(NI, float64ToInt32)(double x) {
  /* nowadays no hack necessary anymore */
  return x >= 0 ? (NI)(x+0.5) : (NI)(x-0.5);
}

static N_INLINE(NI32, float32ToInt32)(float x) {
  /* nowadays no hack necessary anymore */
  return x >= 0 ? (NI32)(x+0.5) : (NI32)(x-0.5);
}

#define float64ToInt64(x) ((NI64) (x))

#define zeroMem(a, size) memset(a, 0, size)
#define equalMem(a, b, size) (memcmp(a, b, size) == 0)

#define STRING_LITERAL(name, str, length) \
  static const struct {                   \
    TGenericSeq Sup;                      \
    NIM_CHAR data[(length) + 1];          \
  } name = {{length, length}, str}

typedef struct TStringDesc* string;

/* declared size of a sequence/variable length array: */
#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
#  define SEQ_DECL_SIZE /* empty is correct! */
#else
#  define SEQ_DECL_SIZE 1000000
#endif

#define ALLOC_0(size)  calloc(1, size)
#define DL_ALLOC_0(size) dlcalloc(1, size)

#define GenericSeqSize sizeof(TGenericSeq)
#define paramCount() cmdCount

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__i386__)
#  ifndef NAN
static unsigned long nimNaN[2]={0xffffffff, 0x7fffffff};
#    define NAN (*(double*) nimNaN)
#  endif
#endif

#ifndef NAN
#  define NAN (0.0 / 0.0)
#endif

#ifndef INF
#  ifdef INFINITY
#    define INF INFINITY
#  elif defined(HUGE_VAL)
#    define INF  HUGE_VAL
#  elif defined(_MSC_VER)
#    include <float.h>
#    define INF (DBL_MAX+DBL_MAX)
#  else
#    define INF (1.0 / 0.0)
#  endif
#endif

typedef struct TFrame TFrame;
struct TFrame {
  TFrame* prev;
  NCSTRING procname;
  NI line;
  NCSTRING filename;
  NI16 len;
  NI16 calldepth;
};

#define nimfr(proc, file) \
  TFrame FR; \
  FR.procname = proc; FR.filename = file; FR.line = 0; FR.len = 0; nimFrame(&FR);

#define nimfrs(proc, file, slots, length) \
  struct {TFrame* prev;NCSTRING procname;NI line;NCSTRING filename; NI len; VarSlot s[slots];} FR; \
  FR.procname = proc; FR.filename = file; FR.line = 0; FR.len = length; nimFrame((TFrame*)&FR);

#define nimln(n, file) \
  FR.line = n; FR.filename = file;

#define NIM_POSIX_INIT  __attribute__((constructor))

#if defined(_MSCVER) && defined(__i386__)
__declspec(naked) int __fastcall NimXadd(volatile int* pNum, int val) {
  __asm {
    lock xadd dword ptr [ECX], EDX
    mov EAX, EDX
    ret
  }
}
#endif

#ifdef __GNUC__
#  define likely(x) __builtin_expect(x, 1)
#  define unlikely(x) __builtin_expect(x, 0)
/* We need the following for the posix wrapper. In particular it will give us
   POSIX_SPAWN_USEVFORK: */
#  ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#  endif
#else
#  define likely(x) (x)
#  define unlikely(x) (x)
#endif

#if 0 // defined(__GNUC__) || defined(__clang__)
// not needed anymore because the stack marking cares about
// interior pointers now
static inline void GCGuard (void *ptr) { asm volatile ("" :: "X" (ptr)); }
#  define GC_GUARD __attribute__ ((cleanup(GCGuard)))
#else
#  define GC_GUARD
#endif

/* Test to see if Nim and the C compiler agree on the size of a pointer.
   On disagreement, your C compiler will say something like:
   "error: 'assert_numbits' declared as an array with a negative size" */
typedef int assert_numbits[sizeof(NI) == sizeof(void*) && NIM_INTBITS == sizeof(NI)*8 ? 1 : -1];
#endif

#ifdef  __cplusplus
#  define NIM_EXTERNC extern "C"
#else
#  define NIM_EXTERNC
#endif

/* we have to tinker with TNimType as it's both part of system.nim and
   typeinfo.nim but system.nim doesn't export it cleanly... */
typedef struct TNimType TNimType;

/* ---------------- platform specific includes ----------------------- */

/* VxWorks related includes */
#if defined(__VXWORKS__)
#  include <sys/types.h>
#  include <types/vxWind.h>
#  include <tool/gnu/toolMacros.h>
#elif defined(__FreeBSD__)
#  include <sys/types.h>
#endif
