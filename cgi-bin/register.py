#!/usr/bin/python3
import os
import cgi
import cgitb
from http import cookies

cgitb.enable()  # Pour afficher les erreurs dans le navigateur

# Récupérer les cookies envoyés par le client
cookie = cookies.SimpleCookie(os.environ.get('HTTP_COOKIE', ''))
pseudo = cookie.get('pseudo')

# Vérifier si un formulaire a été soumis
form = cgi.FieldStorage()
new_pseudo = form.getvalue('pseudo')  # Récupérer le pseudo envoyé via POST
# logout = form.getvalue('logout')  # Récupérer la déconnexion si elle est demandée

console.log(new_pseudo)

# Définir les en-têtes HTTP
if new_pseudo:
    print("Content-Type: text/html; charset=UTF-8")
    print(f"Set-Cookie: pseudo={new_pseudo}; Path=/; HttpOnly")
    print()  # Ligne vide obligatoire
    print(f"""
    <!DOCTYPE html>
    <html lang="fr">
    <head>
        <meta charset="UTF-8">
        <title>Inscription réussie</title>
    </head>
    <body>
        <h1>Bienvenue, {new_pseudo} !</h1>
        <a href="/index.html">Retour à l'accueil</a>
    </body>
    </html>
    """)
    exit()
# elif logout:
#     print("Content-Type: text/html; charset=UTF-8")
#     print("Set-Cookie: pseudo=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT")
#     print("Status: 302 Found")
#     print("Location: /index.html")
#     print()  # Ligne vide obligatoire
#     exit()
else:
    print("Content-Type: text/html; charset=UTF-8")
    print()  # Ligne vide obligatoire

# Générer la page HTML
print("""
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Inscription</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js"></script>
    <link rel="stylesheet" href="assets/css/style.css" />
    <link rel="stylesheet" href="assets/css/register.css" />
</head>
<body>
<div class="body_index body_register">
    <nav class="navbar fixed-top">
        <div class="container-fluid">
            <a class="navbar-brand" href="#">NOM</a>
            <div class="d-flex">
                <a href="index.html"><img src="assets/images/home.svg" alt="Go Home" class="navbar-icon"></a>
                <a href="login.html"><img src="assets/images/login.svg" alt="Log in" class="navbar-icon"></a>
            </div>
        </div>
    </nav>
    <div class="wrapper">
        <div class="title"><span>Rejoins nous :)</span></div>
        <form action="/cgi-bin/register.py" method="post">
            <div class="input-box">
                <input type="text" name="pseudo" placeholder="pseudo" required>
            </div>
            <div class="policy">
                <input type="checkbox">
                <h3>je hais ce projet</h3>
            </div>
            <div class="input-box button">
                <input type="submit" value="Tout est OK !">
            </div>
        </form>
    </div>
</div>
</body>
</html>
""")
