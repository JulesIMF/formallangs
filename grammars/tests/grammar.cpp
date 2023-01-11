/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    grammar.cpp

Abstract:

    

Author / Creation date:

    JulesIMF / 11.01.23

Revision History:

--*/


//
// Includes / usings
//

#include "tests.h"
#include <grammars.h>
#include <earley/earley.h>

//
// Definitions
//

using namespace Grammars;

TEST(TestBuilding, Brackets)
{
    Vector<String> lines = {
        "()",
        "S",
        "S",
        "S->",
        "S->(S)S"
    };

    Grammar grammar = BuildGrammar(lines);

    ASSERT_EQ(grammar.Start(), 'S');
    ASSERT_EQ(grammar.Alphabet(), Set<Terminal>({ '(', ')' }));
    ASSERT_EQ(grammar.Nonterminals(), Set<Nonterminal>({ 'S' }));
    ASSERT_EQ(grammar.Rules().size(), 1);
    
    ASSERT_NE(std::find(grammar.RulesFrom('S').begin(), grammar.RulesFrom('S').end(), 
        Rule{ 'S', "" }), 
        grammar.RulesFrom('S').end());

    ASSERT_NE(std::find(grammar.RulesFrom('S').begin(), grammar.RulesFrom('S').end(),
        Rule{ 'S', "(S)S" }),
        grammar.RulesFrom('S').end());
}

TEST(TestBuilding, LowLines)
{
    Vector<String> lines = {
        "()",
        "S",
    };

    ASSERT_THROW(BuildGrammar(lines), std::runtime_error);
}

TEST(TestBuilding, LongStart)
{
    Vector<String> lines = {
        "()",
        "S`",
        "S`",
        "S`->",
        "S`->(S`)S`"
    };

    ASSERT_THROW(BuildGrammar(lines), std::runtime_error);
}

TEST(TestBuilding, LowRule)
{
    Vector<String> lines = {
        "()",
        "S",
        "S",
        "S",
        "S->(S)S"
    };

    ASSERT_THROW(BuildGrammar(lines), std::runtime_error);
}


TEST(TestBuilding, WrongNonterminal)
{
    Vector<String> lines = {
        "()",
        "S",
        "S",
        "S->",
        "S->(S)S",
        "A->aB"
    };

    ASSERT_THROW(BuildGrammar(lines), std::runtime_error);
}

TEST(TestBuilding, NoArrow)
{
    Vector<String> lines = {
        "()",
        "S",
        "S",
        "S ",
        "S (S)S"
    };

    ASSERT_THROW(BuildGrammar(lines), std::runtime_error);
}

TEST(TestGrammarExceptions, Intersect)
{
    ASSERT_THROW(Grammar({ 'A' }, { 'A', 'B' }, {}, 'A'), std::logic_error);
}

TEST(TestGrammarExceptions, WrongStart)
{
    ASSERT_THROW(Grammar({ 'a', 'b' }, { 'A', 'B' }, {}, 'C'), std::logic_error);
}

TEST(TestGrammarExceptions, AddRuleNonTerminalHead)
{
    Grammar grammar({ 'a', 'b' }, { 'A', 'B' }, {}, 'A');
    ASSERT_THROW(grammar.AddRule({'C', "Aab"}), std::logic_error);
}

TEST(TestGrammarExceptions, AddRuleWrongSymbol)
{
    Grammar grammar({ 'a', 'b' }, { 'A', 'B' }, {}, 'A');
    ASSERT_THROW(grammar.AddRule({ 'A', "Aac" }), std::logic_error);
}

