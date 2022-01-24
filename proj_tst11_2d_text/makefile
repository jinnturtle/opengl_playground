NAME = exe

CXX_SRC =\
	main.cpp \
	FPS_manager.cpp \
	Randomizer.cpp \
	utils.cpp \
	logs.cpp \
	tutorial_libs/text2D.cpp \
	tutorial_libs/shader.cpp \
	tutorial_libs/texture.cpp

C_SRC =

CXX = g++
LL = g++
CC = gcc
CXX_FLAGS = -std=c++17 -Wall -Wextra -MMD -MF $(patsubst %.o,%.d,$@)
CC_FLAGS = -Wall -Wextra
LD_FLAGS =
DBG_FLAGS = -ggdb -DDEBUG=9
INCLUDE =
LIBS := -lstdc++
LIBS += $(shell pkg-config --libs gl glew glfw3)
SRC_DIR = src
OBJ_DIR = obj

_OBJ := $(CXX_SRC:.cpp=.o)
_OBJ += $(C_SRC:.c=.o)
OBJ = $(_OBJ:%=$(OBJ_DIR)/%)

DEPS = $(OBJ:%.o=%.d)

all: $(OBJ_DIR) $(NAME)

$(NAME): $(OBJ)
	@echo "LL $@"
	@$(LL) -o $@ $(OBJ) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp makefile
	@echo "CXX $< -> $@"
	@$(CXX) $(INCLUDE) $(DBG_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c makefile
	@echo "CC $< -> $@"
	@$(CC) $(INCLUDE)$(DBG_FLAGS) $(CC_FLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $@
	# guiltyly hacking the tut lib directory in for the time being
	mkdir -p "obj/tutorial_libs"


-include $(DEPS)

# release ----------------------------------------------------------------------
#  nothing here yet

# CLANG_TIDY_CHECKS = -checks=-*,bugprone-*,cert-*,cppcoreguidelines-*,clang-analyzer-*,google-*,-google-readability-todo,hicpp-*,llvm-*,misc-*,modernize-*,performance-*,portability-*,readability-*,-llvm-header-guard,-llvm-include-order
CLANG_TIDY_CHECKS = -checks=*,-abseil-*,-android-*,-boost-*,-fuchsia-*,-objc-*,-openmp-*,-zircon-*,-google-readability-todo,-llvm-header-guard,-llvm-include-order

# lint
.PHONY: lint lint-fast
lint-fast:
	@echo "\n***** cpplint *****\n"
	./cpplint.py --filter=-legal/copyright,-whitespace/comments,-whitespace/braces,-readability/nolint,-readability/todo,-build/c++11 --linelength=80 --extensions=cpp,hpp $(SRC_DIR)/*

lint: lint-fast
	@echo "\n***** clang-tidy *****\n"
	clang-tidy-9 $(SRC_DIR)/* $(CLANG_TIDY_CHECKS) -extra-arg=-std=c++17 --

# other ------------------------------------------------------------------------
.PHONY: todo todo.txt
todo: todo.txt
todo.txt:
	bash -c "grep 'TODO' -rn -A2 --include=*.{c,cpp,h,hpp} > $@"

.PHONY: ctags
ctags:
	bash -c "ctags -R {src,inc}"

.PHONY: clean
clean:
	rm -vrf $(OBJ_DIR)
	rm -vf $(NAME)
