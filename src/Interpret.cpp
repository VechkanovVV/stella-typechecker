#include <iostream>

#include "Eval.h"
#include "TypeCheck.h"
#include "Stella/Absyn.H"
#include "Stella/Printer.H"
#include "Stella/Parser.H"
#include "Stella/ParserError.H"

void usage()
{
    std::cerr << "Usage: Call with one of the following argument combinations:" << std::endl;
    std::cerr << "\t(no arguments)\t\tTypecheck input from stdin." << std::endl;
    std::cerr << "\ttypecheck FILE\t\tParse and typecheck the content of FILE." << std::endl;
    std::cerr << "\tinterpret FILE\t\tParse the content of FILE, then interpret the program with input from stdin." <<
        std::endl;
    std::cerr << "\thelp\t\tShow this message." << std::endl;
    std::cerr << "\tFILE\t\t\tInterpret the program in FILE with input from stdin (default mode)." << std::endl;
}

enum MODE { TYPECHECK, INTERPRET };

int main(const int argc, char** argv)
{
    FILE* input;
    std::string filename;

    MODE mode = TYPECHECK;

    if (argc > 1)
    {
        if (strcmp(argv[1], "help") == 0)
        {
            usage();
            exit(1);
        }
        else if (strcmp(argv[1], "typecheck") == 0)
        {
            if (argc <= 2)
            {
                usage();
                exit(1);
            }
            filename = argv[2];
        }
        else if (strcmp(argv[1], "interpret") == 0)
        {
            mode = INTERPRET;
            if (argc <= 2)
            {
                usage();
                exit(1);
            }
            filename = argv[2];
        }
        else
        {
            mode = INTERPRET;
            filename = argv[1];
        }
    }

    if (!filename.empty())
    {
        input = fopen(filename.c_str(), "r");
        if (!input)
        {
            std::cerr << "Cannot read file: " << filename << std::endl;;
            exit(1);
        }
    }
    else
    {
        input = stdin;
    }

    Stella::Program* prog;
    try
    {
        prog = Stella::pProgram(input);
        std::cout << "Parsing succeeded" << std::endl;
    }
    catch (Stella::parse_error& ex)
    {
        std::cerr << "Parsing failed: " << ex.what() << std::endl;
        exit(1);
    }
    if (!filename.empty())
        fclose(input);

    Stella::typecheckProgram(prog);

    if (mode == INTERPRET)
    {
        size_t intInput;
        std::cin >> intInput;

        Stella::Expr* inputExpr = new Stella::ConstInt(0);
        for (size_t i = 0; i < intInput; i++)
        {
            inputExpr = new Stella::Succ(inputExpr);
        }

        auto* result = Stella::evalMainWith(prog, inputExpr);
        auto printTree = Stella::PrintAbsyn();
        std::cout << printTree.print(result) << std::endl;
    }

    return 0;
}
