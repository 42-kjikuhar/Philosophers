# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/05 15:45:00 by kjikuhar          #+#    #+#              #
#    Updated: 2026/04/23 01:59:58 by kjikuhar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#-----------------------------------------------------------------------------
#> variables
#-----------------------------------------------------------------------------

NAME		:= philo

#─ compile config ────────────────────────────────────────────────────────────

CC			:= cc
CFLAGS		:= -Wall -Wextra -Werror -pthread

DEV_CFLAGS	:= -Wall -Wextra -Werror \
               -Wpedantic -Wshadow \
               -Wstrict-prototypes -Wmissing-prototypes \
               -Wfloat-equal -Wundef -Wpointer-arith \
               -Wcast-align -Wformat=2 -Winit-self \
               -Wbad-function-cast -Wnested-externs \
               -Wstrict-aliasing=2 -Wdouble-promotion \
               -Wunreachable-code -Wredundant-decls \
               -O0 -g3 -std=c11

#─ directory name ────────────────────────────────────────────────────────────

OBJDIR		:= obj

#─ sources & objects ─────────────────────────────────────────────────────────

SRCS		:= main.c init.c utils.c routine.c monitor.c
OBJS		:= $(SRCS:%.c=$(OBJDIR)/%.o)

#─ colors ────────────────────────────────────────────────────────────────────

GREEN		:= \033[32m
YELLOW		:= \033[33m
CYAN		:= \033[36m
WHITE		:= \033[37m
MAGENTA		:= \033[35m
RESET		:= \033[0m
BOLD		:= \033[1m

#-----------------------------------------------------------------------------
#> rules
#-----------------------------------------------------------------------------

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $(OBJS)
	@echo "$(GREEN)$(BOLD)========================================="
	@echo "    $(NAME) created successfully!"
	@echo "=========================================$(RESET)"

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	@echo "Compiled ✅ $(WHITE)$(BOLD) $< $(RESET)"
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf $(OBJDIR)
	@echo "$(CYAN)Object files cleaned.$(RESET)"

fclean: clean
	@rm -f $(NAME) $(NAME_ASAN) $(NAME_TSAN)
	@rm -rf $(NAME_ASAN).dSYM $(NAME_TSAN).dSYM
	@echo "$(CYAN)$(NAME) removed.$(RESET)"

re: fclean all

ARGS ?= 5 800 200 200 3

valgrind: $(NAME)
	@echo "$(MAGENTA)$(BOLD)[valgrind] ./$(NAME) $(ARGS)$(RESET)"
	@valgrind --leak-check=full --show-leak-kinds=definite,indirect,possible \
	  --errors-for-leak-kinds=definite,indirect,possible \
	  --track-origins=yes --error-exitcode=1 \
	  ./$(NAME) $(ARGS)

dev:
	@$(MAKE) re CFLAGS="$(DEV_CFLAGS)"
	@echo "$(MAGENTA)$(BOLD)[dev build] strict warnings + debug symbols$(RESET)"

NAME_ASAN	:= philo_asan
ASAN_CFLAGS	:= $(DEV_CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer

asan:
	@$(CC) $(ASAN_CFLAGS) $(SRCS) -o $(NAME_ASAN)
	@echo "$(MAGENTA)$(BOLD)[asan build] ./$(NAME_ASAN) — AddressSanitizer + UBSan$(RESET)"

NAME_TSAN	:= philo_tsan
TSAN_CFLAGS	:= $(DEV_CFLAGS) -fsanitize=thread -fno-omit-frame-pointer

tsan:
	@$(CC) $(TSAN_CFLAGS) $(SRCS) -o $(NAME_TSAN)
	@echo "$(MAGENTA)$(BOLD)[tsan build] ./$(NAME_TSAN) — ThreadSanitizer$(RESET)"

.PHONY: all clean fclean re valgrind dev asan tsan
