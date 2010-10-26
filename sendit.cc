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
#include <string.h>
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

// DESTRUCTOR
SendIt::~SendIt() {
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
        cerr << "Warning: Message not fully transmitted.\n";
    }
    bytes_sent = read(sd, buf, 999);
    // Make sure the rest of the array doesn't spill junk.
    buf[bytes_sent] = '\0';
    return (string(buf));
}


// SEND
// This function is inherited from MailSender and is designed
// to connect to a socket, and send the email message to the server
// specified over that the socket.
// It uses SMTP messaging to send specific commands to the server
// and checks the response each time before continuing.
// Data members will already be set by parse_file before this
// function is called, which will be used in the commands sent
// to the server.
// Anywhere there is a return 1 means the communication stopped
// before the conversation with the SMTP server completed.
int SendIt::send(string &host_to,
                 string &envelope_from,
                 string &envelope_to)
{
    int socket_descriptor;
    int status;
    // setting my address flags
    addrinfo hints;
    // making sure struct is cleared out
    memset(&hints, 0, sizeof hints);
    // either IPv4 or IPv6
    hints.ai_family = AF_UNSPEC;
    // TCP Sockets
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    addrinfo *host_addrinfo;
    // Make the call to fill the addrinfo struct
    if ((status = getaddrinfo(host_to.c_str(), "25", &hints, &host_addrinfo)) != 0) {
        cerr << gai_strerror(status) << endl;
        return -1;
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
    string long_response = read_and_write(socket_descriptor, smtp_msg);
    string short_response = long_response.substr(0,3);
    // Do not continue if server closed connection
    if (short_response.compare("220") != 0) {
        cerr << long_response;
        close(socket_descriptor);
        freeaddrinfo(host_addrinfo);
        return -1;
    } else {
        cout << "OK.\n";
    }


    // Set MAIL FROM message
    smtp_msg = "MAIL FROM:";
    smtp_msg += envelope_from + "\r\n";
    // Use utility function to contact server
    long_response = read_and_write(socket_descriptor, smtp_msg);
    short_response = long_response.substr(0,3);
    // Do not continue if server closed connection
    if (short_response.compare("250") != 0) {
        cerr << long_response;
        close(socket_descriptor);
        freeaddrinfo(host_addrinfo);
        return -1;
    } else {
        cout << "OK.\n";
    }


    // Set RCPT TO message
    smtp_msg = "RCPT TO:";
    smtp_msg += envelope_to + "\r\n";
    // Use utility function to contact server
    long_response = read_and_write(socket_descriptor, smtp_msg);
    short_response = long_response.substr(0,3);
    // Do not continue if server closed connection
    if (short_response.compare("250") != 0) {
        cerr << long_response;
        close(socket_descriptor);
        freeaddrinfo(host_addrinfo);
        return -1;
    } else {
        cout << "OK.\n";
    }


    // Set DATA message
    smtp_msg = "DATA\r\n";
    // Use utility function to contact server
    long_response = read_and_write(socket_descriptor, smtp_msg);
    short_response = long_response.substr(0,3);
    // Do not continue if server closed connection
    if (short_response.compare("354") != 0) {
        cerr << long_response;
        close(socket_descriptor);
        freeaddrinfo(host_addrinfo);
        return -1;
    } else {
        cout << "OK.\n";
    }


    // Set actual message
    smtp_msg = Message + "\r\n.\r\n";
    // Use utility function to contact server
    long_response = read_and_write(socket_descriptor, smtp_msg);
    short_response = long_response.substr(0,3);
    // Do not continue if server closed connection
    if (short_response.compare("250") != 0) {
        cerr << long_response;
        close(socket_descriptor);
        freeaddrinfo(host_addrinfo);
        return -1;
    } else {
        cout << "OK.\n";
    }


    // Set QUIT message
    smtp_msg = "QUIT\r\n";
    // Use utility function to contact server
    read_and_write(socket_descriptor, smtp_msg);

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
    // make sure find calls were all successful
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
    // This is for any dangling newline characters
    if ((checker = address.find("\n")) != string::npos) {
        address.erase(checker);
    }
    // This removes parenthesised comments
    size_t leftpar, rightpar;
    while ((leftpar = address.find("(")) != string::npos &&
           (rightpar = address.find_last_of(')')) != string::npos) {
        address = address.erase(leftpar + 1, rightpar - leftpar);
    }
}


// PARSE_FILE
// This function is designed to open the file and pick all of
// the information out of it. It gets the envelope information, 
// as well as storing the message as a string.
// All of this information is held in the SendIt object, to be
// used by the SEND function.
int SendIt::parse_file() {
    // variables that will be passed to send_message()
    string host, env_from, env_to;
    string conf_filename("modestmail.conf");
    string line;
    // stream variable for opening config file
    ifstream config_file(conf_filename.c_str(), ifstream::in);
    // Check if config file exists
    if (config_file) {
        // It does exist, so open it.
        string conf;
        // read contents of config file
        while (getline(config_file, line) != 0) {
            conf += line + "\n";
        }
        config_file.close();
        // Look for host in the config file
        size_t finder;
        if ((finder = conf.find("HOST = ")) != string::npos) {
            // value was found. get the host, sanitize it
            host = conf.substr(finder + 7);
            sanitize_addr(host);
        } else {
            // Exit, config file is corrupt.
            cerr << "HOST not found in current "
                 << conf_filename << " file.\n";
            return -1;
        }
    } else {
        // Config file doesn't exist. So using default host and
        // creating a config file containing it.
        cout << "Creating file modestmail.conf...\n";
        ofstream making_conf_file(conf_filename.c_str());
        host = "mailhost.cecs.pdx.edu";
        making_conf_file << "HOST = " << host;
        making_conf_file.close();
    }

    // New stream variable for opening email file
    ifstream fin(EmailFile.c_str());
    if (!fin) {
        cerr << "File " << EmailFile << " not found.\n";
        return 1;
    }
    cout << "Opening file " << EmailFile << endl;
    // variable for reading each line of data from file
    while (getline(fin, line) != 0) {
        // SendIt's data member building up its data line by line
        // Check for lone "." to make sure message isn't cut off early
        if ((line.compare(".")) == 0)
            line += ".";
        Message += line += "\n";
    }
    fin.close();

    // Entire message file is now read. Need to search the
    // "Message" data member string for certain substrings.

    // Use utility function to extract to address
    env_to = find_addr(Message, "To: ");
    // Check for error from find
    if (env_to.empty() == true) {
        cerr << "No valid To email address found.\n";
        return -1;
    }
    // Use utility function to clean it up.
    sanitize_addr(env_to);

    // Use utility function to extract from address
    env_from = find_addr(Message, "From: ");
    // Check for error
    if (env_from.empty() == true) {
        cerr << "No valid From email address found.\n";
        return -1;
    }
    // Use utility function to clean it up.
    sanitize_addr(env_from);

    // OK! Now we should have all the necessary pieces!
    if (send(host, env_from, env_to) != 0) {
        cerr << "Error sending message. Please try again.\n";
        return -1;
    }
    return 0;
}
