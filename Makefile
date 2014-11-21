CC = g++
CFLAGS = -g -O2 -Wshadow -Wall -I/usr/local/include
GMP = -lgmp
HEdir = /home/michael/Desktop/cryptology/HElib/src
HElib = $(HEdir)/fhe.a

LDLIBS = -L/usr/local/lib $(HElib) -lntl $(GMP) -lm
INCDIR = -I$(HEdir)

HEgradebook: HEgradebook.cpp
	$(CC) $(CFLAGS) -o $@ $(INCDIR) $< $(LDLIBS)

clean:
	rm -f HEGradebook
