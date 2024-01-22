# -*- coding: utf-8 -*-
import socket
import time

def slow_client(host, port, read_interval, request_message):
    """Un client TCP qui envoie une requête et lit les données lentement.

    Args:
    host (str): L'adresse IP du serveur.
    port (int): Le numéro de port du serveur.
    read_interval (int): L'intervalle en secondes entre les lectures.
    request_message (str): Message à envoyer au serveur.
    """
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    print "Connecté au serveur."

    try:
        # Envoyer une requête au serveur
        s.sendall(request_message)

        while True:
            data = s.recv(64)
            if not data:
                break
            print "Données reçues:", data
            time.sleep(read_interval)
    finally:
        s.close()

# Remplacez '127.0.0.1' et 8080 par l'adresse et le port de votre serveur.
# Modifiez également read_interval pour contrôler la lenteur de la lecture.
# Définissez la requête à envoyer au serveur
request_message = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n"
slow_client('127.0.0.1', 8080, read_interval=1, request_message=request_message)
