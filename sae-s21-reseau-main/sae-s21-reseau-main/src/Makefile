CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_DIR = .
OBJ_DIR = ../out
EXE_DIR = ../exe

# Liste des fichiers source
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
EXE = $(EXE_DIR)/network_test

# Création des répertoires si nécessaire
$(shell mkdir -p $(OBJ_DIR) $(EXE_DIR))

# Règle par défaut
all: $(EXE)

# Compilation des fichiers source
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Création de l'exécutable
$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Nettoyage
clean:
	rm -f $(OBJ_DIR)/*.o $(EXE)

.PHONY: all clean 