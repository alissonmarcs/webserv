NAME		= webserv
WFLAGS 		= -Wall -Werror -Wextra -fsanitize=address
CPPFLAGS	= -std=c++98 
DEBUG_FLAGS =

CLASSES = ./src/classes

HEADERS_FOLDERS := $(wildcard $(CLASSES)/*/)
HEADERS_FOLDERS := $(addprefix -I, $(HEADERS_FOLDERS))
HEADER_FILES = $(wildcard $(CLASSES)/*/*.hpp)

SOURCES = $(wildcard src/classes/*/*.cpp)
SOURCES += src/main.cpp

OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS := $(addprefix ./objs/, $(OBJECTS))

ifdef REMOVE_W_FLAGS
	WFLAGS =
endif

ifdef DEBUG_CLANG
		DEBUG_FLAGS := -gdwarf-4 -O0 -Wunused-macros -g3 -fno-omit-frame-pointer -fno-inline -fno-elide-constructors -fno-limit-debug-info
endif

ifdef DEBUG_GCC
		DEBUG_FLAGS := -g3 -O0 -Wunused-macros -fno-omit-frame-pointer -fno-inline -fno-elide-constructors
endif

ifdef REMOVE_STD
		CPPFLAGS := 
endif

CYAN = \033[36m
RESET = \033[0m

all: objs $(NAME)

objs:
	@mkdir ./objs;

$(NAME): $(OBJECTS) $(HEADER_FILES)
	@printf "$(CYAN)Compiling...$(RESET)\n" 
	@c++ $(CPPFLAGS) $(HEADERS_FOLDERS) $(DEBUG_FLAGS) $(WFLAGS) $(OBJECTS) -o $(NAME)
	@printf "$(CYAN)Target ./$(NAME) done$(RESET)\n" 


./objs/%.o: %.cpp 
	@mkdir -p $(dir $@)
	@c++ $(CPPFLAGS) $(HEADERS_FOLDERS) $(DEBUG_FLAGS) $(WFLAGS) -c $< -o $@

clean:
	@rm -rf objs

fclean: clean
	@rm -rf $(NAME)

re: fclean all
