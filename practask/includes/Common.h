/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    Common.h

Abstract:

    Common definitions.

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/

#pragma once

//
// Includes / usings
//

#include <cstdio>
#include <unordered_set>

using AlphabetType = std::unordered_set<char>;

//
// Definitions
//

#ifdef DEBUG
#define DEBUG_OUT(fmt, ...) fprintf(stderr, "Debug (%s:%d):" fmt "\n" , __FILE__, __LINE__, __VA_ARGS__);
#else
#define DEBUG_OUT(fmt, ...) ;
#endif