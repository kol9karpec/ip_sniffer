CC=gcc
SCRS=main.c lib.c conf.c data.c
HDRS=lib.h conf.h data.h
OBJS=$(SCRS:.c=.o)
APP=my_app

$(APP): $(OBJS) $(HDRS)
	@$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	@rm -rf $(OBJS) $(APP)
