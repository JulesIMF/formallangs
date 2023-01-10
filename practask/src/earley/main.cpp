/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    main.cpp

Abstract:

    Main Earley module.

Author / Creation date:

    JulesIMF / 10.01.23

Revision History:

--*/


//
// Includes / usings
//

#include <earley/earley.h>

//
// Definitions
//


/*
    Grammar example:
        A = { +, *, n, (, ) }
        N = { E, T, F }
        S = E

        Rules:
            E → T
            E → E+T
            T → F
            T → T*F
            F → n
            F → (E)

*/
