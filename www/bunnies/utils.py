

def createDict(string, sep):
	dict = {}
	if (string != ""):
		for el in string.split(sep):
			splitEl = el.split("=")
			if (len(splitEl) == 1):
				dict[splitEl[0]] = ""
			else:
				dict[splitEl[0]] = splitEl[1]
	return dict