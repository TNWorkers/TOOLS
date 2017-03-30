#ifndef HELPER_MACROS_H
#define HELPER_MACROS_H

//Get infos to the used compiler and version. Taken from Eigen (Macros.h)
#ifdef __GNUC__
  #define HELPERS_COMP_GNUC 1
#else
  #define HELPERS_COMP_GNUC 0
#endif

#if COMP_GNUC
  #define HELPERS_GNUC_AT_LEAST(x,y) ((__GNUC__==x && __GNUC_MINOR__>=y) || __GNUC__>x)
  #define HELPERS_GNUC_AT_MOST(x,y)  ((__GNUC__==x && __GNUC_MINOR__<=y) || __GNUC__<x)
  #define HELPERS_GNUC_AT(x,y)       ( __GNUC__==x && __GNUC_MINOR__==y )
#else
  #define HELPERS_GNUC_AT_LEAST(x,y) 0
  #define HELPERS_GNUC_AT_MOST(x,y)  0
  #define HELPERS_GNUC_AT(x,y)       0
#endif

#if defined(__clang__)
  #define HELPERS_COMP_CLANG (__clang_major__*100+__clang_minor__)
#else
  #define HELPERS_COMP_CLANG 0
#endif

//Get supported std of the compiler
#if defined(__cplusplus)
  #if __cplusplus==201103L
    #define HELPERS_CXX11 1
  #elif __cplusplus==201402L
    #define HELPERS_CXX14 1
  #elif __cplusplus>201402L
    #define HELPERS_CXX17 1
  #endif
#endif

//Get c++-17 feautures that are supported
#if __cpp_if_constexpr >= 201606
  #define HELPERS_HAS_CONSTEXPR 1
#endif

//If this formatting library is present, it is used for nicer output of for example a MultipedeQ instance.
//See https://github.com/haarcuba/cpp-text-table for cloning the library (header only)
#if __has_include("TextTable.h")
#include "TextTable.h"
#define HELPERS_IO_TABLE 1
#endif

#endif
