// Copyright 2010 by Russell Miller
// Please see included license information or visit
// http://opensource.org/licenses/mit-license.php

// sendit.cc
// This is the class that will send the email message.
// It inherits from the MailSender class. 
// It has to parse the file for a from address and to 
// address.
// $Header$

#include <iostream>
#include <fstream>
#include <string>
#include "mailsender.hh"
#include "sendit.hh"
using std::cout;
using std::endl;

SendIt::SendIt(string &filename):MailSender(filename) {
    // Setting the data member that holds the file name.
    EmailFile = filename;
}

int SendIt::send(string &host_to,
                 string &envelope_from,
                 string &envelope_to)
{
    cout << host_to << endl
         << envelope_from << endl
         << envelope_to << endl;
    cout << "file:\n";
    ifstream fin;
    fin.open(EmailFile.c_str());
    string file_contents;
    while (getline(fin, file_contents) != 0) {
        cout << file_contents << endl;
    }
    fin.close();
    return 0;
}

void SendIt::parse_file() {
    string host("host");
    string env_from("envelope from");
    string env_to("envelope to");
    send(host, env_from, env_to);
}
