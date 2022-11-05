/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    Regexp.cpp

Abstract:

    Regexp parsing implementation.

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/


//
// Includes / usings
//

#include <cctype>
#include <stdexcept>
#include <stack>
#include <Regexp.h>

//
// Definitions
//

Automaton CreateSymbol(char Sym)
{
    assert(Sym != Eps);

    State* initial = State::Allocate(std::string(1, Sym));
    State* finite  = State::Allocate();
    finite->SetFinite();

    initial->Connect(finite, Sym);
    return Automaton(initial, finite);
}


Automaton CreateOne()
{
    State* initial = State::Allocate(std::string(1, SYM_ONE));
    State* finite = State::Allocate();
    finite->SetFinite();

    initial->Connect(finite, Eps);
    return Automaton(initial, finite);
}


Automaton CreateConcat(Automaton First, Automaton Second)
{
    assert(First.IsSingleFiniteValid() &&
           Second.IsSingleFiniteValid());

    // No special states
    First.Finite->Connect(Second.Initial, Eps);
    First.Finite->SetName(std::string(1, SYM_CONCAT));
    First.Finite->UnsetFinite();
    return Automaton(First.Initial, Second.Finite);
}


Automaton CreateUnion(Automaton First, Automaton Second)
{
    assert(First.IsSingleFiniteValid() &&
           Second.IsSingleFiniteValid());

    State* initial = State::Allocate(std::string(1, SYM_UNION));
    State* finite = State::Allocate();
    finite->SetFinite();

    initial->Connect(First.Initial, Eps);
    initial->Connect(Second.Initial, Eps);

    First.Finite->Connect(finite, Eps);
    Second.Finite->Connect(finite, Eps);

    First.Finite->UnsetFinite();
    Second.Finite->UnsetFinite();

    return Automaton(initial, finite);
}


Automaton CreateKleene(Automaton Source)
{
    assert(Source.IsSingleFiniteValid());

    State* singleState = State::Allocate(std::string(1, SYM_KLEENE));
    singleState->SetFinite();

    singleState->Connect(Source.Initial, Eps);
    Source.Finite->Connect(singleState, Eps);
    Source.Finite->UnsetFinite();
    return Automaton(singleState, singleState);
}


Automaton ParseReversePolishRegexp(std::string Regexp, AlphabetType const& Aplhabet)
{
    if (Regexp.empty())
        throw std::runtime_error(
            "Empty regular expression");

    std::stack<Automaton> stack;

    size_t idx = 0;

    for (auto sym : Regexp)
    {
        if (isspace(sym))
            continue;
            
        switch (sym)
        {
            case SYM_ONE:
                stack.push(CreateOne());
                break;
            
            case SYM_CONCAT:
            {
                if (stack.size() < 2)
                    throw std::runtime_error(
                        "Not enough operands for concatenation (regexp_idx = " + 
                        std::to_string(idx) + ")");
                
                auto second = stack.top(); stack.pop();
                auto first = stack.top();  stack.pop();
                stack.push(CreateConcat(first, second));
            }
                break;
            
            case SYM_UNION:
            {
                if (stack.size() < 2)
                    throw std::runtime_error(
                        "Not enough operands for union (regexp_idx = " +
                        std::to_string(idx) + ")");

                auto second = stack.top(); stack.pop();
                auto first = stack.top();  stack.pop();
                stack.push(CreateUnion(first, second));
            }
                break;
            
            case SYM_KLEENE:
            {
                if (stack.size() < 1)
                    throw std::runtime_error(
                        "No operand for Kleene star (regexp_idx = " +
                        std::to_string(idx) + ")");

                auto source = stack.top(); stack.pop();
                stack.push(CreateKleene(source));
            }
                break;
            
            default:
                if (Aplhabet.find(sym) == Aplhabet.end())
                    throw std::runtime_error(
                        "Invalid symbol \'" + 
                        std::string(1, sym) +
                        "\' (regexp_idx = " +
                        std::to_string(idx) + ")");

                stack.push(CreateSymbol(sym));
        }

        idx++;
    }

    if (stack.size() != 1)
        throw std::runtime_error(
            "Extra expressions left in stack after regular expression parsing (stack.size() = " +
            std::to_string(stack.size()) + ")");
    
    return stack.top();
}
