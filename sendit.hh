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

public:

    // constructor
    SendIt(string &filename);

    // public functions
    int parse_file();
    // renamed send because it clashes with socket send
    virtual int send_message(string &host_to,
                             string &envelope_from,
                             string &envelope_to);

protected:

    // data members
    string EmailFile;   // name of file
    string Message;     // message contents

private:
    
    // private worker functions
    string find_addr(string &, string);
    string sanitize_addr(string &);

};
#endif
