<?php
echo "testttt"."\n";
print_r($_SERVER);
var_dump($_POST);
$post = file_get_contents('php://input');
var_dump($post);
?>
