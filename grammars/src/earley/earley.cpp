/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    earley.cpp

Abstract:

    

Author / Creation date:

    JulesIMF / 11.01.23

Revision History:

--*/


//
// Includes / usings
//

#include <iostream>
#include <cassert>
#include <earley/earley.h>

//
// Definitions
//

namespace Grammars::Parsing
{
    struct Situation
    {
        Rule const& Rule;
        size_t i;
        size_t DotIdx = 0;

        bool operator<(Situation const& other) const
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

        bool End() const
        {
            assert(DotIdx <= Rule.Body.length());
            return DotIdx == Rule.Body.length();
        }

        String::value_type Next() const
        {
            assert(DotIdx <= Rule.Body.length());
            if (DotIdx == Rule.Body.length())
                return '\0';
            
            return Rule.Body[DotIdx];
        }

        Situation Scanned() const
        {
            assert(!End());
            return Situation { Rule, i, DotIdx + 1 };
        }
    };

    using SituationContainer = Vector<Set<Situation>>;
    char const Super = '$';

    void Scan_(Grammar const& Grammar, 
               SituationContainer& Situations, 
               size_t j,
               String const& Word)
    {
        if (j == 0)
            return;
        
        assert(j <= Word.length());
        auto nextSymbol = Word[j - 1];
        
        for (auto const& situation : Situations[j - 1])
        {
            if (situation.Next() == nextSymbol)
                Situations[j].insert(situation.Scanned());
        }
    }

    void Predict_(Grammar const& Grammar,
                  SituationContainer& Situations,
                  size_t j,
                  String const& Word)
    {
        assert(j <= Word.length());

        for (auto const& situation : Situations[j])
        {
            if (Grammar.Nonterminals().find(situation.Next()) == 
                Grammar.Nonterminals().end())
            {
                continue;
            }

            for (auto const& rule : Grammar.RulesFrom(situation.Next()))
                Situations[j].insert({ rule, j });
        }
    }

    void Complete_(Grammar const& Grammar,
                   SituationContainer& Situations,
                   size_t j,
                   String const& Word)
    {
        assert(j <= Word.length());

        for (auto const& completed : Situations[j])
        {
            if (!completed.End())
                continue;

            for (auto const& parent : Situations[completed.i])
            {
                if (parent.Next() != completed.Rule.Head)
                    continue;
                
                Situations[j].insert(parent.Scanned());
            }
        }
    }

    bool SizeChanged_(size_t& OldSize, Set<Situation> const& Situations)
    {
        if (OldSize == Situations.size())
            return false;
        
        OldSize = Situations.size();
        return true;
    }

    void PrintContainer(SituationContainer const& S)
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

        SituationContainer situations(Word.length() + 1);
        situations[0].insert({superRule, 0});
        for (int j = 0; j <= Word.length(); j++)
        {
            Scan_(Grammar, situations, j, Word);
            // PrintContainer(situations);
            size_t oldSize = situations[j].size();

            do
            {
                Complete_(Grammar, situations, j, Word);
                // PrintContainer(situations);
                Predict_(Grammar, situations, j, Word);
                // PrintContainer(situations);
            }
            while (SizeChanged_(oldSize, situations[j]));
        }

        return situations[Word.length()].find({superRule, 0, 1}) !=
               situations[Word.length()].end();
    }
}