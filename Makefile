ifndef CC
	CC = gcc
endif
ifndef CFLAGS
	CFLAGS = -Wall -Wextra -O2 -g $(EXTRA_CFLAGS)
endif

ifndef WIN32
	SHARED_LIB_NAME = libcl2.so
else
	SHARED_LIB_NAME = libcl2.dll
endif

define LOG
	@printf '\t%s\t%s\n' $1 $2
endef

define COMPILE
	$(call LOG,CC,$1)
	@$(CC) $(CFLAGS) $1 \
		-Iconfig -Iinclude -Isrc/include \
		-fPIC -c -o $2
endef

HEADER_FILES = $(wildcard include/*.h)
SOURCE_FILES = $(wildcard src/*.c)

OBJECT_FILES := $(patsubst src/%.c,%.o,$(SOURCE_FILES))

.PHONY: all libcl2 libcl2-log
all: libcl2

libcl2: libcl2-log $(SHARED_LIB_NAME)

libcl2-log:
	@echo Building $(SHARED_LIB_NAME)

$(SHARED_LIB_NAME): cc_defs.o $(OBJECT_FILES)
	$(call LOG,LD,$(SHARED_LIB_NAME))
	@$(CC) $(CFLAGS) $(OBJECT_FILES) -fPIC -shared -o $(SHARED_LIB_NAME)

%.o: src/%.c $(HEADER_FILES) src/include/cc_impl.h
	$(call COMPILE,$<,$@)

define BUILD_TEST_ITEM
	$(call LOG,BUILD,$1)
	@$(CC) $(CFLAGS) $2 -Iconfig -Iinclude -L. -lcl2 -lm -o $1
endef

define RUN_TEST_ITEM
	@printf '\tTEST\t%s\t\t\tCASE %s/%s\t' $1 $2 $3
	@LD_LIBRARY_PATH=. ./$1.bin $2 > /dev/null
	@printf 'PASS\n'
endef

.PHONY: test
test: test_vec

.PHONY: test_vec
test_vec: test_vec.bin
	$(call RUN_TEST_ITEM,test_vec,1,1)

TEST_SOURCE_FILES = $(wildcard test/*.c)
TEST_BIN_FILES := $(patsubst test/%.c,%.bin,$(TEST_SOURCE_FILES))

%.bin: test/%.c $(SHARED_LIB_NAME)
	$(call BUILD_TEST_ITEM,$@,$<)

.PHONY: clean
clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *.dll
	rm -f test_*.bin*
	rm -rf html
	rm -rf latex
