/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    Automaton.h

Abstract:

    Automaton and conjugated classes definition.

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/

#pragma once

//
// Includes / usings
//

#include <cassert>
#include <string>
#include <set>
#include <vector>
#include <Common.h>

//
// Definitions
//

class State;

char const Eps = ' ';

struct Transition
{
    State* const From, * const To;

    char Sym;
    Transition(State* From, State* To, char Sym);
    void Remove();

    bool operator<(Transition const& Other) const;
    bool operator==(Transition const& Other) const;
};

class State
{
public:
    enum class Color { White, Gray, Black };
    using TransitionsContainer = std::set<Transition>;
    using StatesContainer = std::set<State*>;

protected:
    static size_t TotalAllocated_;
    static StatesContainer Allocated_;

    size_t const Id_ = 0;
    Color Color_ = Color::White;
    std::string Name_;
    bool Finite_ = false;

    TransitionsContainer Outputs_;
    TransitionsContainer Inputs_;
    
    State(size_t Id, std::string Name);
    ~State() = default;

public:
    static State* Allocate(std::string Name = "\0");
    static void DestructAll();
    static void ResetAll();
    static StatesContainer const& AllocatedStates()
    {
        return Allocated_;
    }

    void Connect(State* To, char Sym);
    void Disconnect(State* To, char Sym);

    TransitionsContainer const& Transitions();
    TransitionsContainer TransitionsBy(char Sym);
    State* To(char Sym);

    size_t inline Id()
    {
        return Id_;
    }

    bool inline IsVisited()
    {
        return Color_ != Color::White;
    }

    void inline StartVisit()
    {
        Color_ = Color::Gray;
    }

    void inline EndVisit()
    {
        Color_ = Color::Black;
    }

    void inline SetName(std::string NewName)
    {
        Name_ = NewName;
    }

    std::string const& Name()
    {
        return Name_;
    }

    void inline SetFinite()
    {
        Finite_ = true;
    }
    void inline UnsetFinite()
    {
        Finite_ = false;
    }

    bool inline Finite()
    {
        return Finite_;
    }
};

struct Automaton
{
    State* Initial, * Finite;

    static inline void StartUsing()
    {
        // Pass -- only for symmetry
    }

    static void EndUsing();

    inline Automaton(State* Initial, State* Finite = nullptr) :
        Initial(Initial),
        Finite(Finite)
    {
    }

    inline bool IsValid()
    {
        return (Initial != nullptr);
    }

    inline bool IsSingleFiniteValid()
    {
        return (Initial != nullptr) &&
               (Finite  != nullptr);
    }

    static inline Automaton Invalid()
    {
        return Automaton(nullptr, nullptr);
    }
};

void DebugAutomaton(Automaton Debugee, std::string Name = "automaton", std::string Folder = "img");
