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
#include <Task.h>
#include <Automaton.h>
#include <Regexp.h>

//
// Definitions
//

/*
    Tests:
        ab+c.aba.*.bac.+.+*1+ babc          2
        acb..bab.c.*.ab.ba.+.+*a. abbaa     4
    Normal regexps:
        ((a + b)c + a(ba)*(b + ac))*
        (acb + (b(abc)*)(ab + ba))*a
*/

TEST(TestSolution, FirstRegexp) 
{
    ASSERT_EQ(SolveTask13("ab+c.aba.*.bac.+.+*1+", "babc", {'a', 'b', 'c'}), 2);
    ASSERT_EQ(SolveTask13("ab+c.aba.*.bac.+.+*1+", "aaaa", { 'a', 'b', 'c' }), 0);
    ASSERT_EQ(SolveTask13("ab+c.aba.*.bac.+.+*1+", "", { 'a', 'b', 'c' }), 0);
    ASSERT_EQ(SolveTask13("ab+c.aba.*.bac.+.+*1+", "bcabababacbc", { 'a', 'b', 'c' }), 12);
    ASSERT_EQ(SolveTask13("ab+c.aba.*.bac.+.+*1+", "ccccbcabababacbc", { 'a', 'b', 'c' }), 12);
}

TEST(TestSolution, SecondRegexp) 
{
    ASSERT_EQ(SolveTask13("acb..bab.c.*.ab.ba.+.+*a.", "abbaa", { 'a', 'b', 'c' }), 4);
    ASSERT_EQ(SolveTask13("acb..bab.c.*.ab.ba.+.+*a.", "aaaa", { 'a', 'b', 'c' }), 1);
    ASSERT_EQ(SolveTask13("acb..bab.c.*.ab.ba.+.+*a.", "bbbb", { 'a', 'b', 'c' }), 0);
    ASSERT_EQ(SolveTask13("acb..bab.c.*.ab.ba.+.+*a.", "acbacbbabbabcbabbaacba", { 'a', 'b', 'c' }), 22);
    ASSERT_EQ(SolveTask13("acb..bab.c.*.ab.ba.+.+*a.", "abcbababcbacbbcabcaba", { 'a', 'b', 'c' }), 4);
}

TEST(TestDebug, Plug)
{
    Automaton automaton = ParseReversePolishRegexp("ab+c.aba.*.bac.+.+*1+", {'a', 'b', 'c'});
    DebugAutomaton(automaton, "automaton", "/tmp");
}

TestExceptions::TestExceptions()
{
    Automaton::StartUsing();
}

TestExceptions::~TestExceptions()
{
    Automaton::EndUsing();
}

TEST(TestExceptions, Concat)
{
    ASSERT_THROW(ParseReversePolishRegexp(".", {'a', 'b'}), std::runtime_error);
    ASSERT_THROW(ParseReversePolishRegexp("a.", { 'a', 'b' }), std::runtime_error);
    ASSERT_THROW(ParseReversePolishRegexp("ab.b+.", { 'a', 'b' }), std::runtime_error);
}

TEST(TestExceptions, Union)
{
    ASSERT_THROW(ParseReversePolishRegexp("+", { 'a', 'b' }), std::runtime_error);
    ASSERT_THROW(ParseReversePolishRegexp("a+", { 'a', 'b' }), std::runtime_error);
    ASSERT_THROW(ParseReversePolishRegexp("ab+b.+", { 'a', 'b' }), std::runtime_error);
}

TEST(TestExceptions, Kleene)
{
    ASSERT_THROW(ParseReversePolishRegexp("***", { 'a', 'b' }), std::runtime_error);
}

TEST(TestExceptions, InvalidSymbol)
{
    ASSERT_THROW(ParseReversePolishRegexp("ab+c.", { 'a', 'b' }), std::runtime_error);
    ASSERT_THROW(ParseReversePolishRegexp("12+3+", { 'a', 'b' }), std::runtime_error);
    ASSERT_NO_THROW(ParseReversePolishRegexp("ab+ aba..\t+\t*\n", { 'a', 'b' }));
}

TEST(TestExceptions, EmptyRegexp)
{
    ASSERT_THROW(ParseReversePolishRegexp("", { 'a', 'b' }), std::runtime_error);
}

TEST(TestExceptions, ExtraExpressions)
{
    ASSERT_THROW(ParseReversePolishRegexp("ab+bb.", { 'a', 'b' }), std::runtime_error);
    ASSERT_THROW(ParseReversePolishRegexp("ababa", { 'a', 'b' }), std::runtime_error);
    ASSERT_THROW(ParseReversePolishRegexp("acb..bab.c.*.ab.", { 'a', 'b' }), std::runtime_error);
}
