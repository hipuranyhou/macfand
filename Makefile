#
# macfand - hipuranyhou - 18.01.2021
# 
# Daemon for controlling fans on systems using
# applesmc and coretemp.
# 
# https://github.com/Hipuranyhou/macfand
#

CC := gcc
CFLAGS := -Wall -Wextra -pedantic -g
LD := gcc
LDFLAGS := -Wall -Wextra -pedantic -g
SRCDIR := src
SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJDIR := obj
OBJFILES := $(SRCFILES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
SYSDDIR := /etc/systemd/system
INSDIR := /usr/bin
EXECDIR := bin
EXEC := macfand

all: $(OBJDIR) $(EXECDIR) $(EXECDIR)/$(EXEC)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(EXECDIR):
	mkdir -p $(EXECDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECDIR)/$(EXEC): $(OBJFILES)
	$(LD) $(LDFLAGS) $^ -o $@

run:
	$(EXECDIR)/./$(EXEC) --config=macfand.conf

run_valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(EXECDIR)/./$(EXEC) --config=macfand.conf

install:
	cp $(EXECDIR)/$(EXEC) $(INSDIR)
	chmod 755 $(INSDIR)/$(EXEC)
	cp macfand.service $(SYSDDIR)
	cp macfand.conf /etc
	systemctl daemon-reload
	systemctl enable --now macfand.service

uninstall:
	systemctl disable --now macfand.service
	systemctl daemon-reload
	rm -f $(SYSDDIR)/macfand.service $(INSDIR)/$(EXEC) /etc/macfand.conf

clean:
	rm -rf $(OBJDIR) $(EXECDIR)

.PHONY: clean install uninstall run run_valgrind