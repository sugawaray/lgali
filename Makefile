.POSIX:

TGTOBJS = eintr.o eled.o emc.o eser.o eeth.o
TARGETS = $(TGTOBJS:.o=)
OBJS = intr.o led.o lib.o mc.o quarkim.o ser.o serpri.o quark.o pcic.o

all: $(TARGETS)

clean:
	-rm $(TARGETS) $(TGTOBJS) $(OBJS)

$(TARGETS): $(TGTOBJS) $(OBJS)
	$(LD) -o $@ ${@}.o $(OBJS) $(LDFLAGS)


