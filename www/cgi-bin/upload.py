#!/usr/bin/env python3

import cgi
import os
import cgitb
import sys

cgitb.enable()  # Pour afficher les erreurs dans le navigateur

# Afficher le répertoire de travail actuel


try:
    UPLOAD_DIR = "../uploads/"

    if not os.path.exists(UPLOAD_DIR):
        os.makedirs(UPLOAD_DIR)

    form = cgi.FieldStorage()
    if "image" in form:
        file_item = form["image"]

        # Vérifier si un fichier a été envoyé
        if file_item.filename:
            filename = os.path.basename(file_item.filename)
            filepath = os.path.join(UPLOAD_DIR, filename)

            # Écrire le fichier sur le serveur
            with open(filepath, "wb") as f:
                f.write(file_item.file.read())

            print("Status: 303 See Other")
            print("Location: ../site/confirmation.html")
            print("Content-Type: text/html")
            print()
        else:
            print("Content-Type: text/html")
            print()
            print("<html><body><h1>No file uploaded</h1></body></html>")
    else:
        print("Content-Type: text/html")
        print()
        print("<html><body><h1>Invalid request</h1></body></html>")
except Exception as e:
    print("Content-Type: text/html")
    print()
    print(f"<html><body><h1>Error: {e}</h1></body></html>")
