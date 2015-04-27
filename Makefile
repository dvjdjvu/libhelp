CC = gcc
LINK = $(CC)

PATH_LIBS = /usr/local/lib
PATH_HEAD = /usr/local/include/help

BIN_PREFIX = ""

SRC = ./src

BUILD = ./objs

CFLAGS      = -g3 -O3
CSAHREDLIBS = -shared

INCS = -I$(SRC) \
	-I/usr/local/include

LIBS = -L/usr/local/lib 

LDLIBS = -lJudy

DEPS = $(SRC)/config.h \
	$(SRC)/core.h \
	$(SRC)/logger.h \
	$(SRC)/pool.h \
	$(SRC)/str.h 

OBJSLIB = $(BUILD)/config.o \
	    $(BUILD)/logger.o \
	    $(BUILD)/pool.o 

BINS = $(BUILD)/libhelp.so 

BINS2 = libhelp.so

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
	
$(BUILD)/libhelp.so: \
	$(OBJSLIB)
	$(CC) $(CSAHREDLIBS) -o $(BUILD)/libhelp.so $(OBJSLIB) $(LDLIBS)

clean:
	rm -rf $(BUILD)

prebuild:
	test -d $(BUILD) || mkdir -p $(BUILD)

install: 
	test -d $(PATH_HEAD) || mkdir -p $(PATH_HEAD)
	
	chmod 755 $(PATH_HEAD)
	
	cp -rf $(BUILD)/*.so $(PATH_LIBS)
	cp -rf $(SRC)/*.h $(PATH_HEAD)

	@list='$(BINS2)'; for p in $$list; do \
		echo "cp $(BUILD)/$$p $(PATH_BIN)/$$p"; \
		`cp $(BUILD)/$$p $(PATH_BIN)/$$p`; \
	done
	
	ldconfig
	
