/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    lr.h

Abstract:

    

Author / Creation date:

    JulesIMF / 12.01.23

Revision History:

--*/

#pragma once

//
// Includes / usings
//

#include <iostream>
#include <cassert>
#include <grammars.h>

//
// Definitions
//

namespace Grammars::Parsing
{
    class LR
    {
    public:
        struct Item
        {
            Rule const& Rule;
            SymbolType C;
            size_t DotIdx = 0;

            bool operator<(Item const& other) const
            {
                if (Rule.Id < other.Rule.Id)
                    return true;

                if (Rule.Id == other.Rule.Id &&
                    C < other.C)
                {
                    return true;
                }

                if (Rule.Id == other.Rule.Id &&
                    C == other.C &&
                    DotIdx < other.DotIdx)
                {
                    return true;
                }

                return false;
            }

            bool operator==(Item const& other) const
            {
                return (Rule.Id == other.Rule.Id &&
                        C == other.C &&
                        DotIdx == other.DotIdx);
            }

            /// @brief This fuction checks if dot is 
            /// in the end of the rule
            /// @return DotIdx == Rule.Body.length()
            bool End() const
            {
                assert(DotIdx <= Rule.Body.length());
                return DotIdx == Rule.Body.length();
            }

            /// @brief This function is used to get next
            /// char after the dot.
            /// @return Char after the dot
            String::value_type Next() const
            {
                assert(DotIdx <= Rule.Body.length());
                if (DotIdx == Rule.Body.length())
                    return '\0';

                return Rule.Body[DotIdx];
            }

            /// @brief This function is used to get substring
            /// after the next symbol
            /// @return Char after the dot
            String Remainder() const
            {
                assert(DotIdx < Rule.Body.length());

                return Rule.Body.substr(DotIdx + 1);
            }

            /// @brief This function creates a copy of the item
            /// with a shifted dot.
            /// @return Item with a shifted dot
            Item Scanned() const
            {
                assert(!End());
                return Item{ Rule, C, DotIdx + 1 };
            }

            void Print() const
            {
                std::cout << Rule.Head << "->"
                << Rule.Body.substr(0, DotIdx) << "*"
                << Rule.Body.substr(DotIdx) << ", " << C;
            }
        };

        using SetOfItems = Set<Item>; // Using structure name from the Dragon Book
        SymbolType const Super = '#';
        SymbolType const EOL = '$';

    protected:
        enum class ActionType
        {
            Error,
            Shift,
            Reduce,
            GoTo,
            Accept
        };

        struct Action
        {
            ActionType Type;

            union
            {
                size_t StateIdx = 0;
                size_t RuleIdx;
            };

            bool operator==(Action const& other) const
            {
                return other.Type == Type &&
                       StateIdx == other.StateIdx;
            }

            bool operator!=(Action const& other) const
            {
                return !(*this == other);
            }
        };

        //
        // Usings an consts
        //
    
    protected:
        using LRTableType = Vector<Map<SymbolType, Action>>;
        using FirstTableType = Map<SymbolType, Set<Terminal>>;
        SymbolType const Eps = '\0';
        size_t const StartState = 0;

        //
        // Fields
        //

        Grammar const Grammar_;
        Vector<Rule const*> RulesVec_;
        FirstTableType FirstTable_;
        Vector<SetOfItems> States_;
        LRTableType LRTable_;

        size_t TotalRules_ = 0;
        Set<SymbolType> AllSymbols_;

        size_t WordIdx_;
        String CurrentWord_;
        
        //
        // Internal functions
        //

        Grammar CompleteGrammar_(Grammar const& Grammar);

        void BuildRulesVec_();
        void BuildFirstTable_();
        void BuildStates_(); 
        void BuildLRTable_();
        void TrySetAction_(size_t StateIdx, SymbolType symbol, Action NewAction);
        size_t FindState_(SetOfItems const& State);
        void SetWord_(String const& Word);
        Terminal Watch_();
        void Read_();
    
    public:
        Set<Terminal> First(String S);
        SetOfItems Closure(SetOfItems const& Items);
        SetOfItems GoTo(SetOfItems const& Items, SymbolType Next);
        LR(Grammar const& Grammar);
        bool Derives(String const& Word);
    };
}