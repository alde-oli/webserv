# Nom du programme
NAME = webserv

# Compilateur
CXX = c++  # Utilisation de clang++ pour une meilleure compatibilité avec ASan

# Drapeaux de compilation
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 #-fsanitize=address -fno-omit-frame-pointer

# Dossiers
SRCDIR = src
OBJDIR = obj

# Trouver les fichiers source (.cpp) dans SRCDIR et ses sous-dossiers
SRC = $(shell find $(SRCDIR) -name '*.cpp')

# Remplacer le chemin des fichiers source par le chemin des fichiers objet (.o) dans OBJDIR
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Règle par défaut
all: $(NAME)

tg: $(NAME)

# Lien des fichiers objet pour créer l'exécutable
$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) #-fsanitize=address  Ajout de -fsanitize=address pour la liaison
	@echo "Compilation terminée : $(NAME)"

# Compilation des fichiers objet
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compilé : $<"

# Nettoyage des fichiers intermédiaires
clean:
	@rm -rf $(OBJDIR)
	@echo "Fichiers objet supprimés"

# Nettoyage complet (y compris l'exécutable)
fclean: clean
	@rm -f $(NAME)
	@echo "$(NAME) supprimé"

# Recompilation
re: fclean all

.PHONY: all tg clean fclean re
