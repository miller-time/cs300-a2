// Copyright 2010 by Russell Miller
// Please see included license information in file
// COPYING or visit
// http://opensource.org/licenses/mit-license.php

#ifndef _WF_SENDIT_HH
#define _WF_SENDIT_HH

// sendit.hh
// This is the class that will send the email message.
// $Header$

#include "mailsender.hh"

class SendIt: public MailSender
{

public:

    // constructor
    SendIt(string &filename);
    // destructor
    virtual ~SendIt();

    // public functions
    int parse_file();
    virtual int send(string &host_to,
                     string &envelope_from,
                     string &envelope_to);

private:

    // data members
    string EmailFile;   // name of file
    string Message;     // message contents

    // utility functions
    string read_and_write(int, string &);
    string find_addr(string &, string);
    void sanitize_addr(string &);

};
#endif
