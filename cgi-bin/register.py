#!/usr/bin/python3

import cgi			# handle the form data
import os			# handle the file system
import pickle		# handle the pickle for database quick saves
import hashlib		# handle the hash for the id
import time			# handle the time because we're god-like :)

class UserDB:
	def __init__(self):
		self.users = {}

	def addUser(self, username, id):
		if id in self.id:
			raise ValueError("User ID already exists")
		self.users[user_id] = username
        with open('user_database', 'wb') as f:
            pickle.dump(self, f)

def loadUserDB():
	if os.path.exists('user_database'):
		with open('user_database', 'rb') as f:
			return pickle.load(f)
	return UserDB()

def main():
	form = cgi.FieldStorage()
	username = form.getvalue('pseudo')
	
	print("Content-Type: text/html")
	print()

	if not username:
		print("<h1>Donne un nom poto</h1>")
		return

	database = loadUserDB()

	if username in database.usernames:
		print("<h1>Pseudo déjà pris, t'as pas d'inspi ?</h1>")
	else:
		user_id = hashlib.sha256(str(time.time()).encode()).hexdigest()
        database.addUser(user_id, username)
		print("<h1>Enchanté {}</h1>".format(username))

if __name__ == "__main__":
    main()
