.POSIX:
.SUFFIXES: .o .to .s

TGTOBJS = eintr.o eled.o emc.o eser.o eeth.o
TESTENTS = tintr.to tethim.to
TARGETS = $(TGTOBJS:.o=)
TESTTGTS = $(TESTENTS:.to=)
OBJS = intr.o lib.o mc.o quarkim.o ser.o serpri.o quark.o pcic.o led.o ethim.o mii.o
TOBJS = $(OBJS:.o=.to)

all: $(TARGETS) $(TESTTGTS)

clean:
	-rm $(TARGETS) $(TGTOBJS) $(OBJS) $(TESTTGTS) $(TESTENTS) $(TOBJS)

$(TARGETS): $(TGTOBJS) $(OBJS)
	$(LD) -o $@ ${@}.o $(OBJS) $(LDFLAGS)

$(TESTTGTS): $(TESTENTS) $(TOBJS)
	$(LD) -o $@ ${@}.to $(TOBJS) $(TLDFLAGS)

.c.to:
	$(CC) -c -o $@ $< $(TCFLAGS) $(TCPPFLAGS)

.s.to:
	$(AS) -o $@ $< $(TASFLAGS)
