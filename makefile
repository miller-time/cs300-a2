# Copyright 2010 by Russell Miller
# Please see included license information or visit
# http://opensource.org/licenses/mit-license.php

# makefile

all: modestmail

modestmail: main.o mailsender.o  sendit.o
	g++ main.o mailsender.o sendit.o -o modestmail

main.o: main.cc
	g++ -c main.cc

mailsender.o: mailsender.cc
	g++ -c mailsender.cc

sendit.o: sendit.cc
	g++ -c sendit.cc

clean:
	rm *.o modestmail
