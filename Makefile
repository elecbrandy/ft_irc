NAME = ircserv

CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = $(wildcard *.cpp)
HDRS = $(wildcard *.hpp)
OBJS = $(SRCS:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

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