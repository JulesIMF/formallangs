/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    earley.h

Abstract:

    

Author / Creation date:

    JulesIMF / 10.01.23

Revision History:

--*/

#pragma once

//
// Includes / usings
//

#include <grammars.h>

//
// Definitions
//

namespace Grammars::Parsing
{
    Tree EarleyParser(Grammar const& Grammar, String Word);
}