/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    tests.cpp

Abstract:

    Tests definitions.

Author / Creation date:

    JulesIMF / 05.11.22

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

TEST(TestEarley, Brackets)
{
    Vector<String> lines = {
        "()",
        "S",
        "S",
        "S->",
        "S->(S)S"
    };

    Grammar grammar = BuildGrammar(lines);

    ASSERT_TRUE(Parsing::EarleyParser(grammar, 
        ""));

    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "()"));
    
    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "(())"));
    
    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "()()"));

    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "(()())()()((()()))"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        ")))"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "))())()"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "(()"));

    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "()()()(()"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "((())"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "dacsadcsdc"));    
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "S"));    
}

TEST(TestEarley, Arithmetics)
{
    Vector<String> lines = {
        "+*n()",
        "ETF",
        "E",
        "E->T",
        "E->E+T",
        "T->F",
        "T->T*F",
        "F->n",
        "F->(E)"
    };

    Grammar grammar = BuildGrammar(lines);

    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "(n*n+n)*(n+n)+n+n*n*n+n*(n+n)"));
    
    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "n+n"));
    
    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "n*n"));
    
    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "(n)"));
    
    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "(n+n)*n"));
    
    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "(((n)))"));
    
    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "(n*n+n)*(n+n)+n+n*n*n+n*(n+n)"));
    
    ASSERT_TRUE(Parsing::EarleyParser(grammar,
        "n+n+n+n+n*n*n*n*n*(((((n*n*n+n+n*n*n+n)))))+n+n"));    
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "nn"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "+n"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "*n"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "()"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        ""));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "n-n*n"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "n(n)"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "^^^"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "n*(n)(n)"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "()()(())"));
    
    ASSERT_FALSE(Parsing::EarleyParser(grammar,
        "S"));
}

TEST(TestEarley, DollarSymbolThrow)
{
    Vector<String> lines = {
        "+*n()",
        "$ETF",
        "E",
        "E->T",
        "E->E+T",
        "T->F",
        "T->T*F",
        "F->n",
        "F->(E)"
    };

    Grammar grammar = BuildGrammar(lines);

    ASSERT_THROW(Parsing::EarleyParser(grammar, "n+n*n"), std::logic_error);
}
