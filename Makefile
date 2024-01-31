NAME = MaisQuelServeurIncroyableIlEstVraimentTropBienLesGensQuiLOntFaitSontVraimentDesGenies

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCDIR = src
INCDIR = include
OBJDIR = obj

GREEN = \033[0;32m
BLUE = \033[0;34m
VIOLET = \033[0;35m
BOLD = \033[1m
NC = \033[0m

SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

all: $(NAME)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(NAME): $(OBJDIR) $(OBJECTS)
	@echo -e "$(GREEN)$(BOLD)[ ok ] files compiled$(NC)"
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJECTS) -I$(INCDIR) -g -D PORT=8080
	@./$(NAME) || (echo -e "$(RED)$(BOLD)[ error ] $(NAME) failed with PORT=8080, retrying with PORT=4242...$(NC)" && \
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJECTS) -I$(INCDIR) -g -D PORT=4242 && ./$(NAME))
	@echo -e "$(BLUE)$(BOLD)[ ok ] $(NAME) executed successfully$(NC)"


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)
	@echo -e "$(VIOLET)$(BOLD)obj files deleted$(NC)"

fclean: clean
	@rm -f $(NAME)
	@echo -e "$(VIOLET)$(BOLD)program deleted$(NC)"

re: fclean all

run: re
	@./$(NAME)

.PHONY: all clean fclean re run