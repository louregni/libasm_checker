# SOURCES 
unit_test_src := main.c string.c io.c
			
# PATH GLOBAL
srcs_path := srcs
objs_path := objs
headers_path := headers

objs := $(unit_test_src:%.c=$(objs_path)/%.o)

# GLOBAL
CC := gcc
NAME := checker
CFLAGS = -Wall -Wextra -O0 -ggdb -I$(headers_path) #-Werror 
#CFLAGS = -Wall -Wextra -Werror -I$(headers_path)
#CFLAGS = -Wall -Wextra -Werror 

#RULES
.PHONY: all
all: $(NAME)

$(NAME): $(objs)
	@printf "\e[34mGenerated : \e[5;36m$(NAME)\e[0m\n"
	$(CC) $< -o $(NAME)

print_status: 
	@printf "\e[92mCompilation's flags: \e[36m$(ASFLAGS)\n"
	@printf "\e[92mGenerating objects :\e[0m\n"

$(objs_path)/%.o: $(srcs_path)/%.c
	@mkdir -p $(@D)
	@printf "\e[1;31m  >>  \e[4;34m$@\e[0m\n"
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@printf "\e[1;31mObject's removed\e[0m\n"
	$(RM) -r $(objs_path)

.PHONY: fclean
fclean: clean
	@printf "\e[1;31m$(NAME) removed\e[0m\n"
	$(RM) $(NAME)

.PHONY: re
re: fclean all
