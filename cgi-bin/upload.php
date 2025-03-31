<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        error_log("Request method: POST");
        error_log("FILES array: " . print_r($_FILES, true));
    
        if (isset($_FILES['image'])) {
            error_log("File upload detected.");
            error_log("File upload error code: " . $_FILES['image']['error']);
        } else {
            error_log("No file uploaded.");
        }
    }
    if (isset($_FILES['image']) && $_FILES['image']['error'] === UPLOAD_ERR_OK) {
        $uploadDir = '../www/uploads/';
        $uploadFile = $uploadDir . basename($_FILES['image']['name']);

        error_log("Uploading image to: " . $uploadFile);

        if (!file_exists($uploadDir)) {
            mkdir($uploadDir, 0777, true);
        }

        if (move_uploaded_file($_FILES['image']['tmp_name'], $uploadFile)) {
            echo "Image successfully uploaded: " . htmlspecialchars(basename($_FILES['image']['name']));
        } else {
            echo "Error uploading the image.";
        }
    } else {
        echo "No file uploaded or an error occurred.";
        error_log("File upload error: " . $_FILES['image']['error']);
    }
} else {
    echo "Invalid request method.";
}
?>
