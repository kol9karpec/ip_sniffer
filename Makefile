CC=gcc

SCRS=main.c \
	 lib.c \
	 log.c \
	 conf.c \
	 data.c

HDRS=lib.h \
	 log.h \
	 conf.h \
	 data.h \
	 types.h

OBJS=$(SCRS:.c=.o)
APP=my_app
CFLAGS+= -pthread
CFLAGS+= -DNO_AUTO_SNIFF

$(APP): $(OBJS) $(HDRS)
	@$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	@rm -rf $(OBJS) $(APP)
