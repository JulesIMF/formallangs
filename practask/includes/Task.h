/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    Task.h

Abstract:

    Declarations of immediate tasks solution functions.

Author / Creation date:

    JulesIMF / 06.11.22

Revision History:

--*/

#pragma once

//
// Includes / usings
//

#include <string>
#include <Common.h>

//
// Definitions
//

/*
    Tests:
        ab+c.aba.*.bac.+.+* babc            2
        acb..bab.c.*.ab.ba.+.+*a. abbaa     4
*/

size_t SolveTask13
(
    std::string const& ReversePolishRegexp, 
    std::string const& Word, 
    AlphabetType const& Alphabet,
    bool Debug = false
);