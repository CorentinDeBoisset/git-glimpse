NAME = fast-git-prompt
BUILD_DIR = ./bin
SRC_DIR = ./src

CC = cc
INCLUDES = -I $(SRC_PATH)
LIBS = libgit2
CFLAGS += -W -Wall -ansi -pedantic
LDFLAGS +=

ifneq ($(LIBS),)
	CFLAGS += $(shell pkg-config --cflags $(LIBS))
	LFLAGS += $(shell pkg-config --libs $(LIBS))
endif

SRCS = src/main.c
OBJS = $(SRCS:.c=.o)

all: $(BUILD_DIR)/$(NAME)

$(BUILD_DIR)/$(NAME): $(OBJS)
	@$(CC) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_PATH)/%.c
	@$(CC) $(CFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

.PHONY: clean
clean: 
	rm -f $(OBJS) $(BUILD_DIR)/$(NAME)
