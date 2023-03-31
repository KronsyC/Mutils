/**
 * funcmod.h
 * 
 * A file consisting of a few simple macros used for adding common attributes to
 * functions, such as being exported or inlined
*/
#pragma once


#define INLINE __attribute__((always_inline))


#ifdef WIN32

#define EXTERN extern "C" __declspec(dllexport)

#else

#define EXTERN extern "C"

#endif

#define DEPRECATED(reason) [[deprecated(reason)]]