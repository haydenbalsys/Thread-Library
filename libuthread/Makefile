# Target library
lib := libuthread.a
objs := queue.o uthread.o context.o preempt.o sem.o

CC := gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS += -g

# Debug flag
ifneq ($(V),1)
Q = @
endif

all: $(lib)

# Include dependencies
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Make libuthread.a
$(lib): $(objs)
	@echo "AR    $@"
	$(Q)ar rcs $@ $^

# Generic rule for compiling objects
%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

# Cleaning rule
clean:
	@echo "CLEAN"
	$(Q)rm -rf $(lib) $(objs) $(deps)
