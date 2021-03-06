.POSIX:
.SUFFIXES: .o .to .s

TGTOBJS = emc.o eser.o eeth.o eioapic.o etm.o esd.o
TESTENTS = tintr.to tethim.to tmii.to tethim_read.to tsd.to tsd_cmd.to tfat.to
TARGETS = $(TGTOBJS:.o=)
TESTTGTS = $(TESTENTS:.to=)
OBJS = intr.o lib.o mc.o quarkim.o ser.o serpri.o quark.o pcic.o led.o ethim.o mii.o ioapic.o hstbr.o pit8254.o tmim.o lapic.o lapic1.o sd.o ctl.o time.o fat.o
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
	./tsd
	./tsd_cmd
	./tfat

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
esd.o: esd.c include/quark.h include/led.h include/ser.h ctl.h
hstbr.o: hstbr.c include/quark.h include/hstbr.h
serout.o: serout.c include/ser.h serim.h
serstub.to: serstub.c
tethim_read.to: tethim_read.c ethim.h include/quark.h
tsd.to: tsd.c sdim.h
tsd_cmd.to: tsd_cmd.c
pit8254.o: pit8254.s include/pit8254.h
sd.o: sd.c sdim.h
tmim.o: tmim.s include/tick.h
lapic.o: lapic.c include/lapic.h
lapic1.o: lapic1.s
ctl.o: ctl.c ctl.h
time.o: time.c include/time.h
tfat.to: tfat.c include/fat.h th.h
fat.o: fat.c include/fat.h
include/time.h: include/types.h
include/intr.h: include/types.h
include/lapic.h: include/types.h
include/lib.h: include/types.h
include/mii.h: include/types.h
include/quark.h: include/types.h
include/tick.h: include/types.h
include/unistd.h: include/types.h
