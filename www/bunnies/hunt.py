import os
import sys
import random
from utils import *

cookiesString = os.environ.get("HTTP_COOKIE", "")
queryString = os.environ.get("QUERY_STRING", "")

cookies = createDict(cookiesString, ";")
query = createDict(queryString, "&")

bunnies = [el[:-4] for el in os.listdir("./bunnies/")]
bunniesToPrint = bunnies.copy()
bunnyToCongratulate = ""

for bunny in bunnies:
	if bunny in cookies and cookies[bunny] == "found":
		bunniesToPrint.remove(bunny)
	elif bunny in query and query[bunny] == "found":
		bunnyToCongratulate = bunny
		bunniesToPrint.remove(bunny)
		cookies[bunny] = "found"

background = "background/forest2.jpg"

def getBunnyHtml(bunnyName):
	return f"""
		<style>
			#{bunnyName} {{
				position: absolute;
				background-image: url("bunnies/{bunnyName}.gif");
				background-size: contain;
				background-repeat: no-repeat;
				background-position: center;
				background-color: rgba(0,0,0,0);
				border: 0;
				width: 150px;
				height: 150px;
			}}
		</style>
		<form>
			<input type="hidden" name="{bunnyName}" value="found">
			<input type="submit" id="{bunnyName}" value="">
		</form>
		<script>
			{bunnyName}.style.left = (Math.random() * window.innerWidth - {bunnyName}.clientWidth / 2)+"px";
			{bunnyName}.style.top = (Math.random() * window.innerHeight - {bunnyName}.clientHeight / 2)+"px";
		</script>
"""

def getFullBunnyHtml(bunniesToPrint):
	html = ""
	for i, bunny in enumerate(bunniesToPrint):
		if random.randint(0, i + 1) == 0:
			html+=getBunnyHtml(bunny)
	return html

def getCongratulationHtml(bunnyName):
	if (bunnyName == ""):
		return ""
	return f"""
		<style>
			@import url('https://fonts.googleapis.com/css2?family=DynaPuff:wght@400..700&display=swap');
			h2 {{
				text-shadow: 5px 5px black;
				text-align: center;
				font-size: 40px;
				font-family: "DynaPuff";
				color: rgb(20, 211, 131);
			}}
		</style>
		<h2>
			You have caught the {bunnyName.replace("_", " ")}!!
		</h2>
"""

def getCompletionHtml():
	return f"""
		<style>
			@import url('https://fonts.googleapis.com/css2?family=DynaPuff:wght@400..700&display=swap');
			h1 {{
				text-shadow: 5px 5px black;
				text-align: center;
				margin-top: 300px;
				font-size: 50px;
				font-family: "DynaPuff";
				color: rgb(32, 210, 210);
			}}
		</style>
		<h1>
			Congratulations!<br />
			You have caught all bunnies!
		</h1>
"""

meta = 	f"""
	<meta http-equiv="refresh" content="1" />
"""

defaultHTML = f"""
<!DOCTYPE html>
<html>
{""}
	<head>
		<title>Bunny Hunt</title>
	</head>
	<body>
		<style>
			@import url('https://fonts.googleapis.com/css2?family=Lexend:wght@100..900&display=swap');
			html, body {{
				height:100%;
			}}
			body {{
				background-image: url({background});
				background-size: cover;
				background-repeat: no-repeat;
				overflow: hidden;
			}}
			h3 {{
				margin-left: 50px;
				font-family: "Lexend";
				font-weight:800;
				font-size: 30px;
				color: rgb(0, 120, 42);
			}}
		</style>
		<h3>
			<a href="inventory.py">Inventory</a>
		</h3>
{getCongratulationHtml(bunnyToCongratulate)}{getCompletionHtml() if bunniesToPrint == [] else ""}
{getFullBunnyHtml(bunniesToPrint)}
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

