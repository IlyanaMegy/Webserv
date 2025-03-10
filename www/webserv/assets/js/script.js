var userName = null;
document.addEventListener("DOMContentLoaded", function() {
	var header = document.getElementById("greetings");
    if (userName && userName != "") {
        header.textContent = userName;
    } else {
        header.textContent = "Stranger";
    }
	header.classList.add("animate-grow");
});