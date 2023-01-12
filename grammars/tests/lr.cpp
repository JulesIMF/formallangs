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
#include <parsing/lr.h>

//
// Definitions
//

using namespace Grammars;

TEST(TestLR, Brackets)
{
    Vector<String> lines = {
        "()",
        "S",
        "S",
        "S->",
        "S->(S)S"
    };

    Grammar grammar = BuildGrammar(lines);
    Parsing::LR lr(grammar);

    ASSERT_TRUE(lr.Derives(""));

    ASSERT_TRUE(lr.Derives("()"));
    
    ASSERT_TRUE(lr.Derives("(())"));
    
    ASSERT_TRUE(lr.Derives("()()"));

    ASSERT_TRUE(lr.Derives("(()())()()((()()))"));
    
    ASSERT_FALSE(lr.Derives(")))"));
    
    ASSERT_FALSE(lr.Derives("))())()"));
    
    ASSERT_FALSE(lr.Derives("(()"));

    ASSERT_FALSE(lr.Derives("()()()(()"));
    
    ASSERT_FALSE(lr.Derives("((())"));
    
    ASSERT_FALSE(lr.Derives("dacsadcsdc"));    
    
    ASSERT_FALSE(lr.Derives("S"));    
}

TEST(TestLR, Arithmetics)
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
    Parsing::LR lr(grammar);

    ASSERT_TRUE(lr.Derives("(n*n+n)*(n+n)+n+n*n*n+n*(n+n)"));
    
    ASSERT_TRUE(lr.Derives("n+n"));
    
    ASSERT_TRUE(lr.Derives("n*n"));
    
    ASSERT_TRUE(lr.Derives("(n)"));
    
    ASSERT_TRUE(lr.Derives("(n+n)*n"));
    
    ASSERT_TRUE(lr.Derives("(((n)))"));
    
    ASSERT_TRUE(lr.Derives("(n*n+n)*(n+n)+n+n*n*n+n*(n+n)"));
    
    ASSERT_TRUE(lr.Derives("n+n+n+n+n*n*n*n*n*(((((n*n*n+n+n*n*n+n)))))+n+n"));    
    
    ASSERT_FALSE(lr.Derives("nn"));
    
    ASSERT_FALSE(lr.Derives("+n"));
    
    ASSERT_FALSE(lr.Derives("*n"));
    
    ASSERT_FALSE(lr.Derives("()"));
    
    ASSERT_FALSE(lr.Derives(""));
    
    ASSERT_FALSE(lr.Derives("n-n*n"));
    
    ASSERT_FALSE(lr.Derives("n(n)"));
    
    ASSERT_FALSE(lr.Derives("^^^"));
    
    ASSERT_FALSE(lr.Derives("n*(n)(n)"));
    
    ASSERT_FALSE(lr.Derives("()()(())"));
    
    ASSERT_FALSE(lr.Derives("S"));
}

TEST(TestLR, DollarSymbolThrow)
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

    ASSERT_THROW(Parsing::LR{grammar}, std::logic_error);
}

TEST(TestLR, SharpSymbolThrow)
{
    Vector<String> lines = {
        "+*n()",
        "#ETF",
        "E",
        "E->T",
        "E->E+T",
        "T->F",
        "T->T*F",
        "F->n",
        "F->(E)"
    };

    Grammar grammar = BuildGrammar(lines);

    ASSERT_THROW(Parsing::LR{ grammar }, std::logic_error);
}


TEST(TestLRComponents, First)
{
    Vector<String> lines = {
        "+*()n",
        "ETFDU",
        "E",
        "E->TD",
        "D->+TD",
        "D->",
        "T->FU",
        "U->*FU",
        "U->",
        "F->(E)",
        "F->n"
    };

    Grammar grammar = BuildGrammar(lines);
    Parsing::LR lr(grammar);

    ASSERT_EQ(lr.First({'F'}), Set<Terminal>({'(', 'n'}));
    ASSERT_EQ(lr.First({'T'}), Set<Terminal>({'(', 'n'}));
    ASSERT_EQ(lr.First({'E'}), Set<Terminal>({'(', 'n'}));

    ASSERT_EQ(lr.First({'D'}), Set<Terminal>({'+'}));
    ASSERT_EQ(lr.First({'U'}), Set<Terminal>({'*'}));
    ASSERT_EQ(lr.First("DUF"), Set<Terminal>({'+', '*', '(', 'n'}));
    ASSERT_EQ(lr.First("UF"), Set<Terminal>({'*', '(', 'n'}));
}