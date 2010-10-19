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

// This function is inherited from MailSender and is designed
// to open a socket, and send the email message to the server
// the socket is communicating with.

// CHECK FOR VALIDITY

// Data members will already be set by parse_file before this
// function is called.
int SendIt::send(string &host_to,
                 string &envelope_from,
                 string &envelope_to)
{
    cout << host_to << endl
         << envelope_from << endl
         << envelope_to << endl;
    return 0;
}

// This function is designed to open the file and pick all of
// the information out of it. It gets the envelope information, 
// as well as storing the message as a string into data members.
int SendIt::parse_file() {
    string host("host");
    string env_from("from");
    string env_to;
    ifstream fin;
    fin.open(EmailFile.c_str());
    string line;
    while (getline(fin, line) != 0) {
        Message += line += "\n";
    }
    fin.close(); // check return value
    // Entire message file is now read. Need to search the
    // "Message" data member string for certain substrings.
    string to_address("To: ");
    string at_symbol("@");
    string from_address("From: ");
    // Looking for the to address
    // start_address is the index of the substring "To: "
    size_t start_address = Message.find(to_address);
    // This cuts off everything before "To: " (length 4)
    string msg_after_str = Message.substr(start_address + 4);
    // This measures how long the string goes until the newline
    size_t address_length = msg_after_str.find("\n");
    if (start_address != string::npos &&
        address_length != string::npos)
    {
            // This assigns the resulting email address to the data member
            env_to = Message.substr(start_address + 4, address_length);
    } else {
            cout << "To address not found in file.\n";
            return 1;
    }
    // Now need to find the host. This is just the tail of the
    // to address
    start_address = env_to.find(at_symbol);
    if (start_address != string::npos)
        host = env_to.substr(start_address + 1);
    else {
        cout << "To address not formatted correctly.\n";
        return 1;
    }
    // And finally getting out the from address the same way
    start_address = Message.find(from_address);
    msg_after_str = Message.substr(start_address + 6);
    address_length = msg_after_str.find("\n");
    if (start_address != string::npos &&
        address_length != string::npos)
    {
            env_from = Message.substr(start_address + 6, address_length);
    } else {
            cout << "From address not found in file.\n";
            return 1;
    }
    // OK! Now we should have all the necessary pieces!
    send(host, env_from, env_to);
    return 0;
}
