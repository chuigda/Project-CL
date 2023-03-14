ifndef CC
	CC = gcc
endif
ifndef CFLAGS
	CFLAGS = -Wall -Wextra -O2 -g $(EXTRA_CFLAGS)
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

libcl2: libcl2-log libcl2.so

libcl2-log:
	@echo 'Building libcl2.so'

libcl2.so: cc_defs.o $(OBJECT_FILES)
	$(call LOG,LD,libcl2.so)
	@$(CC) $(CFLAGS) $(OBJECT_FILES) -fPIC -shared -o libcl2.so

%.o: src/%.c $(HEADER_FILES) src/include/cc_impl.h
	$(call COMPILE,$<,$@)

.PHONY: clean
clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f test_*.bin*
	rm -rf html
	rm -rf latex
