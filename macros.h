#ifndef TOOLS_MACROS_H
#define TOOLS_MACROS_H

//Get infos to the used compiler and version. Taken from Eigen (Macros.h)
#ifdef __GNUC__
  #define TOOLS_COMP_GNUC 1
#else
  #define TOOLS_COMP_GNUC 0
#endif

#if COMP_GNUC
  #define TOOLS_GNUC_AT_LEAST(x,y) ((__GNUC__==x && __GNUC_MINOR__>=y) || __GNUC__>x)
  #define TOOLS_GNUC_AT_MOST(x,y)  ((__GNUC__==x && __GNUC_MINOR__<=y) || __GNUC__<x)
  #define TOOLS_GNUC_AT(x,y)       ( __GNUC__==x && __GNUC_MINOR__==y )
#else
  #define TOOLS_GNUC_AT_LEAST(x,y) 0
  #define TOOLS_GNUC_AT_MOST(x,y)  0
  #define TOOLS_GNUC_AT(x,y)       0
#endif

#if defined(__clang__)
  #define TOOLS_COMP_CLANG (__clang_major__*100+__clang_minor__)
#else
  #define TOOLS_COMP_CLANG 0
#endif

//Get supported std of the compiler
#if defined(__cplusplus)
  #if __cplusplus==201103L
    #define TOOLS_CXX11 1
  #elif __cplusplus==201402L
    #define TOOLS_CXX14 1
  #elif __cplusplus>=201703L
    #define TOOLS_CXX17 1
  #endif
#endif

//Get c++-17 feautures that are supported
#if __cpp_if_constexpr >= 201606
  #define TOOLS_HAS_CONSTEXPR 1
#endif

//If this formatting library is present, it is used for nicer output of for example a MultipedeQ instance.
//See https://github.com/haarcuba/cpp-text-table for cloning the library (header only)
#if __has_include("TextTable.h")
#include "TextTable.h"
#define TOOLS_IO_TABLE 1
#endif

#if __has_include("boost/functional/hash.hpp")
#define TOOLS_HAS_BOOST_HASH_COMBINE 1
#endif

#endif
