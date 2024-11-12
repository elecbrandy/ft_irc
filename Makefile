NAME = ircserv

CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -g -fsanitize=address -Wall -Wextra -Werror -std=c++98

INCS = .
SRCS_CMD = $(wildcard Cmd/*.cpp)
SRCS_MAIN = $(wildcard *.cpp)
SRCS = $(SRCS_MAIN) $(SRCS_CMD)
HDRS = $(wildcard *.hpp)
OBJS = $(SRCS:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME) -I$(INCS)

%.o : %.cpp $(HDRS)
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re :
	$(MAKE) fclean
	$(MAKE) all

.PHONY : all clean fclean re