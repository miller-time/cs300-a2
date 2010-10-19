// Copyright 2010 by Russell Miller
// Please see included license information or visit
// http://opensource.org/licenses/mit-license.php

// main.cc
// This is the entry point for the ModestMail program
// $Header$

#include <iostream>
#include <string>
#include <fstream>
#include "sendit.hh"
using namespace std;

void missing_operand() {
    cout << "modestmail: missing operand\n"
         << "Try `modestmail --help' for more information.\n";
}

void parse_arg(char* arg) {
    ifstream fin;
    string help ("--help");
    if (help.compare(arg) == 0)
    {
        fin.open("HELP");
        string help_contents;
        while (getline(fin, help_contents) != 0) {
            cout << help_contents << endl;
        }
        fin.close();
    } else {
    // open a file - check if it exists etc..
        //string filename ("HELP");
        string filename (arg);
        SendIt newmail(filename);
        newmail.parse_file();
    }
}

int main (int argc, char **argv) {
    if (argc != 2) {
        missing_operand();
    } else {
        parse_arg(argv[1]);
    }
}
