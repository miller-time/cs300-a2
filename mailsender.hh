// Copyright 2010 by Russell Miller
// Please see included license information or visit
// http://opensource.org/licenses/mit-license.php

// mailsender.hh
// This is the class that will send the email message.
// $Header$

#ifndef MAILSENDER_H
#define MAILSENDER_H

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

    virtual int send(string &host_to,
                     string &envelope_from,
                     string &envelope_to) = 0;

};
#endif
