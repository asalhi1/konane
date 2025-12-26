LD = gcc
CC = gcc

BLUE = \033[0;34m
GREEN = \033[;32m
YELLOW = \033[;33m
NC = \033[0m

CFLAGS := \
					-Isrc/include \
					-Wall \
					-Wextra \
					-Wno-unused-value \
					-Wno-unused-parameter \
					-Wno-return-type \
					-lncurses \
					-lpanel \
					-lmenu \
					-std=gnu99 \

CFILES := $(shell find src/ -name '*.c')
OFILES := $(CFILES:.c=.o)

TARGET = konane

all: welcome clean compile

welcome:
	@ echo ""
	@ echo " K O N A N E "
	@ echo ""

compile: ld
	@ echo "Done!"

ld: $(OFILES)
	@ echo -e "${GREEN}[ LD ]${NC} $^"
	@ $(LD) $^ -o $(TARGET)

%.o: %.c
	@ echo -e "${BLUE}[ CC ]${NC} $<"
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ echo -e "${YELLOW}[ CLEAN ]${NC}"
	@ rm -rf $(OFILES) $(TARGET)

run:
	@ ./konane

install:
	@ echo "Installing..."
	@ cp ./konane /usr/bin
	@ echo "Done!"

