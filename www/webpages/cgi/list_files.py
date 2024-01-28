#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os

def list_files(directory):
    try:
        files = os.listdir(directory)
        html = "<ul>"
        for file in files:
            html += "<li>{}</li>".format(file)
        html += "</ul>"
        return html
    except Exception as e:
        return "Erreur lors de la lecture du dossier : {}".format(e)

if __name__ == "__main__":
    # Récupérer le chemin du dossier depuis les variables d'environnement
    base_directory = os.environ.get("PWD", "")
    base_directory = os.path.join(base_directory, "www", "webpages")
    directory = os.environ.get("directory", "")
    directory = os.path.join(base_directory, directory)

    html_files = list_files(directory)

    # Générer la sortie CGI
    print("Content-type: text/html; charset=utf-8\n")
    print("<html><head><title>Liste de Fichiers</title></head><body>")
    print("<h1>Liste de Fichiers dans le Dossier</h1>")
    print(html_files)
    print("</body></html>")
