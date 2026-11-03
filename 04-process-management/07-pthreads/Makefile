CC := gcc
CFLAGS := -Wall -Wextra -std=c11
LDLIBS := -pthread

PROGRAMS := \
	01-foundations/p1a_hello \
	01-foundations/p1b_shared_private \
	02-lifecycle/p2a_args \
	02-lifecycle/p2b_return \
	02-lifecycle/p2c_exit \
	02-lifecycle/p2d_detach \
	02-lifecycle/p2e_loop_bug \
	03-attributes/p3a_stack_size \
	03-attributes/p3b_detach_attr \
	04-mutexes/p4a_race \
	04-mutexes/p4b_mutex \
	04-mutexes/p4c_trylock \
	04-mutexes/p4d_mutex_init

all: $(PROGRAMS)

01-foundations/p1a_hello: 01-foundations/p1a_hello.c
01-foundations/p1b_shared_private: 01-foundations/p1b_shared_private.c
02-lifecycle/p2a_args: 02-lifecycle/p2a_args.c
02-lifecycle/p2b_return: 02-lifecycle/p2b_return.c
02-lifecycle/p2c_exit: 02-lifecycle/p2c_exit.c
02-lifecycle/p2d_detach: 02-lifecycle/p2d_detach.c
02-lifecycle/p2e_loop_bug: 02-lifecycle/p2e_loop_bug.c
03-attributes/p3a_stack_size: 03-attributes/p3a_stack_size.c
03-attributes/p3b_detach_attr: 03-attributes/p3b_detach_attr.c
04-mutexes/p4a_race: 04-mutexes/p4a_race.c
04-mutexes/p4b_mutex: 04-mutexes/p4b_mutex.c
04-mutexes/p4c_trylock: 04-mutexes/p4c_trylock.c
04-mutexes/p4d_mutex_init: 04-mutexes/p4d_mutex_init.c

$(PROGRAMS):
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
