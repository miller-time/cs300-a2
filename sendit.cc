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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "mailsender.hh"
#include "sendit.hh"
using std::cout;
using std::endl;


// CONSTRUCTOR
SendIt::SendIt(string &filename):MailSender(filename) {
    // Setting the data member that holds the file name.
    EmailFile = filename;
}


// SEND_MESSAGE
// This function is inherited from MailSender and is designed
// to open a socket, and send the email message to the server
// the socket is communicating with.

// CHECK FOR VALIDITY                                <----------------------

// Data members will already be set by parse_file before this
// function is called.
int SendIt::send_message(string &host_to,
                         string &envelope_from,
                         string &envelope_to)
{
    int socket_descriptor;
    int status;
    addrinfo *host_addrinfo;
    // Make the call to fill the addrinfo struct
    if ((status = getaddrinfo(host_to.c_str(), "25", NULL, &host_addrinfo)) != 0) {
        cerr << gai_strerror(status) << endl;
        return 1;
    }
    // Get the socket descriptor
    socket_descriptor = socket(host_addrinfo->ai_family,
                               host_addrinfo->ai_socktype,
                               host_addrinfo->ai_protocol);
    // Connect to the socket
    connect(socket_descriptor, host_addrinfo->ai_addr, host_addrinfo->ai_addrlen);


    // socket ready to connect, now need to begin communicating with
    // the SMTP server. this happens through several individual
    // sends and receives.


    int len, bytes_sent;
    // Initialize variables used to contact SMTP server
    string smtp_msg("HELO ");
    smtp_msg += host_to + "\r\n";
    len = smtp_msg.length();
    cout << smtp_msg;
    // Make initial contact to the SMTP Server
    bytes_sent = send(socket_descriptor, smtp_msg.c_str(), len, 0);
    char buf[999];
    int bytes_rcvd;
    // Check for Server's response
    bytes_rcvd = recv(socket_descriptor, buf, 999, 0);
    buf[3] = '\0'; // chops off everything but the status
    cout << buf << endl;
    string two20("220");
    // Do not continue if server closed connection
    if (two20.compare(buf) != 0) {
        cerr << "Server closed connection\n";
        // should add the close() and free here                   <----------
        return 1;
    }


    // Reinitialize variables for MAIL message
    smtp_msg = "MAIL FROM:<";
    smtp_msg += envelope_from + ">\r\n";
    len = smtp_msg.length();
    cout << smtp_msg;
    // Send message
    bytes_sent = send(socket_descriptor, smtp_msg.c_str(), len, 0);
    // Check response
    buf[3] = ' '; // don't want recv to stop after 3 chars
    bytes_rcvd = recv(socket_descriptor, buf, 999, 0);
    buf[3] = '\0'; // chops off everything but the status
    cout << buf << endl;
    string two50("250");
    // Do not continue if server closed connection
    if (two50.compare(buf) != 0) {
        cerr << "Server closed connection\n";
        return 1;
    }

   
    // Reinitializing for RCPT message
    smtp_msg = "RCPT TO:<";
    smtp_msg += envelope_to + ">\r\n";
    len = smtp_msg.length();
    cout << smtp_msg;
    // Send message
    bytes_sent = send(socket_descriptor, smtp_msg.c_str(), len, 0);
    // Check response
    buf[3] = ' '; // don't want recv to stop after 3 chars
    bytes_rcvd = recv(socket_descriptor, buf, 999, 0);
    buf[3] = '\0'; // chops off everything but the status
    cout << buf << endl;
    // Do not continue if server closed connection
    if (two50.compare(buf) != 0) {
        cerr << "Server closed connection\n";
        return 1;
    }


    // Reinitializing for DATA message
    smtp_msg = "DATA\r\n";
    len = smtp_msg.length();
    cout << smtp_msg;
    // Send message
    bytes_sent = send(socket_descriptor, smtp_msg.c_str(), len, 0);
    // Check response
    buf[3] = ' '; // don't want recv to stop after 3 chars
    bytes_rcvd = recv(socket_descriptor, buf, 999, 0);
    buf[3] = '\0'; // chops off everything but the status
    cout << buf << endl;
    string three54("354");
    // Do not continue if server closed connection
    if (three54.compare(buf) != 0) {
        cerr << "Server closed connection\n";
        return 1;
    }


    // Reinitializing for actual sending of message
    smtp_msg = Message + "\r\n.\r\n";
    len = smtp_msg.length();
    cout << smtp_msg;
    // Send message
    bytes_sent = send(socket_descriptor, smtp_msg.c_str(), len, 0);
    // Check response
    buf[3] = ' '; // don't want recv to stop after 3 chars
    bytes_rcvd = recv(socket_descriptor, buf, 999, 0);
//    buf[3] = '\0'; // chops off everything but the status
    cout << buf << endl;
    // Do not continue if server closed connection
    if (two50.compare(buf) != 0) {
        cerr << "Server closed connection\n";
        return 1;
    }





    // free the linked list of server info
    close(socket_descriptor);
    freeaddrinfo(host_addrinfo);
//    cout << "Message sending complete. Bytes sent: " << bytes_sent << endl;
    return 0;
}


// PARSE_FILE
// This function is designed to open the file and pick all of
// the information out of it. It gets the envelope information, 
// as well as storing the message as a string into data members.
int SendIt::parse_file() {
    // variables that will be passed to send_message()
    string host, env_from, env_to;
    // stream variable for opening email file
    ifstream fin;
    fin.open(EmailFile.c_str());
    // variable for reading each line of data from file
    string line;
    while (getline(fin, line) != 0) {
        // SendIt's data member building up its data line by line
        // might need to check for the lone newline and add the \r   <---------
        // also might want to check for lone .'s to avoid errors     <---------
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
    send_message(host, env_from, env_to);
    return 0;
}
