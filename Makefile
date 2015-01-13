# PZAB Interpreter Makefile
# -------------------------
#  Makefile for the interpreter for the PZAB programming language.
#  See README for information on setting up this makefile.
#
#  Copyright (C) 2015 Zac Herd <zacjherd@gmail.com>
#  Released under the GNU General Public License v2.
#  See LICENSE for more info
#


###################
# BUILD VARIABLES #
###################

# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -g

# Directory containing source files
SOURCEDIR = src

# Directory to build objects
OBJDIR = obj

# Directory to build executable
BUILDDIR = bin

# Executable to produce
TARGET = pzab

# Directory to install to
INSTDIR = /usr/local/bin

# NOTHING SHOULD NEED TO CHANGE BEYOND THIS POINT

# Source files to use
SOURCES = $(wildcard $(SOURCEDIR)/*.c)

# Objects to compile in executable
OBJS = $(patsubst $(SOURCEDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))


#################
# BUILD TARGETS #
#################

# make target
.PHONY: all
all: $(SOURCES) $(BUILDDIR)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJS): $(OBJDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# install target
.PHONY: install
install:
	cp $(BUILDDIR)/$(TARGET) $(INSTDIR)/$(TARGET)

# uninstall target
.PHONY: uninstall
uninstall:
	rm $(INSTDIR)/$(TARGET)

# clean build directories
.PHONY: clean
clean: 
	rm -fr $(OBJDIR)/*o $(BUILDDIR)/$(TARGET)
