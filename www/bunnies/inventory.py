import os
import sys
import random
from utils import *

cookiesString = os.environ.get("HTTP_COOKIE", "")
queryString = os.environ.get("QUERY_STRING", "")

cookies = createDict(cookiesString, ";")
query = createDict(queryString, "&")

bunnies = [el[:-4] for el in os.listdir("./bunnies/")]
bunniesCaptured = []

for bunny in bunnies:
	if (bunny in cookies and cookies[bunny] == "found"):
		bunniesCaptured.append(bunny)
	elif (bunny in query and query[bunny] == "found"):
		bunniesCaptured.append(bunny)
		cookies[bunny] = "found"

def getCapturedBunnyHtml(bunnyName):
	return f"""
			<img src="./bunnies/{bunnyName}.gif"/>
"""

def getFullCapturedBunnyHtml(bunniesCaptured):
	html = ""
	for bunny in bunniesCaptured:
		html+=getCapturedBunnyHtml(bunny)
	return html

defaultHTML = f"""
<!DOCTYPE html>
<html>
	<head>
		<title>Inventory</title>
	</head>
	<body>
		<style>
			@import url('https://fonts.googleapis.com/css2?family=DynaPuff:wght@400..700&display=swap');
			@import url('https://fonts.googleapis.com/css2?family=Lexend:wght@100..900&display=swap');
			html, body {{
				height:100%;
			}}
			body {{
				background-image: url(background/forest2.jpg);
				background-size: cover;
				background-repeat: no-repeat;
				overflow: hidden;
			}}
			h1 {{
				text-shadow: 5px 5px black;
				text-align: center;
				font-family: "DynaPuff";
				font-weight:800;
				font-size: 50px;
				color: rgb(20, 211, 131);
			}}
			h3 {{
				margin-left: 50px;
				font-family: "Lexend";
				font-weight:800;
				font-size: 30px;
				color: rgb(0, 120, 42);
			}}
			#inventory {{
				background-color:rgb(0, 168, 104);
				border: 12px solid rgb(1, 186, 118);
				border-radius: 20px;
				height: 650px;
				width: 850px;
				margin-left: auto;
				margin-right: auto;
			}}
		</style>
		<h3>
			<a href="hunt.py">Return to Hunt</a>
		</h3>
		<h1>
			Inventory
		</h1>
		<div id="inventory">{getFullCapturedBunnyHtml(bunniesCaptured)}
		</div>
	</body>
</html>
"""

print("Status: 200")
for c in cookies:
	print("Set-cookie:", end="")
	print(c, end="=")
	print("found")
print("")

print(defaultHTML)

