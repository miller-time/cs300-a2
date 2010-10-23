// Copyright 2010 by Russell Miller
// Please see included license information in file
// COPYING or visit
// http://opensource.org/licenses/mit-license.php

// sendit.hh
// This is the class that will send the email message.
// $Header$

#ifndef SENDIT_H
#define SENDIT_

#include "mailsender.hh"

class SendIt: public MailSender
{

public:

    // constructor
    SendIt(string &filename);

    // public functions
    int parse_file();
    virtual int send(string &host_to,
                     string &envelope_from,
                     string &envelope_to);

protected:

    // data members
    string EmailFile;   // name of file
    string Message;     // message contents

private:

    // utility functions
    string read_and_write(int, string &);
    string find_addr(string &, string);
    void sanitize_addr(string &);

};
#endif
