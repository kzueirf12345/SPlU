.PHONY: all clean

PROJECT_NAME = SPlU

COMPILER = gcc

DEBUG_ = 1

FLAGS =	-Wall -Wextra -Waggressive-loop-optimizations \
		-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
		-Wconversion -Wempty-body -Wfloat-equal \
		-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
		-Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self \
		-Wredundant-decls -Wshadow -Wsign-conversion \
		-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
		-Wsuggest-final-types -Wswitch-default -Wswitch-enum -Wsync-nand \
		-Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
		-Wno-missing-field-initializers -Wno-narrowing -Wno-varargs \
		-Wstack-protector -fcheck-new -fstack-protector -fstrict-overflow \
		-flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=81920 -Wstack-usage=81920 -pie \
		-fPIE -Werror=vla \

SANITIZER = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,$\
		integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,$\
		shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DEBUG_FLAGS = -D _DEBUG  -ggdb -Og -g3 -D_FORTIFY_SOURCES=3 $(SANITIZER)
RELEASE_FLAGS = -DNDEBUG -O2
FLAGS += $(if $(DEBUG_),$(DEBUG_FLAGS),$(RELEASE_FLAGS))
ADD_FLAGS = #FIXME


all: assembler_all processor_all

build: assembler_build processor_build

start: assembler_start processor_start


assembler_all: assembler_build assembler_start

assembler_start:
	make ADD_FLAGS="$(ADD_FLAGS)" start -C ./assembler/

assembler_rebuild: assembler_clean assembler_build

assembler_build:
	make ADD_FLAGS="$(ADD_FLAGS)" build -C ./assembler/

assembler_clean:
	make ADD_FLAGS="$(ADD_FLAGS)" clean -C ./assembler/


processor_all: processor_build processor_start

processor_start:
	make ADD_FLAGS="$(ADD_FLAGS)" start -C ./processor/

processor_rebuild: processor_clean processor_build

processor_build:
	make ADD_FLAGS="$(ADD_FLAGS)" build -C ./processor/

processor_clean:
	make ADD_FLAGS="$(ADD_FLAGS)" clean -C ./processor/


libs_rebuild: libs_clean libs_build

libs_build:
	make ADD_FLAGS="$(ADD_FLAGS)" build -C ./libs/stack_on_array/ && \
	make ADD_FLAGS="$(ADD_FLAGS)" build -C ./libs/logger/

libs_clean:
	make ADD_FLAGS="$(ADD_FLAGS)" clean -C ./libs/stack_on_array/ && \
	make ADD_FLAGS="$(ADD_FLAGS)" clean -C ./libs/logger/



clean: assembler_clean processor_clean

clean_all:
	make ADD_FLAGS="$(ADD_FLAGS)" clean_all -C ./assembler/ && \
	make ADD_FLAGS="$(ADD_FLAGS)" clean_all -C ./processor/