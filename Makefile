#
# macfand - hipuranyhou - 21.06.2020
# 
# Daemon for controlling fans on systems using
# applesmc and coretemp.
# 
# https://github.com/Hipuranyhou/macfand
#

CC := gcc
CFLAGS := -w -O2
LD := gcc
LDFLAGS := -w -O2
SRCDIR := src
SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJDIR := obj
OBJFILES := $(SRCFILES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
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

install:
	cp $(EXECDIR)/$(EXEC) /usr/bin
	chown root:root /usr/bin/$(EXEC)
	chmod 755 /usr/bin/$(EXEC)
	cp macfand.service /etc/systemd/system
	systemctl daemon-reload
	systemctl enable --now macfand.service

uninstall:
	systemctl disable --now macfand.service
	systemctl daemon-reload
	rm -f /etc/systemd/system/macfand.service /usr/bin/$(EXEC)

clean:
	rm -rf $(OBJDIR) $(EXECDIR)

.PHONY: clean install uninstall