# Copyright 2010 by Russell Miller
# Please see included license information or visit
# http://opensource.org/licenses/mit-license.php

# makefile

all: modestmail

modestmail: main.o sendit.o
	g++ main.o sendit.o -o modestmail

main.o: main.cc
	g++ -c main.cc

sendit.o: sendit.cc
	g++ -c sendit.cc

clean:
	rm *.o modestmail
