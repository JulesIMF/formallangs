/*++

Copyright (c) 2023 JulesIMF, MIPT

Module Name:

    grammars.cpp

Abstract:

    

Author / Creation date:

    JulesIMF / 10.01.23

Revision History:

--*/


//
// Includes / usings
//

#include <fstream>
#include <iostream>
#include <cassert>
#include <grammars.h>

//
// Definitions
//

namespace Grammars
{
    // The following code had never been used and was excluded 
    // in order to avoid its considering in the code coverage

    // size_t Tree::TotalAllocated_ = 0;

    // std::string SubtreeName_(Tree* Subtree)
    // {
    //     return "s" + std::to_string(uint64_t(Subtree));
    // }

    // void DebugTreeTraverse_(Tree* Self, std::ofstream& DotFile)
    // {
    //     if (!Self->Valid())
    //         throw std::runtime_error("Tree error: found an invalid subtree in the valid tree");
            
    //     if (Self->TreeType() == Tree::Type::Tree)
    //         DotFile << SubtreeName_(Self) << " [label = \"" <<
    //             Self->Subtrees().Head << "\", shape=\"triangle\"];\n";

    //     else
    //     {
    //         DotFile << SubtreeName_(Self) << " [label = \"" <<
    //             Self->Symbol() << "\", shape=\"underline\"];\n";

    //         return;
    //     }

    //     for (auto child : Self->Subtrees().List)
    //     {
    //         DotFile << SubtreeName_(Self) << "->" << SubtreeName_(child) << ";\n";

    //         DebugTreeTraverse_(child, DotFile);
    //     }
    // }


    // void DebugTree(Tree* Debugee, std::string Name, std::string Folder)
    // {
    //     if (!Debugee->Valid())
    //     {
    //         std::cerr << "Invalid tree!\n";
    //         return;
    //     }

    //     std::string dotFileName = Folder + "/" + Name + ".dot";
    //     std::string pngFileName = Folder + "/" + Name + ".png";

    //     std::ofstream dotFile(dotFileName);
    //     dotFile << "digraph\n{\ndpi = 300;\n";

    //     DebugTreeTraverse_(Debugee, dotFile);

    //     dotFile << "}";
    //     dotFile.close();

    //     system(("dot -Tpng " + dotFileName + " -o " + pngFileName + " > /dev/null").c_str());
    // }

    Grammar BuildGrammar(Vector<String> const& Lines)
    {
        // 0 <Terminals>
        // 1 <Nonterminals>
        // 2 <Start>
        // Then go rules in form:
        // 3+ N <...>

        Set<Terminal> alphabet;
        Set<Nonterminal> nonterminals;
        Nonterminal start = '\0';

        if (Lines.size() < 3) 
            throw std::runtime_error("Wrong grammar file: less than 3 lines");
        
        if (Lines[2].length() != 1)
            throw std::runtime_error("Wrong grammar file: 3rd line is not a single-shar");

        for (auto c : Lines[0])
            alphabet.insert(c);

        for (auto c : Lines[1])
            nonterminals.insert(c);

        start = Lines[2].front();
        
        Grammar grammar(alphabet, nonterminals, {}, start);
        for (int i = 3; i != Lines.size(); i++)
        {
            if (Lines[i].length() < 3)
                throw std::runtime_error("Wrong grammar file: rule line length is less than 3");

            if (nonterminals.find(Lines[i][0]) == nonterminals.end())
                throw std::runtime_error("Wrong grammar file: rule line starts from nonexistiant nonterminal");
            
            if (Lines[i][1] != '-' || Lines[i][2] != '>')
                throw std::runtime_error("Wrong grammar file: arrow not found");

            auto head = Lines[i].front();
            auto body = Lines[i].substr(3);
            grammar.AddRule({ head, body });
        }

        return grammar;
    }
}