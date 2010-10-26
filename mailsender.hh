// Copyright 2010 by Russell Miller
// Please see included license information in file
// COPYING or visit
// http://opensource.org/licenses/mit-license.php

#ifndef _WF_MAILSENDER_HH
#define _WF_MAILSENDER_HH

// mailsender.hh
// This is the class that will send the email message.
// $Header$


#include <string>

using namespace std;

class MailSender
{

public:
    // Set up the class for sending the given file as email.

    // The file should contain an RFC-822 formatted email

    // message, whose correctness will be assumed by this object.

    MailSender(string &filename);

    // Actually send the email to the given host

    // with the given source and recipient. Returns

    // 0 on success, and -1 on failure. In the

    // case of failure, the global variable errno

    // will be set to indicate a failure code.
    

    // renamed send because of clash with socket send

    virtual int send(string &host_to,
                     string &envelope_from,
                     string &envelope_to) = 0;

};
#endif
