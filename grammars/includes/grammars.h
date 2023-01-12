/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    grammars.h

Abstract:

    

Author / Creation date:

    JulesIMF / 10.01.23

Revision History:

--*/

#pragma once

//
// Includes / usings
//

#include <string>
#include <set>
#include <map>
#include <vector>
#include <stdexcept>
#include <variant>
#include <ostream>

//
// Definitions
//

namespace Grammars
{
    using String = std::string;
    using SymbolType = String::value_type;
    using Terminal = SymbolType;
    using Nonterminal = SymbolType;
    Nonterminal const Eps = '#';

    template<class T>
    using Set = std::set<T>;

    template<class K, class V>
    using Map = std::map<K, V>;

    template<class T>
    using Vector = std::vector<T>;

    struct Rule
    {
        Nonterminal Head;
        String Body;
        size_t Id = 0;
        bool operator==(Rule const& other) const = default;
    };

    class Grammar
    {
        Set<Terminal> Alphabet_;
        Set<Terminal> Nonterminals_;
        Map<Nonterminal, Vector<Rule>> Rules_;
        Nonterminal Start_;

        Map<Nonterminal, Vector<Rule>> RulesVec2Map_(
            Vector<Rule> const& RulesVec,
            Set<Nonterminal> const& Nonterminals)
        {
            Map<Nonterminal, Vector<Rule>> rulesMap;

            for (auto n : Nonterminals)
                rulesMap[n] = Vector<Rule>();
            
            for (auto const& rule : RulesVec)
                rulesMap[rule.Head].push_back(rule);

            return rulesMap;
        }

    public:
        Set<Terminal> const& Alphabet() const { return Alphabet_; }
        Set<Nonterminal> const& Nonterminals() const { return Nonterminals_; }
        Set<SymbolType> AllSymbols() const
        {
            Set<SymbolType> all = Alphabet_;
            all.insert(Nonterminals_.begin(), Nonterminals_.end());
            return all;
        }

        Map<Nonterminal, Vector<Rule>>& Rules() { return Rules_; }
        Vector<Rule>& RulesFrom(Nonterminal N) { return Rules_.at(N); }

        Map<Nonterminal, Vector<Rule>> const& Rules() const { return Rules_; }
        Vector<Rule> const& RulesFrom(Nonterminal N) const { return Rules_.at(N); }
        Nonterminal Start() const { return Start_; }

        bool IsTerminal(SymbolType Symbol) const { return Alphabet().find(Symbol) != Alphabet().end(); }
        bool IsNonterminal(SymbolType Symbol) const { return Nonterminals().find(Symbol) != Nonterminals().end(); }

        Grammar(
            Set<Terminal> const& Alphabet,
            Set<Nonterminal> const& Nonterminals,
            Vector<Rule> Rules,
            Nonterminal const& Start) :

            Alphabet_(Alphabet),
            Nonterminals_(Nonterminals),
            Rules_(RulesVec2Map_(Rules, Nonterminals)),
            Start_(Start)
        {
            for (auto const& n : Nonterminals)
            {
                if (Alphabet_.find(n) != Alphabet_.end())
                    throw std::logic_error("Grammar error: Alphabet and Nonterminals intersect");
            }

            if (Nonterminals_.find(Start) == Nonterminals_.end())
                throw std::logic_error("Grammar error: Start not in nonterminals set");
        }

        void AddTerminal(Terminal NewTerminal)
        {
            for (auto const& n : Nonterminals())
            {
                if (n == NewTerminal)
                    throw std::logic_error("Grammar error: Alphabet and Nonterminals intersect");
            }

            Alphabet_.insert(NewTerminal);
        }

        void AddNonterminal(Nonterminal NewNonterminal)
        {
            for (auto const& a : Alphabet())
            {
                if (a == NewNonterminal)
                    throw std::logic_error("Grammar error: Alphabet and Nonterminals intersect");
            }

            Nonterminals_.insert(NewNonterminal);
        }

        void SetStart(Nonterminal NewStart)
        {
            if (Nonterminals_.find(NewStart) == Nonterminals_.end())
                throw std::logic_error("Grammar error: New start not in nonterminals set");

            Start_ = NewStart;
        }

        Rule const& AddRule(Rule NewRule)
        {
            if (Nonterminals_.find(NewRule.Head) == Nonterminals_.end())
                throw std::logic_error("Grammar error: non-terminal not from the"
                    " list attempted to be set as the head of the new rule");
            
            for (auto c : NewRule.Body)
            {
                if (Nonterminals_.find(c) == Nonterminals_.end() &&
                    Alphabet_.find(c) == Alphabet_.end())
                {
                    throw std::logic_error("Grammar error: unexpected symbol in the"
                                           " body of the new rule");
                }
            }
            Rules_[NewRule.Head].push_back(NewRule);
            return Rules_[NewRule.Head].back();
        }

        void Print(std::ostream& Stream)
        {
            Stream << "Terminals:\n\t";
            for (auto c : Alphabet())
                Stream << c << " ";
            
            Stream << "\n\nNon-terminals:\n\t";
            for (auto c : Nonterminals())
                Stream << c << " ";
            
            Stream << "\n\nStart: " << Start() << "\n\n";
            Stream << "Rules: \n";
            for (auto const& ruleVec : Rules())
                for (auto const& rule : ruleVec.second)
                    Stream << "\t" << rule.Head << "->" << rule.Body << "\n";
        }
    };

    // The following code had never been used and was excluded 
    // in order to avoid its considering in the code coverage

    // class Tree
    // {
    // public:
    //     enum class Type
    //     {
    //         Invalid = -1,
    //         Tree,
    //         Terminal,
    //     };

    //     struct SubtreesInfo
    //     {
    //         Nonterminal Head;
    //         Vector<Tree*> List;
    //     };

    // protected:
    //     size_t Id_;
    //     Type Type_;
    //     std::variant<SubtreesInfo, Terminal> Variant_;

    //     static size_t TotalAllocated_;

    //     Tree(Type Type) :
    //         Id_(TotalAllocated_++),
    //         Type_(Type)
    //     {}

    //     Tree(SubtreesInfo const& Subtrees) :
    //         Id_(TotalAllocated_++),
    //         Type_(Type::Tree),
    //         Variant_(Subtrees)
    //     {}

    //     Tree(Terminal Symbol) :
    //         Id_(TotalAllocated_++),
    //         Type_(Type::Terminal),
    //         Variant_(Symbol)
    //     {}

    // public:
    //     Type TreeType() { return Type_; }
    //     SubtreesInfo& Subtrees() { return std::get<0>(Variant_); }
    //     Terminal Symbol() { return std::get<1>(Variant_); }
    //     bool Valid() { return Type_ != Type::Invalid; }

    //     static Tree* AllocInvalid()
    //     {
    //         return new Tree(Type::Invalid);
    //     }

    //     static Tree* AllocTree(SubtreesInfo const& Subtrees)
    //     {
    //         return new Tree(Subtrees);
    //     }

    //     static Tree* AllocTerminal(Terminal Symbol)
    //     {
    //         return new Tree(Symbol);
    //     }

    //     ~Tree()
    //     {
    //         if (Type_ == Type::Tree)
    //         {
    //             for (auto tree : Subtrees().List)
    //                 delete tree;
    //         }
    //     }
    // };

    // void DebugTree(Tree* Debugee, std::string Name = "tree", std::string Folder = "img");

    Grammar BuildGrammar(Vector<String> const& Lines);
}