# -*- coding: utf-8 -*-
import socket
import threading
import time

# Paramètres du serveur et du client
HOST = '127.0.0.1'
PORT = 2020
NUM_CLIENTS = 50
KEEP_ALIVE_DURATION = 60  # Durée en secondes pour garder les connexions ouvertes
REQUEST_MESSAGE = "GET /index.html HTTP/1.1\r\nHost: {}\r\n\r\n".format(HOST)
THREAD_START_DELAY = 0.5  # Délai en secondes entre le lancement des threads

open_sockets = []

def simple_client():
    """Un client TCP qui envoie une requête GET et garde la session ouverte."""
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    print("Connecté au serveur.")
    try:
        s.sendall(REQUEST_MESSAGE)
        open_sockets.append(s)
    except Exception as e:
        print("Erreur:", e)

def create_clients():
    """Crée plusieurs clients qui se connectent et envoient une requête."""
    threads = []

    for _ in range(NUM_CLIENTS):
        thread = threading.Thread(target=simple_client)
        thread.start()
        threads.append(thread)
        time.sleep(THREAD_START_DELAY)  # Attendre avant de démarrer le prochain thread

    for thread in threads:
        thread.join()

    time.sleep(KEEP_ALIVE_DURATION)

    for s in open_sockets:
        s.close()

if __name__ == '__main__':
    create_clients()

