/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    Optimize.h

Abstract:

    Automaton optimization routines declarations.

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/

#pragma once

//
// Includes / usings
//

#include <map>
#include <unordered_set>
#include <iostream>
#include <Common.h>
#include <Automaton.h>

using DeltaType = std::map<State*, State::StatesContainer>;

//
// Definitions
//

Automaton RemoveEpsilonTransitions(Automaton Automaton);
Automaton NdfsmToDfsm(Automaton Auto, AlphabetType const& Alphabet);