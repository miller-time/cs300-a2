# Copyright 2010 by Russell Miller
# Please see included license information in file
# COPYING or visit
# http://opensource.org/licenses/mit-license.php

# makefile

OBJS = main.o mailsender.o sendit.o
FILES = main.cc mailsender.cc sendit.cc
CC = g++
FLAGS = -c -Wall -g

all : modestmail

modestmail : objects
	$(CC) -o modestmail $(OBJS)

objects : $(FILES)
	$(CC) $(FLAGS) $(FILES)

clean :
	rm $(OBJS) modestmail
