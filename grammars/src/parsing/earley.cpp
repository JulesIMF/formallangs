/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    earley.cpp

Abstract:

    Earley parser implementation and internal
    structures definition.

Author / Creation date:

    JulesIMF / 11.01.23

Revision History:

--*/


//
// Includes / usings
//

#include <iostream>
#include <cassert>
#include <parsing/earley.h>

//
// Definitions
//

namespace Grammars::Parsing
{
    struct Item
    {
        Rule const& Rule;
        size_t i;
        size_t DotIdx = 0;

        bool operator<(Item const& other) const
        {
            if (&Rule < &other.Rule)
                return true;
            
            if (&Rule == &other.Rule &&
                i < other.i)
            {
                return true;
            }

            if (&Rule == &other.Rule &&
                i == other.i &&
                DotIdx < other.DotIdx)
            {
                return true;
            }

            return false;
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

        /// @brief This function creates a copy of the item
        /// with a shifted dot.
        /// @return Item with a shifted dot
        Item Scanned() const
        {
            assert(!End());
            return Item { Rule, i, DotIdx + 1 };
        }
    };

    using ItemContainer = Vector<Set<Item>>;
    char const Super = '$'; // Used to complete the grammar


    /// @brief SCAN function implementation: shift the dot
    /// if next nonterminal in the word is equal to current
    /// symbol in the rule
    /// @param Grammar Const ref to the used grammar
    /// @param Items NOT const ref to the items 
    /// container
    /// @param j Current index in the main loop
    /// @param Word Parsed word
    void Scan_(Grammar const& Grammar, 
               ItemContainer& Items, 
               size_t j,
               String const& Word)
    {
        if (j == 0)
            return;
        
        assert(j <= Word.length());
        auto nextSymbol = Word[j - 1];
        
        for (auto const& item : Items[j - 1])
        {
            if (item.Next() == nextSymbol)
                Items[j].insert(item.Scanned());
        }
    }

    /// @brief PREDICT function implementation: going down the
    /// tree if next symbol is a terminal
    /// @param Grammar Const ref to the used grammar
    /// @param Items NOT const ref to the items 
    /// container
    /// @param j Current index in the main loop
    /// @param Word Parsed word
    void Predict_(Grammar const& Grammar,
                  ItemContainer& Items,
                  size_t j,
                  String const& Word)
    {
        assert(j <= Word.length());

        for (auto const& item : Items[j])
        {
            if (Grammar.Nonterminals().find(item.Next()) == 
                Grammar.Nonterminals().end())
            {
                continue;
            }

            for (auto const& rule : Grammar.RulesFrom(item.Next()))
                Items[j].insert({ rule, j });
        }
    }


    /// @brief COMPLETE function implementation: returning
    /// to the parent rule after completing deriving 
    /// from the rule
    /// @param Grammar Const ref to the used grammar
    /// @param Items NOT const ref to the items 
    /// container
    /// @param j Current index in the main loop
    /// @param Word Parsed word
    void Complete_(Grammar const& Grammar,
                   ItemContainer& Items,
                   size_t j,
                   String const& Word)
    {
        assert(j <= Word.length());

        for (auto const& completed : Items[j])
        {
            if (!completed.End())
                continue;

            for (auto const& parent : Items[completed.i])
            {
                if (parent.Next() != completed.Rule.Head)
                    continue;
                
                Items[j].insert(parent.Scanned());
            }
        }
    }


    /// @brief Checks if Items size has changed and
    /// updating OldSize if so.
    /// @param OldSize NOT const ref to OldSize
    /// @param Items Const ref to the set of items
    /// @return True if the size has changed, false otherwise
    bool SizeChanged_(size_t& OldSize, Set<Item> const& Items)
    {
        if (OldSize == Items.size())
            return false;
        
        OldSize = Items.size();
        return true;
    }

    /// @brief Debug prints the contatiner
    /// @param S The debugee container
    void PrintContainer(ItemContainer const& S)
    {
        std::cout << "---------------------------------------------\n";
        for (int i = 0; i != S.size(); i++)
        {
            std::cout << i << "\n";
            for (auto& s : S[i])
                std::cout << "\t" << s.Rule.Head << "->" <<
                s.Rule.Body.substr(0, s.DotIdx) << "*" << 
                s.Rule.Body.substr(s.DotIdx) << ", " << 
                s.i << "\n";
            
            std::cout << "\n";
        }
    }

    /// @brief The Earley parser.
    /// @param Grammar Const ref to the used Grammar which
    /// MUST NOT contain symbol '$' as a terminal or non-terminal
    /// @param Word Parsed word
    /// @return True if the word can be derived in this grammar,
    /// false otherwise
    bool EarleyParser(Grammar const& Grammar, std::string Word)
    {
        for (auto c : Word)
        {
            if (Grammar.Alphabet().find(c) ==
                Grammar.Alphabet().end())
            {
                return false;
            }
        }

        if (Grammar.Nonterminals().find(Super) != Grammar.Nonterminals().end() ||
            Grammar.Alphabet().find(Super) != Grammar.Alphabet().end())
        {
            throw std::logic_error("Grammar contains reserved symbol \'$\'!");
        }

        Rule superRule = { Super, {Grammar.Start()} };

        ItemContainer items(Word.length() + 1);
        items[0].insert({superRule, 0});
        for (int j = 0; j <= Word.length(); j++)
        {
            Scan_(Grammar, items, j, Word);
            // PrintContainer(items);
            size_t oldSize = items[j].size();

            do
            {
                Complete_(Grammar, items, j, Word);
                // PrintContainer(items);
                Predict_(Grammar, items, j, Word);
                // PrintContainer(items);
            }
            while (SizeChanged_(oldSize, items[j]));
        }

        return items[Word.length()].find({superRule, 0, 1}) !=
               items[Word.length()].end();
    }
}