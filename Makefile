# SOURCES 
unit_test_src := main.c utils.c test.c check.c io.c display.c display_utils.c \
				sandbox.c list.c bonus.c string.c menu.c

# PATH GLOBAL
srcs_path := $(CURDIR)/srcs
objs_path := $(CURDIR)/objs
headers_path := headers

objs := $(unit_test_src:%.c=$(objs_path)/%.o)

# GLOBAL
CC := gcc
NAME := checker
PKG_LIBS := menu panel ncurses 
CFLAGS := -Wall -Wextra -Wshadow -Wformat=2 -Wformat-truncation -Wundef \
		-fno-common -O0 -ggdb -I$(headers_path) -pipe \
		`pkg-config --cflags $(PKG_LIBS)` \
		-rdynamic -MMD -D_GNU_SOURCE -std=c11 \
		-DDISPLAY -Werror

LDLIBS := -L$(PWD)
LDFLAGS := -no-pie -rdynamic -Wl,-whole-archive -lasm -Wl,-no-whole-archive -ldl `pkg-config --libs $(PKG_LIBS)`

deps = ${unit_test_src:%.c=%.d}

#RULES
.PHONY: all
all: $(PWD)/$(NAME)

.PHONY: $(NAME)
$(NAME): all

$(PWD)/$(NAME): $(objs)
	@printf "\e[34mGenerated : \e[5;36m$(NAME)\e[0m\n"
	@$(CC) $(objs) $(CFLAGS) -o $(PWD)/$(NAME) $(LDLIBS) $(LDFLAGS)

print_status: 
	@printf "\e[92mCompilation's flags: \e[36m$(ASFLAGS)\n"
	@printf "\e[92mGenerating objects :\e[0m\n"

$(objs_path)/%.o: $(srcs_path)/%.c
	@mkdir -p $(@D)
	@printf "\e[1;31m  >>  \e[4;34m$@\e[0m\n"
	@$(CC) $(CFLAGS) -c $< -o $@

-include ${deps}

.PHONY: clean
clean:
	@printf "\e[1;31mObject's removed\e[0m\n"
	@$(RM) -r $(objs_path)

.PHONY: fclean
fclean: clean
	@printf "\e[1;31m$(NAME) removed\e[0m\n"
	@$(RM) $(NAME)

.PHONY: re
re: fclean all
