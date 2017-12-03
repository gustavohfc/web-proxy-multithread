SRC_DIR = src
INC_DIR = ./include
OBJ_DIR = obj
DOC_DIR = doc
CACHE_DIR = ./cache

SOURCES  = $(wildcard $(SRC_DIR)/*.cpp)
INCLUDES = $(wildcard $(INC_DIR)/*.h)
OBJECTS  = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS = -g -Wall -Werror -std=c++11 -I$(INC_DIR)
EXE = bin/proxy
CXX = g++
RM = rm -rf

all: $(EXE)

$(EXE): $(OBJECTS)
	@echo "Linking and generating executable ("$@")"
	@$(CXX) $(CPPFLAGS) $(OBJECTS) -o $@

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo "Compiling "$<" to "$@""
	@$(CXX) $(CPPFLAGS) -c $< -o $@

.PHONY: docs
docs:
	doxygen Doxyfile

.PHONY: clean
clean: clean_obj clean_bin clean_cache

.PHONY: clean_doc
clean_doc:
	rm -rf $(DOC_DIR)/*

.PHONY: clean_obj
clean_obj:
	rm -rf $(OBJ_DIR)/*

.PHONY: clean_bin
clean_bin:
	rm -rf $(EXE)

.PHONY: clean_cache
clean_cache:
	rm -rf $(CACHE_DIR)/*
