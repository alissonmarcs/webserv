NAME		= webserv
WFLAGS 		= -Wall -Werror -Wextra
CPPFLAGS	= -std=c++98 -I ./includes/
DEBUG_FLAGS =

SOURCES		= $(wildcard ./src/*.cpp)
SOURCES		:= $(patsubst ./src/%,%, $(SOURCES))
OBJECTS		= $(addprefix objs/,$(SOURCES:.cpp=.o))

HEADERS 	= $(wildcard ./includes/*.hpp)

ifdef REMOVE_W_FLAGS
	WFLAGS =
endif

ifdef DEBUG
		DEBUG_FLAGS := -g3 -fno-limit-debug-info
endif

ifdef REMOVE_STD
		CPPFLAGS := $(subst -std=c++98,,$(CPPFLAGS))
endif

CYAN = \033[36m
RESET = \033[0m

all: objs $(NAME)

objs:
	@mkdir objs

$(NAME): $(OBJECTS)
	@printf "$(CYAN)Compiling...$(RESET)\n" 
	@c++ $(CPPFLAGS) $(DEBUG_FLAGS) $(WFLAGS) $(OBJECTS) -o $(NAME)
	@printf "$(CYAN)Target ./$(NAME) done$(RESET)\n" 

objs/%.o: ./src/%.cpp $(HEADERS)
	@c++ $(CPPFLAGS) $(DEBUG_FLAGS) $(WFLAGS) -c $< -o $@

clean:
	@rm -rf objs

fclean: clean
	@rm -rf $(NAME)

re: fclean all
