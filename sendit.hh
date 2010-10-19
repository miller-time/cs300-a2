// Copyright 2010 by Russell Miller
// Please see included license information or visit
// http://opensource.org/licenses/mit-license.php

// sendit.hh
// This is the class that will send the email message.
// $Header$

#ifndef SENDIT_H
#define SENDIT_

#include "mailsender.hh"

class SendIt: public MailSender
{

protected:

    string EmailFile;
    string Message;

public:

    SendIt(string &filename);

    int parse_file(); 
    virtual int send(string &host_to,
                     string &envelope_from,
                     string &envelope_to);
};
#endif
