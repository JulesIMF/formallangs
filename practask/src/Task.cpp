/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    Task.cpp

Abstract:

    Immediate tasks solutions.

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/


//
// Includes / usings
//

#include <Regexp.h>
#include <Optimize.h>

//
// Definitions
//

size_t TryAcceptTask13(Automaton automaton, std::string::const_iterator begin, std::string::const_iterator end)
{
    assert(automaton.IsValid());

    size_t stepped = 0;
    size_t maxAcceptedPrefixLen = 0;
    State* current = automaton.Initial;
    
    for (auto& position = begin; position != end; position++)
    {
        auto to = current->To(*position);
        if (to == nullptr)
            return maxAcceptedPrefixLen;
        
        stepped++;        
        current = to;
        
        if (current->Finite())
            maxAcceptedPrefixLen = stepped;
    }

    return maxAcceptedPrefixLen;
}

size_t SolveTask13(std::string const& ReversePolishRegexp, std::string const& Word, AlphabetType const& Alphabet, bool Debug)
{
    Automaton::StartUsing();

    auto automaton = ParseReversePolishRegexp(ReversePolishRegexp, Alphabet);
    assert(automaton.IsValid());    
    if (Debug)
        DebugAutomaton(automaton, "regexp");

    automaton = RemoveEpsilonTransitions(automaton);
    assert(automaton.IsValid());
    if (Debug)
        DebugAutomaton(automaton, "epsremoved");

    automaton = NdfsmToDfsm(automaton, Alphabet);
    assert(automaton.IsValid());
    if (Debug)
        DebugAutomaton(automaton, "dfsm");

    size_t maxAcceptedSubstrLen = 0;
    size_t symbolsLeft = Word.length();

    for (auto current = Word.cbegin(); current != Word.cend(); current++, symbolsLeft--)
    {
        if (maxAcceptedSubstrLen >= symbolsLeft)
            break;
        
        maxAcceptedSubstrLen = std::max(maxAcceptedSubstrLen, 
            TryAcceptTask13(automaton, current, Word.cend()));
    }
    
    Automaton::EndUsing();

    return maxAcceptedSubstrLen;
}