prefix := $(shell /usr/realtime/bin/rtai-config --prefix)

ifeq ($(prefix),)
$(error Please add <rtai-install>/bin to your PATH variable)
endif

CC = $(shell /usr/realtime/bin/rtai-config --cc)
LXRT_CFLAGS = $(shell /usr/realtime/bin/rtai-config --lxrt-cflags)
LXRT_LDFLAGS = $(shell /usr/realtime/bin/rtai-config --lxrt-ldflags)
TARGET = demo_rtai
SOURCES = demo_rtai.c ../gdatalogger.c ../gmatlabdatafile.c  ../gqueue.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(LXRT_CFLAGS) $(SOURCES) -o $(TARGET) $(LXRT_LDFLAGS) 

clean:
	rm -f *.o $(TARGET)

.PHONY: clean
