CC = gcc
PP = g++
LINK = $(CC)
LINKPP = $(PP)

PATH_LIBS = /usr/local/lib
PATH_HEAD = /usr/local/include/help
PATH_BIN = /usr/local/bin

BIN_PREFIX = ""

SRC = ./src

BUILD = ./objs

CFLAGS      = -g3 -O3 -lJudy -lX11 -lzmq
PFLAGS      = -std=c++11
CSAHREDLIBS = -shared

INCS = -I$(SRC) \
	-I/usr/local/include

LIBS = -L/usr/local/lib \
	-L/usr/X11R6/lib

LDLIBS = -pthread -lJudy -W -Wall

DEPS = $(SRC)/config.h \
	$(SRC)/core.h \
	$(SRC)/logger.h \
	$(SRC)/pool.h \
	$(SRC)/str.h \
	$(SRC)/proc.h \
	$(SRC)/file.h 

DEPSPP = $(SRC)/proc_msg.h \
	    $(SRC)/proc_msg_udp.h \
	    $(SRC)/proc_msg_tcp.h \
	    $(SRC)/proc_msg_zmq.h \
	    $(SRC)/proc_msg_queue.h

OBJS = $(BUILD)/config.o \
	    $(BUILD)/logger.o \
	    $(BUILD)/pool.o \
	    $(BUILD)/proc.o \
	    $(BUILD)/file.o \
	    $(BUILD)/lockrun.o \
	    $(BUILD)/proc_msg.pp.o \
	    $(BUILD)/proc_msg_udp.pp.o \
	    $(BUILD)/proc_msg_tcp.pp.o \
	    $(BUILD)/proc_msg_zmq.pp.o \
	    $(BUILD)/proc_msg_queue.pp.o
	
OBJSCC = $(BUILD)/config.o \
	    $(BUILD)/logger.o \
	    $(BUILD)/pool.o \
	    $(BUILD)/proc.o \
	    $(BUILD)/file.o 

OBJSLOCK = $(BUILD)/lockrun.o
	
OBJSPP = $(BUILD)/proc_msg.pp.o \
	    $(BUILD)/proc_msg_udp.pp.o \
	    $(BUILD)/proc_msg_tcp.pp.o \
	    $(BUILD)/proc_msg_zmq.pp.o \
	    $(BUILD)/proc_msg_queue.pp.o

BINS = $(BUILD)/libhelp.so \
	$(BUILD)/lockrun \
	$(BUILD)/libhelppp.so

BINS2 = libhelp.so \
	lockrun \
	libhelppp.so

all: prebuild \
	$(BINS)

$(BUILD)/config.o: $(DEPS) \
	$(SRC)/config.c
	$(CC) -c $(CFLAGS) -fPIC $(INCS) -o $(BUILD)/config.o $(SRC)/config.c

$(BUILD)/logger.o: $(DEPS) \
	$(SRC)/logger.c
	$(CC) -c $(CFLAGS) -fPIC $(INCS) -o $(BUILD)/logger.o $(SRC)/logger.c
	
$(BUILD)/pool.o: $(DEPS) \
	$(SRC)/pool.c
	$(CC) -c $(CFLAGS) -fPIC $(INCS) -o $(BUILD)/pool.o $(SRC)/pool.c
	
$(BUILD)/string.o: $(DEPS) \
	$(SRC)/string.c
	$(CC) -c $(CFLAGS) -fPIC $(INCS) -o $(BUILD)/string.o $(SRC)/string.c
	
$(BUILD)/proc.o: $(DEPS) \
	$(SRC)/proc.c
	$(CC) -c $(CFLAGS) -fPIC $(INCS) -o $(BUILD)/proc.o $(SRC)/proc.c
	
$(BUILD)/file.o: $(DEPS) \
	$(SRC)/file.c
	$(CC) -c $(CFLAGS) -fPIC $(INCS) -o $(BUILD)/file.o $(SRC)/file.c
	
$(BUILD)/lockrun.o: $(DEPS) \
	$(SRC)/lockrun.c
	$(CC) -c $(CFLAGS) -fPIC $(INCS) -o $(BUILD)/lockrun.o $(SRC)/lockrun.c
	
$(BUILD)/proc_msg.pp.o: $(DEPS) $(DEPSPP) \
	$(SRC)/proc_msg.cpp
	$(PP) -c $(CFLAGS) $(PFLAGS) -fPIC $(INCS) -o $(BUILD)/proc_msg.pp.o $(SRC)/proc_msg.cpp
	
$(BUILD)/proc_msg_udp.pp.o: $(DEPS) $(DEPSPP) \
	$(SRC)/proc_msg_udp.cpp
	$(PP) -c $(CFLAGS) $(PFLAGS) -fPIC $(INCS) -o $(BUILD)/proc_msg_udp.pp.o $(SRC)/proc_msg_udp.cpp

$(BUILD)/proc_msg_tcp.pp.o: $(DEPS) $(DEPSPP) \
	$(SRC)/proc_msg_tcp.cpp
	$(PP) -c $(CFLAGS) $(PFLAGS) -fPIC $(INCS) -o $(BUILD)/proc_msg_tcp.pp.o $(SRC)/proc_msg_tcp.cpp
	
$(BUILD)/proc_msg_zmq.pp.o: $(DEPS) $(DEPSPP) \
	$(SRC)/proc_msg_zmq.cpp
	$(PP) -c $(CFLAGS) $(PFLAGS) -fPIC $(INCS) -o $(BUILD)/proc_msg_zmq.pp.o $(SRC)/proc_msg_zmq.cpp
	
$(BUILD)/proc_msg_queue.pp.o: $(DEPS) $(DEPSPP) \
	$(SRC)/proc_msg_queue.cpp
	$(PP) -c $(CFLAGS) $(PFLAGS) -fPIC $(INCS) -o $(BUILD)/proc_msg_queue.pp.o $(SRC)/proc_msg_queue.cpp
	
	
	
$(BUILD)/libhelp.so: \
	$(OBJSCC)
	$(CC) $(CSAHREDLIBS) -o $(BUILD)/libhelp.so $(OBJSCC) $(LDLIBS)

$(BUILD)/libhelppp.so: \
	$(OBJS)
	$(PP) $(CFLAGS) $(PFLAGS) $(CSAHREDLIBS) -o $(BUILD)/libhelppp.so $(OBJSCC) $(OBJSPP) $(LDLIBS)

$(BUILD)/lockrun: \
	$(BUILD)/lockrun.o \
	$(OBJS)
	$(LINK) $(CFLAGS) -o $(BUILD)/lockrun $(LIBS) $(OBJSLOCK) $(LDLIBS)

clean:
	rm -rf $(BUILD)

prebuild:
	test -d $(BUILD) || mkdir -p $(BUILD)

install: 
	test -d $(PATH_HEAD) || mkdir -p $(PATH_HEAD)
	
	chmod 755 $(PATH_HEAD)
	
	cp -rf $(BUILD)/*.so $(PATH_LIBS)
	cp -rf $(SRC)/*.h $(PATH_HEAD)
	cp -rf $(SRC)/lockrun $(PATH_BIN)

	@list='$(BINS2)'; for p in $$list; do \
		echo "cp $(BUILD)/$$p $(PATH_BIN)/$$p"; \
		`cp $(BUILD)/$$p $(PATH_BIN)/$$p`; \
	done
	
	ldconfig
	
