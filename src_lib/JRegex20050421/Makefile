# ject library ( libject ) Makefile
#  libject , use for jecty

# use cygwin,gcc,make,makedepend and konjo
#  this make for the Win32 build
# or a msvc-cl use

# define macro
CC = gcc
SRCS = regtest.c JRegex.c JChar.c jectlib.c
OBJS = $(SRCS:%.c=%.o)
LIBS = 
OPT = -Wall -O2 -s -mno-cygwin
OUT = reg

# make cl
VCLINK = /cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio/VC98/bin/link
VCLIBS = kernel32.lib imagehlp.lib -map:regvc.map -nologo
#VCOPT  = -nologo -O2 -W3 -D_DEBUG -DWIN32
VCOPT  = -nologo -O2 -W3 -D_DEBUG -DWIN32 -DUNICODE -D_UNICODE
VCOUT  = regvc

cl: $(VCOUT)
$(VCOUT): $(SRCS:%.c=%.obj)
	$(VCLINK) /OUT:$(VCOUT).exe $+ $(VCLIBS)
	objdump -p $(VCOUT).exe | grep dll
%.obj: %.c
	cl -c $< $(VCOPT)

# make rule
gcc: $(OUT)
$(OUT): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(OPT)
	objdump -p $(OUT).exe | grep dll

.c.o:
	$(CC) -c $< $(OPT)

# action command
# dump
dump:
	cl regdump.c -o regdump -nologo -W3 -O2
	objdump -p regdump.exe | grep dll

# profile
PROFOPT = -O2 -I/usr/include/mingw -mno-cygwin -DCYGWIN
PROFOUT = regpr
prof.%.o: %.c
	$(CC) -c $< -o $@ $(PROFOPT) -pg
prof: $(SRCS:%.c=prof.%.o)
	$(CC) -o $(PROFOUT) $+ $(LIBS) $(PROFOPT) -pg
	$(PROFOUT)
	gprof $(PROFOUT).exe > gprof.txt

# profile (UNICODE)
PROFUOPT = -O2 -I/usr/include/mingw -mno-cygwin -DCYGWIN -DUNICODE -D_UNICODE
PROFUOUT = regupr
profu.%.o: %.c
	$(CC) -c $< -o $@ $(PROFUOPT) -pg
profu: $(SRCS:%.c=profu.%.o)
	$(CC) -o $(PROFUOUT) $+ $(LIBS) $(PROFUOPT) -pg
	$(PROFUOUT)
	gprof $(PROFUOUT).exe > gprofu.txt

# clean
del: clean;
clean:
	rm -f $(OBJS)
	rm -f $(SRCS:%.c=prof.%.o)
	rm -f $(SRCS:%.c=%.obj)
	rm -f $(SRCS:%.c=prof.%.o)
	rm -f $(SRCS:%.c=profu.%.o)
	rm -f $(SRCS:%.c=%.obj)
	rm -f $(OUT).exe
	rm -f $(VCOUT).exe
	rm -f $(PROFOUT).exe
	rm -f $(PROFUOUT).exe
	rm -f regdump.exe
	rm -f regdump.obj
	rm -f gprof.txt
	rm -f gprofu.txt
	rm -f gmon.out
	rm -f regvc.map regvc.dump regvc.dump.txt
	rm -f $(SRCS:%.c=%.html)
	rm -f JRegex.ncb JRegex.opt JRegex.plg
	rm -f -R Debug
	rm -f -R Release
	rmdir Debug || true
	rmdir Release || true
#	rm -f Makefile.bak

# build
build: clean depend $(OUT) strip;

# jectdoc
doc: $(SRCS:%.c=%.html)
%.html: %.c
	jectdoc.pl $<

# makedepend
DEPEND.INC = depend.inc

depend:
	makedepend -- -Y -- $(SRCS) -o.obj 2> /dev/null
#	makedepend -- -Y -- $(SRCS) -o.o   2> /dev/null
# DO NOT DELETE

regtest.obj: jectlib.h JRegex.h JChar.h
JRegex.obj: jectlib.h JChar.h JRegex.h
JChar.obj: jectlib.h JChar.h
jectlib.obj: jectlib.h
