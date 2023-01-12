/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    lr.cpp

Abstract:

    

Author / Creation date:

    JulesIMF / 12.01.23

Revision History:

--*/


//
// Includes / usings
//

#include <parsing/lr.h>
#include <algorithm>
#include <stack>

//
// Definitions
//

namespace Grammars::Parsing
{
    LR::LR(Grammar const& Grammar) : 
        Grammar_(CompleteGrammar_(Grammar))
    {
        AllSymbols_ = Grammar_.AllSymbols();
        BuildRulesVec_();
        BuildFirstTable_();
        BuildStates_();
        BuildLRTable_();
    }

    Grammar LR::CompleteGrammar_(Grammar const& Incomplete)
    {
        if (Incomplete.IsTerminal(Super) || Incomplete.IsNonterminal(Super))
        {
            throw std::logic_error("Grammar contains reserved symbol \'#\'!");
        }

        if (Incomplete.IsTerminal(EOL) || Incomplete.IsNonterminal(EOL))
        {
            throw std::logic_error("Grammar contains reserved symbol \'$\'!");
        }

        auto complete = Incomplete;
        complete.AddNonterminal(Super);
        complete.AddRule({Super, {Incomplete.Start()}});
        complete.SetStart(Super);

        size_t id = 0;
        for (auto n : complete.Nonterminals())
            for (auto& rule : complete.RulesFrom(n))
                rule.Id = id++;
        
        return complete;
    }

    void LR::BuildRulesVec_()
    {
        TotalRules_ = 0;
        for (auto n : Grammar_.Nonterminals())
            TotalRules_ += Grammar_.RulesFrom(n).size();

        RulesVec_.resize(TotalRules_);
        for (auto n : Grammar_.Nonterminals())
            for (auto const& rule : Grammar_.RulesFrom(n))
            {
                assert(rule.Id < TotalRules_);
                RulesVec_[rule.Id] = &rule;
            }
    }

    void LR::BuildFirstTable_()
    {
        bool changed = true;

        for (auto a : Grammar_.Alphabet())
            FirstTable_[a].insert(a);
        
        FirstTable_[EOL].insert(EOL);

        while (changed) 
        {
            changed = false;

            for (auto rule : RulesVec_) {
                Nonterminal head = rule->Head;
                
                if (rule->Body.empty())
                {
                    changed |= FirstTable_[head].insert(Eps).second;
                    continue;
                }

                size_t epsCounter = 0;

                for (auto Y : rule->Body)
                {
                    for (auto first : FirstTable_[Y])
                    {
                        if (first != Eps)
                            changed |= FirstTable_[head].insert(first).second;
                    }

                    if (FirstTable_[Y].find(Eps) == FirstTable_[Y].end())
                        break;
                    
                    epsCounter++;
                }

                if (epsCounter == rule->Body.length())
                    changed |= FirstTable_[head].insert(Eps).second;
            }
        }
    }

    void LR::BuildStates_()
    {
        States_.clear();

        assert(Grammar_.RulesFrom(Super).size() == 1);
        States_.push_back(Closure({Item{
            Grammar_.RulesFrom(Super).front(),
            EOL,
            0
        }}));

        bool changed = true;
        while (changed)
        {
            changed = false;
            size_t oldSize = States_.size();
            for (size_t stateIdx = 0; stateIdx < oldSize; stateIdx++)
            {
                for (auto symbol : AllSymbols_)
                {
                    auto newState = GoTo(States_[stateIdx], symbol);
                    if (newState.empty())
                        continue;
                    
                    if (std::find(States_.begin(), States_.end(), newState) ==
                        States_.end())
                    {
                        States_.push_back(newState);
                        changed = true;
                    }
                }
            }
        }
        
    }

    void LR::TrySetAction_(size_t StateIdx, SymbolType Symbol, Action NewAction)
    {
        if (LRTable_[StateIdx][Symbol].Type != ActionType::Error &&
            LRTable_[StateIdx][Symbol] != NewAction)
        {
            throw std::runtime_error("LR1 error: Grammar is not LR(1)");
        }
            
        
        LRTable_[StateIdx][Symbol] = NewAction;
    }

    size_t LR::FindState_(SetOfItems const& State)
    {
        auto it = std::find(States_.begin(), States_.end(), State);
        return it - States_.begin();
    }

    void LR::BuildLRTable_()
    {
        LRTable_.resize(States_.size());

        for (size_t stateIdx = 0; stateIdx != States_.size(); stateIdx++)
        {
            for (auto symbol : AllSymbols_)
                LRTable_[stateIdx][symbol] = { ActionType::Error };
            
            LRTable_[stateIdx][EOL] = { ActionType::Error };
        }
        
        for (size_t stateIdx = 0; stateIdx != States_.size(); stateIdx++)
        {
            auto const& state = States_[stateIdx];
            for (auto const& item : state)
            {
                if (item.End())
                {
                    if (item.Rule.Head == Super)
                        TrySetAction_(stateIdx, EOL, {ActionType::Accept});
                    
                    else
                        TrySetAction_(stateIdx, item.C, 
                            {ActionType::Reduce, .RuleIdx = item.Rule.Id});
                }

                else
                {
                    auto nextChar = item.Next();
                    if (!Grammar_.IsTerminal(nextChar))
                        continue;
                    
                    auto nextStateIdx = FindState_(GoTo(state, nextChar));
                    if (nextStateIdx == States_.size())
                        continue;

                    TrySetAction_(stateIdx, nextChar, 
                        {ActionType::Shift, .StateIdx = nextStateIdx});
                }
            }

            for (auto A : Grammar_.Nonterminals())
            {
                auto nextStateIdx = FindState_(GoTo(state, A));
                if (nextStateIdx == States_.size())
                    continue;

                TrySetAction_(stateIdx, A, 
                    {ActionType::GoTo, .StateIdx = nextStateIdx});
            }
        }
    }
    
    Set<Terminal> LR::First(String S)
    {
        Set<Terminal> first;
        for (auto X : S)
        {
            for (auto firstSymbol : FirstTable_[X])
                if (firstSymbol != Eps)
                    first.insert(firstSymbol);
            
            if (FirstTable_[X].find(Eps) == FirstTable_[X].end())
                break;
        }

        return first;
    }

    LR::SetOfItems LR::Closure(LR::SetOfItems const& Items)
    {
        auto closed = Items;

        while (true)
        {
            SetOfItems added;
            for (auto const& item : closed)
            {
                if (item.End())
                    continue;
                
                auto nextSymbol = item.Next();
                if (!Grammar_.IsNonterminal(nextSymbol))
                    continue;
                
                for (auto const& rule : Grammar_.RulesFrom(nextSymbol))
                    for (auto b : First(item.Remainder() + item.C))
                        if (Grammar_.IsTerminal(b) || b == EOL)
                            added.insert(Item{rule, b, 0});
            }

            auto oldSize = closed.size();
            closed.insert(added.begin(), added.end());
            if (oldSize == closed.size())
                break;
        }

        return closed;
    }

    LR::SetOfItems LR::GoTo(LR::SetOfItems const& Items, SymbolType Next)
    {
        SetOfItems gone;
        for (auto const& item : Items)
            if (!item.End() && item.Next() == Next)
                gone.insert(item.Scanned());
        
        return Closure(gone);
    }

    void LR::SetWord_(String const& Word)
    {
        CurrentWord_ = Word;
        CurrentWord_ += EOL;
        WordIdx_ = 0;
    }

    void LR::Read_()
    {
        assert(WordIdx_ < CurrentWord_.length());
        WordIdx_++;
    }

    Terminal LR::Watch_()
    {
        assert(WordIdx_ < CurrentWord_.length());
        return CurrentWord_[WordIdx_];
    }

    bool LR::Derives(String const& Word)
    {
        for (auto c : Word)
            if (!Grammar_.IsTerminal(c))
                return false;
        
        SetWord_(Word);
        std::stack<size_t> stateStack;
        stateStack.push(StartState);

        while (true)
        {
            auto nextChar = Watch_();
            auto topState = stateStack.top();

            auto action = LRTable_[topState][nextChar];
            switch (action.Type)
            {
            case ActionType::Accept:
                assert(stateStack.size() == 2);
                return true;
            
            case ActionType::Error:
                return false;
            
            case ActionType::Shift:
                stateStack.push(action.StateIdx);
                Read_();
                break;
            
            case ActionType::Reduce:
            {
                auto rule = RulesVec_[action.RuleIdx];
                for (size_t i = 0; i != rule->Body.length(); i++)
                {
                    stateStack.pop();
                    assert(!stateStack.empty());
                }

                topState = stateStack.top();
                auto goTo = LRTable_[topState][rule->Head];
                assert(goTo.Type == ActionType::GoTo);

                stateStack.push(goTo.StateIdx);
            }
            break;

            case ActionType::GoTo:
            default:
                assert(!"WTF IS GOING ON?????");
            }
        }
    }
}

