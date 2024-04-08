#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import random


def addition(param1, param2):
    try:
        result = int(param1) + int(param2) + random.randint(-10, 10)
        return result
    except ValueError:
        return "Invalid"

if __name__ == "__main__":
    # Récupérer les valeurs depuis les variables d'environnement
    param1 = os.environ.get("num1", "")
    param2 = os.environ.get("num2", "")

    result = addition(param1, param2)

    data = """
    html code
    """
    print(data)
    # Générer la sortie CGI
    print("""
    <html>
    <head>
        <title>CGI Python</title>
        <style>
            body {{
                background-image: url('https://images-na.ssl-images-amazon.com/images/S/pv-target-images/bad033b7710a06cea73dd65b464c31e3cd39c1faa3f26fec7211f960d16940e9._RI_TTW_SX1080_FMjpg_.jpg');
                background-size: cover;
                color: white;
                font-family: Arial, sans-serif;
                text-align: center;
                padding-top: 100px;
            }}
            h1 {{
                color: #ffcc00;
            }}
            p {{
                font-size: 20px;
            }}
        </style>
    </head>
    <body>
        <h1>Addition</h1>
        <p>Result is {}</p>
    </body>
    </html>
    """.format(result))

