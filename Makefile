CC ?= clang
CFLAGS ?= -Wall -O0 -g
INC = include/

APP = mtm

SRC = src
INC = include
SOURCES = $(SRC)/mtm.c $(SRC)/multi_timer.c $(SRC)/mtm_audio.c
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))
LINKS = -lform -lncurses -lsndfile -lportaudio

.PHONY: all
all: $(APP) compdb

$(APP): $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INC) $(OBJECTS) -o $@ $(LINKS)

DEPS := $(OBJECTS:.o=.d)
-include $(DEPS)

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) -I$(INC) -MMD -MF $(patsubst %.o,%.d,$@) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(DEPS)
	rm -f $(APP)
 
TYPES = types.vim
tags: $(OBJECTS)
	ctags -R

# NOTE: For types to have any benefit, you would need the appropriate config
# in your .vimrc for the keyword 'DevType'
types: $(TYPES)
$(TYPES): $(OBJECTS)
	@echo "Re-building custom types"
	@ctags -R --languages=c++ --kinds-c=gstud -o- |\
		awk 'BEGIN{printf("syntax keyword DevType\t")}\
			{printf("%s ", $$1)}END{print ""}' > $@

# REF: https://pypi.org/project/compiledb/
.PHONY: compdb
compdb: compile_commands.json

compile_commands.json: Makefile
	make -Bnwk | compiledb
