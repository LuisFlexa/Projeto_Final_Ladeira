# Makefile - leitor-exibidor JVM 8
#
# Alvos:
#   make           gera o binario "bin"
#   make debug     gera com simbolos -g
#   make clean     remove obj/ e bin
#   make run       compila e roda sem argumentos (mostra uso)
#
# Toolchain: g++ com suporte a C++14
# (testado com gcc 15.2 do MSYS2/mingw64 em Windows).

# No MSYS2/Windows o GCC pode tentar criar arquivos temporarios em
# C:\Windows e falhar com "Permission denied". Forcamos TMP/TEMP para
# uma pasta local. Em Linux/macOS isto e' inofensivo.
TMPLOCAL  := $(if $(LOCALAPPDATA),$(LOCALAPPDATA)/Temp,/tmp)
export TMP    := $(TMPLOCAL)
export TEMP   := $(TMPLOCAL)
export TMPDIR := $(TMPLOCAL)

CXX       := g++
DIR_INC   := include
DIR_SRC   := src
DIR_OBJ   := obj
BIN       := bin

OPT       := -O0
WARN      := -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable
EXTRA     := -std=c++14 -fdiagnostics-color=always
CXXFLAGS  := $(OPT) $(WARN) $(EXTRA) -I$(DIR_INC)

# Fontes (em ordem de dependencia conceitual):
#   util_classe       helpers compartilhados
#   args              parser de CLI
#   leitor_classe     parser binario do .class
#   exibidor_classe   emissor textual estilo jclasslib
#   main              entrada
FONTES    := \
    $(DIR_SRC)/util_classe.cpp \
    $(DIR_SRC)/args.cpp \
    $(DIR_SRC)/leitor_classe.cpp \
    $(DIR_SRC)/exibidor_classe.cpp \
    $(DIR_SRC)/main.cpp
OBJETOS   := $(patsubst $(DIR_SRC)/%.cpp, $(DIR_OBJ)/%.o, $(FONTES))

.PHONY: all debug clean run

all: $(BIN)

debug: CXXFLAGS += -g
debug: all

$(BIN): $(OBJETOS)
	$(CXX) -o $@ $^

$(DIR_OBJ):
	mkdir -p $@

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp | $(DIR_OBJ)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(DIR_OBJ) $(BIN) $(BIN).exe

run: all
	./$(BIN)
