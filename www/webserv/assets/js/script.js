document.addEventListener("DOMContentLoaded", function() {
    var header = document.getElementById("username");
    if (userName && userName != "") {
        header.textContent = userName;
    } else {
        header.textContent = "Stranger";
    }
    document.getElementById("greetings").classList.add("animate-grow");
});