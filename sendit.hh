// Copyright 2010 by Russell Miller
// Please see included license information or visit
// http://opensource.org/licenses/mit-license.php

// sendit.hh
// This is the class that will send the email message.
// $Header$

#include "mailsender.hh"

class SendIt: public MailSender
{

public:

    SendIt(string &filename);

    virtual int send(string &host_to,
                     string &envelope_from,
                     string &envelope_to);
    void parseArgs(); 
};
