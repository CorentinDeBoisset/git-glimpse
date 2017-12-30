NAME = fast-git-prompt
BUILD_DIR = ./bin
SRC_DIR = ./src

CC = cc
INCLUDES = -I$(SRC_DIR)
LIBS = libgit2
CFLAGS += -W -Wall -ansi -pedantic -O2
LDFLAGS +=

ifneq ($(LIBS),)
	CFLAGS += $(shell pkg-config --cflags $(LIBS))
	LDFLAGS += $(shell pkg-config --libs $(LIBS))
endif

SRCS = src/main.c src/branch_status.c src/tree_status.c src/stash_status.c
OBJS = $(SRCS:.c=.o)

all: $(BUILD_DIR)/$(NAME)

$(BUILD_DIR)/$(NAME): $(OBJS)
	@$(CC) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_PATH)/%.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean: 
	rm -f $(OBJS) $(BUILD_DIR)/$(NAME)
