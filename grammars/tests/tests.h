/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    tests.h

Abstract:

    Tests suites declaration.

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/

#pragma once

//
// Includes / usings
//

#include <gtest/gtest.h>

//
// Definitions
//


class TestBuilding : public ::testing::Test
{
    TestBuilding();
    ~TestBuilding();
};

class TestGrammarExceptions : public ::testing::Test
{
    TestGrammarExceptions();
    ~TestGrammarExceptions();
};

class TestEarley : public ::testing::Test
{
    TestEarley();
    ~TestEarley();
};

class TestDebug : public ::testing::Test
{
};