/*++

Copyright (c) 2022 JulesIMF, MIPT

Module Name:

    Main.cpp

Abstract:

    Entry point definition.

Author / Creation date:

    JulesIMF / 05.11.22

Revision History:

--*/


//
// Includes / usings
//

#include <iostream>
#include <Task.h>

//
// Definitions
//

int main()
{
    std::string regexp, word;
    std::cin >> regexp >> word;
    try
    {
        AlphabetType alphabet = { 'a', 'b', 'c' };
        for (size_t idx = 0; idx != word.length(); idx++)
            if (alphabet.find(word[idx]) == alphabet.end())
                throw std::runtime_error(
                    "Invalid symbol \'" +
                    std::string(1, word[idx]) +
                    "\' (word_idx = " +
                    std::to_string(idx) + ")");

        auto ans = SolveTask13(regexp, word, alphabet);
        std::cout << "Task 13 answer is " << ans << "\n";
    }
    
    catch(const std::exception& e)
    {
        std::cerr << "Error!" << e.what() << '\n';
    }
}