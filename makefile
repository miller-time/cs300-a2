# Copyright 2010 by Russell Miller
# Please see included license information or visit
# http://opensource.org/licenses/mit-license.php

# makefile

all: modestmail

modestmail: main.o
	g++ main.o -o modestmail

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o modestmail
