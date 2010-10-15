// Copyright 2010 by Russell Miller
// Please see included license information or visit
// http://opensource.org/licenses/mit-license.php

// main.cpp
// This is the entry point for the ModestMail program
// $Header$

#include <iostream>
#include <string>
#include <fstream>
using namespace std;

void missingOperand() {
    cout << "modestmail: missing operand\n"
         << "Try `modestmail --help' for more information.\n";
}

void parseArg(char* arg) {
    ifstream fin;
    string help ("--help");
    if (help.compare(arg) == 0)
    {
        fin.open("HELP");
        string help_contents;
        while (getline(fin, help_contents) != 0) {
            cout << help_contents << endl;
        }
    } else {
    // open a file - check if it exists etc..
    }
}

int main (int argc, char **argv) {
    if (argc != 2) {
        missingOperand();
    } else {
        parseArg(argv[1]);
    }
}
