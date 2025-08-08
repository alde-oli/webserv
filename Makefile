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

# Règle par défaut, incluant zbeb et surpriz
all: zbeb surpriz $(NAME)

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

# Vos cibles personnalisées
zbeb:
	@echo "\n\n[Message de zbeb]\nJ'koné une meuf narvalo, elle habite à Metz, bah elle est plus fraiche que toua\n\n"

surpriz:
	@echo "Choisissez le kdo 1 ou le kdo 2: "; \
	read number; \
	if [ "$$number" = "1" ]; then \
		echo "\n🎊 Félicitations vous avez gagné une visite sur le chantier d'une autoroute 🥳"; \
	elif [ "$$number" = "2" ]; then \
		echo "\n✨ Félicitations vous venez de gagner une semaine de vacances tout frais payés dans un bidonville 🎉"; \
    else \
		echo "Entrée non reconnue : $$number"; \
    fi

.PHONY: all tg clean fclean re zbeb surpriz
