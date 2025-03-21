#!/usr/bin/python3

import cgi
import os
import pickle
import json

class UserDB:
    def __init__(self):
        self.users = {}

    def addUser(self, user_id, username):
        self.users[user_id] = username
        with open('user_database', 'wb') as f:
            pickle.dump(self, f)

def loadUserDB():
    if os.path.exists('user_database'):
        with open('user_database', 'rb') as f:
            return pickle.load(f)
    return UserDB()

def main():
    # Simuler l'ID de session de l'utilisateur connecté
    session_user_id = "user123"  # Remplacez par la logique réelle pour obtenir l'ID de session

    print("Content-Type: application/json")
    print()

    database = loadUserDB()

    # Récupérer le pseudo de l'utilisateur connecté
    user_display = database.users.get(session_user_id, "Stranger")

    # Retourner les données au format JSON
    print(json.dumps({"userName": user_display}))

if __name__ == "__main__":
    main()