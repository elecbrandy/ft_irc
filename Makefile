CC = c++
CFLAGS = -Wall -Wextra -Werror
# CFLAGS = -Wall -Wextra -Werror -MMD -MP
# CFLAGS = -std=c++98 -Wall -Wextra -Werror -MMD -MP

NAME = server
INCS = .

SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -I$(INCS)

-include $(DEPS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCS)

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
