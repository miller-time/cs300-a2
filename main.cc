// Copyright 2010 by Russell Miller
// Please see included license information in file
// COPYING or visit
// http://opensource.org/licenses/mit-license.php

// main.cc
// This is the entry point for the ModestMail program
// ModestMail will expect one argument on the command line.
// $Header$

#include <iostream>
#include <string>
#include <fstream>
#include "sendit.hh"
using namespace std;


// MISSING_OPERAND
// This function is called if the user does not specify an
// argument on the command line
void missing_operand() {
    cout << "modestmail: missing operand\n"
         << "Try `modestmail --help' for more information.\n";
}


// PARSE_ARG
// This function directs traffic based on commands specified
// on the command line. If the user types the help command it
// will print the HELP file, and anything else will be treated
// like a filename.
void parse_arg(char* arg) {
    ifstream fin;       // stream file will open in
    string help ("--help");
    // if user typed --help on command line
    if (help.compare(arg) == 0)
    {
        // print the help file
        fin.open("HELP"); //check return value <---------------
        string help_contents;
        while (getline(fin, help_contents) != 0) {
            // check for return -1  <-------------------------
            cout << help_contents << endl;
        }
        fin.close();    // check return value <---------------
    } else {
        // create SendIt object with specified filename
        string filename (arg);
        SendIt newmail(filename);
        newmail.parse_file();
    }
}


// MAIN
// Simply check that the user has supplied one argument on the
// command line and use PARSE_ARG to inspect it.
int main (int argc, char **argv) {
    if (argc != 2) {
        missing_operand();
    } else {
        parse_arg(argv[1]);
    }
}
