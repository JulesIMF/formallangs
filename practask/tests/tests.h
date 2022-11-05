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


class TestSolution : public ::testing::Test 
{
};

class TestExceptions : public ::testing::Test
{
    TestExceptions();
    ~TestExceptions();
};

class TestDebug : public ::testing::Test
{
};