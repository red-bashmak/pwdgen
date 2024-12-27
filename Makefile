CC = gcc
LDFLAGS = 

TARGET_EXEC := pwdgen

BUILD_DIR := ./build
SRC_DIRS := ./src

# Find all the C files we want to compile
SRCS := $(shell find $(SRC_DIRS) -name '*.c' -or -name '*.s')

# Prepends BUILD_DIR and appends .o to every src file
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CFLAGS := $(INC_FLAGS) -MMD -MP -Wall -Werror -std=gnu11

all: $(BUILD_DIR)/$(TARGET_EXEC)

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

release: CFLAGS += -O3
release: all

clean:
	rm -r $(BUILD_DIR)

.PHONY: all clean release


# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)