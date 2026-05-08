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

NAME		:= philosophers

#─ compile config ────────────────────────────────────────────────────────────

CC			:= cc
CFLAGS		:= -Wall -Wextra -Werror

# 開発用の厳格 CFLAGS（make dev で使用）
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

SRCS		:= main.c utils.c forks.c update.c init.c
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
	@rm -f $(NAME) philosophers_asan philosophers_tsan
	@echo "$(CYAN)$(NAME) removed.$(RESET)"

re: fclean all

leak: $(NAME)
	@echo "$(MAGENTA)$(BOLD)========================================="
	@echo "    Checking Memory Leaks..."
	@echo "=========================================$(RESET)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) "test string"
	@echo "$(GREEN)$(BOLD)Leak check completed!$(RESET)"

dev:
	@$(MAKE) re CFLAGS="$(DEV_CFLAGS)"
	@echo "$(MAGENTA)$(BOLD)[dev build] strict warnings + debug symbols$(RESET)"

# AddressSanitizer + UndefinedBehaviorSanitizer ビルド（philosophers_asan に出力）
NAME_ASAN	:= philosophers_asan
ASAN_CFLAGS	:= $(DEV_CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer

asan:
	@$(CC) $(ASAN_CFLAGS) $(SRCS) -o $(NAME_ASAN)
	@echo "$(MAGENTA)$(BOLD)[asan build] ./$(NAME_ASAN) — AddressSanitizer + UBSan$(RESET)"

# ThreadSanitizer ビルド（philosophers_tsan に出力）。並行処理のデータ競合検出用。
NAME_TSAN	:= philosophers_tsan
TSAN_CFLAGS	:= $(DEV_CFLAGS) -fsanitize=thread -fno-omit-frame-pointer

tsan:
	@$(CC) $(TSAN_CFLAGS) $(SRCS) -o $(NAME_TSAN)
	@echo "$(MAGENTA)$(BOLD)[tsan build] ./$(NAME_TSAN) — ThreadSanitizer$(RESET)"

.PHONY: all clean fclean re leak dev asan tsan
