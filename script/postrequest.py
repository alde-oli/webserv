# -*- coding: utf-8 -*-

import urllib
import urllib2

# URL de destination
url = 'http://127.0.0.1:2020/'

# Données à envoyer en POST sous forme de dictionnaire
post_data = {'parametre1': 'valeur1', 'parametre2': 'valeur2'}

# Encodage des données POST
data = urllib.urlencode(post_data)

# Création de la requête POST
request = urllib2.Request(url, data)

# Envoi de la requête POST
try:
    response = urllib2.urlopen(request)
    print('Requête POST réussie!')
    # Vous pouvez lire la réponse du serveur en utilisant response.read()
except urllib2.HTTPError as e:
    print('Échec de la requête POST - Code de statut HTTP:', e.code)
    print('Message d\'erreur:', e.read())
except urllib2.URLError as e:
    print('Erreur de l\'URL:', e)

    while True:
            data = s.recv(64)
            if not data:
                break
            print "Données reçues:", data
            time.sleep(read_interval)
