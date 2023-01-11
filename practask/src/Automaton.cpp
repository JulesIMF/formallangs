/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    Automaton.cpp

Abstract:

    Transition, State and Automaton methods definition.

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/


//
// Includes / usings
//

#include <utility>
#include <cassert>
#include <fstream>
#include <Automaton.h>

//
// Definitions
//

Transition::Transition(State* From, State* To, char Sym) :
    From(From),
    To(To),
    Sym(Sym)
{
}


void Transition::Remove()
{
    From->Disconnect(To, Sym);
}


bool Transition::operator<(Transition const& Other) const
{
    return (From < Other.From) ||
           (From == Other.From && To < Other.To) ||
           (From == Other.From && To == Other.To && Sym < Other.Sym);
}


bool Transition::operator==(Transition const& Other) const
{
    return From == Other.From &&
           To == Other.To &&
           Sym == Other.Sym;
}

// -------------------------------------------------------

State::State(size_t Id, std::string Name) :
    Id_(Id),
    Name_(Name)
{
}


State* State::Allocate(std::string Name)
{
    State* state = new State(TotalAllocated_++, Name);
    Allocated_.insert(state);
    return state;
}


void State::DestructAll()
{
    for (auto state : Allocated_)
        delete state;
    
    Allocated_.clear();
}


void State::ResetAll()
{
    for (auto state : Allocated_)
        state->Color_ = Color::White;
}


void State::Connect(State* To, char Sym)
{
    Transition transition(/* From = */ this, To, Sym);
    Outputs_.insert(transition);
    To->Inputs_.insert(transition);
}


void State::Disconnect(State* To, char Sym)
{
    Transition transition(/* From = */ this, To, Sym);

    auto outIt = Outputs_.find(transition);
    if (outIt != Outputs_.end())
        Outputs_.erase(outIt);
    
    auto inIt = To->Inputs_.find(transition);
    if (inIt != To->Inputs_.end())
        To->Inputs_.erase(inIt);
}


State::TransitionsContainer const& State::Transitions()
{
    return Outputs_;
}


State::TransitionsContainer State::TransitionsBy(char Sym)
{
    TransitionsContainer transitionsBySym;
    for (auto const& transition : Transitions())
    {
        if (transition.Sym == Sym)
            transitionsBySym.insert(transition);
    }

    return std::move(transitionsBySym);
}

State* State::To(char Sym)
{
    for (auto const& transition : Transitions())
    {
        if (transition.Sym == Sym)
            return transition.To;
    }

    return nullptr;
}

size_t State::TotalAllocated_ = 0;
std::set<State*> State::Allocated_;

// -------------------------------------------------------

void Automaton::EndUsing()
{
    State::DestructAll();
}


void DebugAutomatonTraverse(State* State, std::ofstream& DotFile)
{
    State->StartVisit();

    if (!State->Finite())
        DotFile << "s" << uint64_t(State) << " [label = \"" << 
        State->Name() <<
        "\n[" << State->Id() << "]\", shape=\"circle\"];\n";
    
    else
        DotFile << "s" << uint64_t(State) << " [label = \"" <<
        State->Name() <<
        "\n[" << State->Id() << "]\", shape=\"doublecircle\"];\n";

    for (auto transition : State->Transitions())
    {        
        DotFile << "s" << uint64_t(transition.From) << "->s" << uint64_t(transition.To) <<
                   " [label = \"" << (transition.Sym == Eps ? ' ' : transition.Sym) <<
                   "\"];\n";
        
        if (transition.To->IsVisited())
            continue;

        DebugAutomatonTraverse(transition.To, DotFile);
    }

    State->EndVisit();
}


void DebugAutomaton(Automaton Debugee, std::string Name, std::string Folder)
{
    std::string dotFileName = Folder + "/" + Name + ".dot";
    std::string pngFileName = Folder + "/" + Name + ".png";

    std::ofstream dotFile(dotFileName);
    dotFile << "digraph\n{\ndpi = 300;\nrankdir=\"LR\";\n";
    dotFile << "nowhere[label=\"\", shape=\"none\"];";

    DebugAutomatonTraverse(Debugee.Initial, dotFile);
    State::ResetAll();

    dotFile << "nowhere->s" << uint64_t(Debugee.Initial) << ";\n";
    dotFile << "}";
    dotFile.close();

    system(("dot -Tpng " + dotFileName + " -o " + pngFileName + " > /dev/null").c_str());
}
