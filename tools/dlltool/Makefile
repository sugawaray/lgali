.POSIX:

TARGET = dllsend test
OBJS = impl.o

all: $(TARGET)

$(TARGET): FORCE $(OBJS)
	$(CC) -o $@ $@.c $(OBJS) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

clean:
	-rm -f $(TARGET)
	-rm -f $(OBJS)

dllsend.c: impl.h
impl.o: impl.c impl.h

FORCE:

