.POSIX:
.SUFFIXES: .o .to .s

TGTOBJS = eintr.o eled.o emc.o eser.o eeth.o eioapic.o etm.o
TESTENTS = tintr.to tethim.to tmii.to tethim_read.to
TARGETS = $(TGTOBJS:.o=)
TESTTGTS = $(TESTENTS:.to=)
OBJS = intr.o lib.o mc.o quarkim.o ser.o serpri.o quark.o pcic.o led.o ethim.o mii.o ioapic.o hstbr.o
HOBJS = serout.o
TOBJS = serstub.to $(OBJS:.o=.to)

all: $(TARGETS) $(TESTTGTS) calltest

clean:
	-rm $(TARGETS) $(TGTOBJS) $(OBJS) $(HOBJS) $(TESTTGTS) $(TESTENTS) $(TOBJS)

calltest:
	./tintr
	./tmii
	./tethim
	./tethim_read

$(TARGETS): $(TGTOBJS) $(OBJS) $(HOBJS)
	$(LD) -o $@ ${@}.o $(OBJS) $(HOBJS) $(LDFLAGS)

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
etm.o: etm.c intrim.h include/intr.h include/led.h include/quark.h include/ser.h
eeth.o: eeth.c ethim.h include/eth.h
hstbr.o: hstbr.c include/quark.h include/hstbr.h
serout.o: serout.c include/ser.h serim.h
serstub.to: serstub.c
tethim_read.to: tethim_read.c ethim.h include/quark.h
