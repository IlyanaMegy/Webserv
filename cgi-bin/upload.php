<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $uploadDir = '../www/uploads/';
    $uploadFile = $uploadDir . basename($_FILES['image']['name']);

    if (!file_exists($uploadDir)) {
        mkdir($uploadDir, 0777, true);
    }

    if (move_uploaded_file($_FILES['image']['tmp_name'], $uploadFile)) {
        echo "Image successfully uploaded: " . htmlspecialchars(basename($_FILES['image']['name']));
    } else {
        echo "Error uploading the image.";
    }
} else {
    echo "Invalid request method.";
}
?>
