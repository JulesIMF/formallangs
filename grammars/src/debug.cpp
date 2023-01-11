/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    debug.cpp

Abstract:

    

Author / Creation date:

    JulesIMF / 11.01.23

Revision History:

--*/


//
// Includes / usings
//

#include <grammars.h>

//
// Definitions
//

void TestTreeDebug()
{
    // (())()
    Grammars::Tree* tree = Grammars::Tree::AllocTree({ 'S',
    {
        Grammars::Tree::AllocTerminal('('),
        Grammars::Tree::AllocTree({'S',
        {
            Grammars::Tree::AllocTerminal('('),
            Grammars::Tree::AllocTerminal(Grammars::Eps),
            Grammars::Tree::AllocTerminal(')'),
            Grammars::Tree::AllocTree({'S',
            {
                Grammars::Tree::AllocTerminal('('),
                Grammars::Tree::AllocTerminal(Grammars::Eps),
                Grammars::Tree::AllocTerminal(')'),
                Grammars::Tree::AllocTerminal(Grammars::Eps),
            }
            }),
        }
        }),
        Grammars::Tree::AllocTerminal(')'),
        Grammars::Tree::AllocTree({'S',
        {
            Grammars::Tree::AllocTerminal('('),
            Grammars::Tree::AllocTerminal(Grammars::Eps),
            Grammars::Tree::AllocTerminal(')'),
            Grammars::Tree::AllocTerminal(Grammars::Eps),
        }
        }),
    } });

    DebugTree(tree);
    delete tree;
}