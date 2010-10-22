// Copyright 2010 by Russell Miller
// Please see included license information in file
// COPYING or visit
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

// READ_AND_WRITE
// This function makes the repeated calls to read() and
// write() and returns the 3-digit response.
string SendIt::read_and_write(int sd, string &msg) {
    size_t bytes_sent; // used to compare with message length
    char buf[999];      // used to store response from server
    cout << msg;
    bytes_sent = write(sd, msg.c_str(), msg.length());
    // warn about bytes_sent mismatch
    if (bytes_sent != msg.length()) {
        cout << "Warning: Message not fully transmitted.\n";
    }
    read(sd, buf, 999);
    buf[3] = '\0';      // force-shorted response to 3 digits
    cout << buf << endl;
    return (string(buf));
}


// SEND
// This function is inherited from MailSender and is designed
// to open a socket, and send the email message to the server
// the socket is communicating with.

// CHECK FOR VALIDITY                                <----------------------

// Data members will already be set by parse_file before this
// function is called.
int SendIt::send(string &host_to,
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


    // Initialize HELO message
    string smtp_msg("HELO ");
    smtp_msg += host_to + "\r\n";
    // Use utility function to contact server
    string response = read_and_write(socket_descriptor, smtp_msg);
    // Do not continue if server closed connection
    if (response.compare("220") != 0) {
        cerr << "Server closed connection\n";
        close(socket_descriptor);
        freeaddrinfo(host_addrinfo);
        return 1;
    }


    // Set MAIL FROM message
    smtp_msg = "MAIL FROM:<";
    smtp_msg += envelope_from + ">\r\n";
    // Use utility function to contact server
    response = read_and_write(socket_descriptor, smtp_msg);
    // Do not continue if server closed connection
    if (response.compare("250") != 0) {
        cerr << "Server closed connection\n";
        close(socket_descriptor);
        freeaddrinfo(host_addrinfo);
        return 1;
    }


    // Set RCPT TO message
    smtp_msg = "RCPT TO:<";
    smtp_msg += envelope_to + ">\r\n";
    // Use utility function to contact server
    response = read_and_write(socket_descriptor, smtp_msg);
    // Do not continue if server closed connection
    if (response.compare("250") != 0) {
        cerr << "Server closed connection\n";
        close(socket_descriptor);
        freeaddrinfo(host_addrinfo);
        return 1;
    }


    // Set DATA message
    smtp_msg = "DATA\r\n";
    // Use utility function to contact server
    response = read_and_write(socket_descriptor, smtp_msg);
    // Do not continue if server closed connection
    if (response.compare("354") != 0) {
        cerr << "Server closed connection\n";
        close(socket_descriptor);
        freeaddrinfo(host_addrinfo);
        return 1;
    }


    // Set actual message
    smtp_msg = Message + "\r\n.\r\n";
    // Use utility function to contact server
    response = read_and_write(socket_descriptor, smtp_msg);
    // Do not continue if server closed connection
    if (response.compare("250") != 0) {
        cerr << "Server closed connection\n";
        close(socket_descriptor);
        freeaddrinfo(host_addrinfo);
        return 1;
    }


    // Set QUIT message
    smtp_msg = "QUIT\r\n";
    // Use utility function to contact server
    response = read_and_write(socket_descriptor, smtp_msg);

    // free the linked list of server info
    close(socket_descriptor);
    freeaddrinfo(host_addrinfo);
    return 0;
}


// FIND_ADDR
// This function looks for a valid email address
// and returns a string containing the line it is on.
string SendIt::find_addr(string &msg, string target) {
    // get size of target substring
    int len = target.length();
    // start_address is the index of the target substring
    size_t start_address = msg.find(target);
    // This cuts off everything before target (plus its length)
    string msg_after_str = msg.substr(start_address + len);
    // This measures how long the string goes until the newline
    size_t address_length = msg_after_str.find("\n");
    string result = msg.substr(start_address + len, address_length);
    // Also make sure valid address
    size_t at_symbol;
    at_symbol = result.find("@");
    if (start_address != string::npos &&
        address_length != string::npos &&
        at_symbol != string::npos)
    {
            // Found. Return the result
            return result;
    } else {
            // Not found. Return empty string to see error.
            result.clear();
            return result;
    }
}


// SANITIZE_ADDR
// This function takes the initial parsing of an address
// and prepares it to be used for SMTP commands
void SendIt::sanitize_addr(string &address) {
    size_t checker;
    // This loop repeatedly deletes any spaces detected
    while ((checker = address.find(" ")) != string::npos) {
        address.erase(checker);
    }
    // This loop does the same thing for tabs
    while ((checker = address.find("\t")) != string::npos) {
        address.erase(checker);
    }
    // This looks for a semicolon and deletes everything after it.
    if ((checker = address.find(";")) != string::npos) {
        address = address.substr((size_t)0, checker);
    }
}


// PARSE_FILE
// This function is designed to open the file and pick all of
// the information out of it. It gets the envelope information, 
// as well as storing the message as a string into data members.
int SendIt::parse_file() {
    // variables that will be passed to send_message()
    string host("mailhost.cecs.pdx.edu"); // MOVE TO CONFIG FILE <----------
    string env_from, env_to;
    // stream variable for opening email file
    ifstream fin;
    cout << "Opening file: " << EmailFile << endl;
    fin.open(EmailFile.c_str());
    // variable for reading each line of data from file
    string line;
    while (getline(fin, line) != 0) {
        // SendIt's data member building up its data line by line
        // Check for lone "." to make sure message isn't cut off early
        if ((line.compare(".")) == 0)
            line += ".";
        Message += line += "\n";
    }
    fin.close(); // check return value                   <-------------------

    // Entire message file is now read. Need to search the
    // "Message" data member string for certain substrings.

    // Use utility function to extract to address
    env_to = find_addr(Message, "To: ");
    // Check for error from find
    if (env_to.empty() == true) {
        cout << "No valid To email address found.\n";
        return 1;
    }
    // Use utility function to clean it up.
    sanitize_addr(env_to);

    // Use utility function to extract from address
    env_from = find_addr(Message, "From: ");
    // Check for error
    if (env_from.empty() == true) {
        cout << "No valid From email address found.\n";
        return 1;
    }
    // Use utility function to clean it up.
    sanitize_addr(env_from);

    // OK! Now we should have all the necessary pieces!
    int sent;
    if ((sent = send(host, env_from, env_to)) != 0) {
        cerr << "Error sending message. Please try again.\n";
        return 1;
    }
    return 0;
}
