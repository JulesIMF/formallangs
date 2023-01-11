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
#include <iostream>
#include <fstream>
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

Grammars::Vector<Grammars::String> GetLines(char const* FileName)
{
    std::ifstream fin(FileName);
    if (!fin.is_open())
        throw std::runtime_error("Nonexistant grammar file!");
    
    Grammars::String s;
    Grammars::Vector<Grammars::String> lines;

    while (true)
    {
        if (fin.eof())
            break;
        
        std::getline(fin, s);
        lines.push_back(s);
    }

    fin.close();

    std::cout << "Readed grammar lines:\n";
    for (auto const& s : lines)
        std::cout << "\t" << s << "\n";
    
    std::cout << "\n";
        
    return lines;
}

int main(int argc, char const* argv[])
{
    size_t const fileIdx = 1,
                 wordIdx = 2;
    if (argc != 3)
    {
        std::cerr << "Wrong args! Specify grammar file and word in cmd arguments!\n";
        return -1;
    }

    try
    {
        auto lines = GetLines(argv[1]);
        auto grammar = Grammars::BuildGrammar(lines);
        grammar.Print(std::cout);

        Grammars::String word(argv[2]);
        std::cout << "\nParsed word: " << word << "\n";

        std::cout << (
            (Grammars::Parsing::EarleyParser(grammar, word)) ?
            "Derived\n" : 
            "Not derived\n"
            );
    }

    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }   
}
