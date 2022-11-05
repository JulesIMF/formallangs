/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    Optimize.cpp

Abstract:

    Automatons transformations and optimizations implementation.

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/


//
// Includes / usings
//

// #define DEBUG
#include <bitset>
#include <queue>
#include <unordered_set>
#include <Optimize.h>

//
// Definitions
//

// ******************************************************
//                    Common routines
// ******************************************************

void PrintReachable(DeltaType const& Reachable, std::ostream& stream = std::cout)
{
    for (auto const& pair : Reachable)
    {
        stream << pair.first->Id() << ": ";
        for (auto state : pair.second)
            stream << state->Id() << " ";
        
        stream << "\n";
    }
}

size_t SumSize(DeltaType const& Reachable)
{
    size_t sumSize = 0;
    for (auto const& pair : Reachable)
        sumSize += pair.second.size();
    
    return sumSize;
}

DeltaType ReachableByOneStep(char Sym)
{
    DeltaType reachable;

    for (auto state : State::AllocatedStates())
    {
        reachable[state] = State::StatesContainer();

        for (auto transition : state->TransitionsBy(Sym))
            reachable[state]
            .insert(transition.To);
    }

    return reachable;
}

DeltaType Reachable(char Sym)
{
    DeltaType reachable = ReachableByOneStep(Sym);

    size_t oldSumSize = 0;
    size_t newSumSize = SumSize(reachable);

    DeltaType oneStepReachable = reachable;

    while ((oldSumSize < newSumSize) && (oldSumSize = newSumSize))
    {
        auto oldReachable = reachable;

        for (auto state : State::AllocatedStates())
        {
            for (auto to : oneStepReachable[state]) // Delta_1
            {
                // Delta_k = Delta_{k - 1} (Delta_1)

                auto oldReachableIt = oldReachable.find(to);
                assert(oldReachableIt != oldReachable.end());

                reachable[state].insert(oldReachableIt->second.begin(),
                    oldReachableIt->second.end());
            }
        }

        newSumSize = SumSize(reachable);
        assert(newSumSize >= oldSumSize);
    }

    return reachable;
}

// ******************************************************
//             Epsilon transitions removal
// ******************************************************

DeltaType EpsReachable()
{
    return Reachable(Eps);
}

void EpsRemovalContractTransitions(DeltaType const& EpsReachable)
{
    size_t totalAdded = 0;

    for (auto const& pair : EpsReachable)
    {
        auto state = pair.first;
        auto const& reachable = pair.second;

        for (auto epsReachableState : reachable)
        {
            for (auto transition : epsReachableState->Transitions())
                if (transition.Sym != Eps)
                {
                    state->Connect(transition.To, transition.Sym);
                    totalAdded++;
                }
        }
    }

    DEBUG_OUT("totalAdded = %zu", totalAdded);
}

void EpsRemovalAddFinites(DeltaType const& EpsReachable)
{
    size_t oldFinites = 0;
    size_t newFinites = 0;

    for (auto const& pair : EpsReachable)
    {
        auto state = pair.first;
        auto const& reachable = pair.second;
        if (state->Finite())
        {
            oldFinites++;
            continue;
        }

        for (auto epsReachableState : reachable)
        {
            if (epsReachableState->Finite())
            {
                state->SetFinite();
                newFinites++;
                break;
            }
        }
    }

    DEBUG_OUT("oldFinites = %zu, newFinites = %zu", oldFinites, newFinites);
}

void EpsRemovalRemoveEpsTransitions()
{
    for (auto state : State::AllocatedStates())
    {
        for (auto transition : state->TransitionsBy(Eps))
            transition.Remove();
    }
}

Automaton RemoveEpsilonTransitions(Automaton Auto)
{
    DeltaType epsReachable = EpsReachable();

#ifdef DEBUG
    PrintReachable(epsReachable);
#endif

    EpsRemovalContractTransitions(epsReachable);
    EpsRemovalAddFinites(epsReachable);
    EpsRemovalRemoveEpsTransitions();

    return Automaton(Auto.Initial);
}

// ******************************************************
//                  NDFSM to DFSM 
// ******************************************************

std::string NameOfSet(State::StatesContainer set)
{
    if (set.empty())
        return "";
    
    std::string name = "";
    auto statesLeft = set.size();

    for (auto state : set)
    {
        name += std::to_string(state->Id());
        if (--statesLeft)
            name += "|";
    }

    return name;
}

Automaton NdfsmToDfsm(Automaton Auto, AlphabetType const& Alphabet)
{
    std::map<char, DeltaType> reachableBy;
    for (auto sym : Alphabet)
        reachableBy[sym] = ReachableByOneStep(sym);

    std::map<State::StatesContainer, State*> newStates;

    std::queue<State::StatesContainer> bfsQueqe;
    bfsQueqe.push({Auto.Initial});
    newStates[{Auto.Initial}] = State::Allocate(NameOfSet({Auto.Initial}));

    while (!bfsQueqe.empty())
    {
        auto set = bfsQueqe.front();
        bfsQueqe.pop();

        auto state = newStates.at(set);

        for (auto sym : Alphabet)
        {
            State::StatesContainer to;
            for (auto state : set)
            {
                auto const& reachableBySym = reachableBy[sym][state];
                to.insert(reachableBySym.begin(), reachableBySym.end());
            }

            if (to.empty())
                continue;

            auto newStatesToIter = newStates.find(to);
            
            if (newStatesToIter == newStates.end())
            {
                auto stateTo = State::Allocate(NameOfSet(to));
                newStates[to] = stateTo;

                state->Connect(stateTo, sym);

                bfsQueqe.push(to);
            }

            else
            {
                state->Connect(newStatesToIter->second, sym);
            }
        }
    }

    for (auto const& pair : newStates)
    {
        auto const& set = pair.first;
        auto state = pair.second;

        for (auto initialState : set)
        {
            if (initialState->Finite())
            {
                state->SetFinite();
                break;
            }                
        }
    }

    return Automaton(newStates[{Auto.Initial}]);
}