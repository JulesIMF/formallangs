/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    grammars.h

Abstract:

    

Author / Creation date:

    JulesIMF / 10.01.23

Revision History:

--*/


//
// Includes / usings
//

#include <string>
#include <set>
#include <map>
#include <vector>
#include <stdexcept>
#include <variant>

//
// Definitions
//

namespace Grammars
{
    using Terminal = char;
    using Nonterminal = char;
    using String = std::string;
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
    };

    class Grammar
    {
        Set<Terminal> const Alphabet_;
        Set<Terminal> const Nonterminals_;
        Map<Nonterminal, Vector<Rule>> const Rules_;
        Nonterminal const Start_;

        Map<Nonterminal, Vector<Rule>> RulesVec2Map_(
            Vector<Rule> const& RulesVec,
            Set<Nonterminal> const& Nonterminals)
        {
            Map<Nonterminal, Vector<Rule>> RulesMap;

            for (auto n : Nonterminals)
                RulesMap[n] = Vector<Rule>();
            
            for (auto const& rule : RulesVec)
                RulesMap[rule.Head].push_back(rule);
        }

    public:
        Set<Terminal> const& Alphabet() const { return Alphabet_; }
        Set<Nonterminal> const& Nonterminals() const { return Nonterminals_; }
        Map<Nonterminal, Vector<Rule>> const& Rules() const { return Rules_; }
        Vector<Rule> const& RulesFrom(Nonterminal N) const { return Rules_.at(N); }
        Nonterminal Start() { return Start_; }

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
    };

    class Tree
    {
    public:
        enum class Type
        {
            Invalid = -1,
            Tree,
            Terminal,
        };

    protected:
        Type Type_;
        std::variant<Vector<Tree*>, Terminal> Variant_;

        Tree(Type Type) :
            Type_(Type)
        {}

    public:
        Type TreeType() { return Type_; }
        Vector<Tree*>& Subtrees() { return std::get<0>(Variant_); }
        Terminal Symbol() { return std::get<1>(Variant_); }

        Tree(Vector<Tree*> Subtrees) :
            Type_(Type::Tree),
            Variant_(Subtrees)
        {}

        Tree(Terminal Symbol) :
            Type_(Type::Terminal),
            Variant_(Symbol)
        {}

        static Tree Invalid()
        {
            return Tree(Type::Invalid);
        }
    };
}