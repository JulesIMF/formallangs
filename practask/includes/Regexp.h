/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    Regexp.h

Abstract:

    Regexp parsing functions declaration.

    Regexp examples:
        ab+c.aba.*.bac.+.+*

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/

#pragma once

//
// Includes / usings
//


#include <Common.h>
#include <Automaton.h>

//
// Definitions
//

enum
{
    SYM_ONE = '1',
    SYM_CONCAT = '.',
    SYM_UNION = '+',
    SYM_KLEENE = '*',
};

Automaton CreateSymbol(char Sym);
Automaton CreateOne();
Automaton CreateConcat(Automaton First, Automaton Second);
Automaton CreateUnion(Automaton First, Automaton Second);
Automaton CreateKleene(Automaton Source);

Automaton ParseReversePolishRegexp(std::string Regexp, AlphabetType const& alphabet);