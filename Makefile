.POSIX:
.SUFFIXES: .o .to .s

TGTOBJS = eintr.o eled.o emc.o eser.o eeth.o eioapic.o
TESTENTS = tintr.to tethim.to tmii.to
TARGETS = $(TGTOBJS:.o=)
TESTTGTS = $(TESTENTS:.to=)
OBJS = intr.o lib.o mc.o quarkim.o ser.o serpri.o quark.o pcic.o led.o ethim.o mii.o ioapic.o hstbr.o
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

eioapic.c: include/quark.h include/ser.h
mii.o: mii.c miiim.h
mii.to: mii.c miiim.h
tmii.to: tmii.c miiim.h
ioapic.o: ioapic.c include/ioapic.h
ethim.o: ethim.c ethim.h include/eth.h
ethim.to: ethim.c ethim.h include/eth.h
eeth.o: eeth.c ethim.h include/eth.h
hstbr.o: hstbr.c include/quark.h include/hstbr.h
