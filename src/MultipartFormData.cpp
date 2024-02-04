#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"
#include "../include/Route.hpp"
#include "../include/Cgi.hpp"
#include "../include/MultipartFormData.hpp"
#include "../include/HtmlRequest.hpp"

// Constructeur MultipartFormData, but :
// Retirer et stocké dans un vector de struct data
// toutes les informations traités dans les requetes.

MultipartFormData::MultipartFormData(std::string contentType, std::string rawContent)
{
    // extraction de la ligne boundary
    boundary = contentType.substr(contentType.find("boundary=") + 9);
    // creation du delimiteur de début
    std::string delimiter = "--" + boundary;
    // creation du delimiteur de fin
    std::string delimiterEnd;

    // trouver le char de retour si present
	size_t returnPos = boundary.find('\r');

    // si char de retour trouvé
	if (returnPos != std::string::npos)
    {
		// Insérer les tirets avant le char de retour
		delimiterEnd = boundary;
		delimiterEnd.insert(returnPos, "--");
	}
    else
    {
		// Si pas de char de retour, ajouter les tirets à la fin normalement
		delimiterEnd = boundary + "--";
	}

    // mettre les tirets de début
	delimiterEnd = "--" + delimiterEnd;

    // Transforme notre string "rawContent"
    // pour pouvoir etre lu par getline()
    std::istringstream rawStream(rawContent);
    std::string line;
    // struct : name, filname, content.
    contentData data;
    
    while (getline(rawStream, line, '\n'))
    {
        // Si l'on est au début du fichier
        if (line == delimiter)
        {
            // Si data.name existe
            if (data.name != "")
            {
                if (data.content.size() >= 4)
                {
                    // extrait content sans les 2 chars de debut et de fin
                    data.content = data.content.substr(2, data.content.size() - 4);
                }
                content.push_back(data);
                // Reset data for next content block
                data = contentData();
            }
            continue;
        }
        else if (!line.find(delimiterEnd)) // Si on est sur la fin du fichier
        {
            // Si data.name non vide
            if (data.name != "")
            {
                if (data.content.size() >= 4)
                {
                    // extrait content sans les 2 chars de debut et de fin
                    data.content = data.content.substr(2, data.content.size() - 4);
                }
                content.push_back(data); // ajout de la structure dans le vector
            }
            break;
        }
        else if (line.find("Content-Disposition: form-data; name=") != std::string::npos)
        {
            // remplir notre structure des infos de content dispo
            data.name = line.substr(line.find("name=") + 6);
            data.name = data.name.substr(0, data.name.find("\""));
            data.filename = line.substr(line.find("filename=") + 10);
            data.filename = data.filename.substr(0, data.filename.find("\""));
            // Skip the next line (Content-Type or empty)
            getline(rawStream, line, '\n');
        }
        else
        {
            data.content += line + "\n";
        }
    }
}

// [ SETTERS ] //

void MultipartFormData::setBoundary(std::string boundary)
{
    this->boundary = boundary;
}

void MultipartFormData::setContent(std::vector<contentData> content)
{
    this->content = content;
}

// [ GETTERS ] //

std::string MultipartFormData::getBoundary()
{
    return (boundary);
}

std::string MultipartFormData::getContent()
{
    return (content);
}