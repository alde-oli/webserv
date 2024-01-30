#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

int check_method(const std::string& single_method)
{
    if (single_method == "GET" || single_method == "POST" || single_method == "DELETE")
	{
        return 1;
    }
	else
	{
        return 0;
    }
}

std::vector<std::string> set_method(const std::string& method)
{
    std::vector<std::string> stock;
    std::stringstream read_method(method);
    std::string tmp;

	if (method.empty())
	{
			std::cout << "Error: method is empty... :( " << std::endl;
            exit(1);
	}
    while (std::getline(read_method, tmp, ' '))
	{
        if (check_method(tmp) == 1)
		{
            // Vérifier si la méthode est déjà dans le vecteur
            if (std::find(stock.begin(), stock.end(), tmp) != stock.end())
			{
                std::cout << "Error: Tu as mis deux fois la meme methode, la shouma sur toi et sur toute ta famille\n #looooooser \n #ouloulou" << std::endl;
                exit(1);
            }
			else
			{
                stock.push_back(tmp);
            }
        }
		else
		{
            std::cout << "Error: Bad method input: " << method << std::endl;
            exit(1);
        }
    }
    return stock;
}

// int main()
// {
//     std::string input = "";
//     std::vector<std::string> result = set_method(input);

//     // Afficher les résultats
//     for (const auto& element : result) {
//         std::cout << "Méthode : " << element << std::endl;
//     }

//     return 0;
// }
