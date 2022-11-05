/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    main.cpp

Abstract:

    Tests entry point definition.

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/


//
// Includes / usings
//

#include <gtest/gtest.h>

//
// Definitions
//

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}