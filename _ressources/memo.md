``` cpp
enum HttpError {
    OK = 200,
    BAD_REQUEST = 400, // malformed request syntax, invalid request message framing
    METHOD_NOT_ALLOWED = 405, // method is not among GET, POST, or DELETE
    URI_TOO_LONG = 414, // uri exceeds server limits
    HEADER_FIELDS_TOO_LARGE = 431, // header fields are too large
    HTTP_VERSION_NOT_SUPPORTED = 505, // version is not HTTP/1.1
    PAYLOAD_TOO_LARGE = 413, // request body exceeds configured client_body_size
    LENGTH_REQUIRED = 411, // content-Length header is missing for POST requests
    NOT_FOUND = 404, // requested resource doesn't exist
    FORBIDDEN = 403, // access to resource is forbidden
    UNAUTHORIZED = 401 // authentication is required
};
```

Structure pollfd
``` cpp
struct pollfd {
    int fd;         // Le descripteur de fichier à surveiller.
    short events;   // Les événements à surveiller (ex. POLLIN, POLLOUT).
    short revents;  // Les événements qui se sont produits (rempli par poll()).
};
```
events : Indique les types d'événements que vous souhaitez surveiller, tels que :
POLLIN : Prêt à lire (données disponibles à lire).
POLLOUT : Prêt à écrire (tampon disponible pour écrire).
POLLERR : Erreur sur le descripteur.
POLLHUP : Fermeture du descripteur par le pair.
POLLNVAL : Descripteur non valide.
revents : Rempli par poll() pour indiquer quels événements se sont réellement produits.