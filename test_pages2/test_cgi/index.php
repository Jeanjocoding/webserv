<?php
echo "SERVER/CGI variables: \n";
print_r($_SERVER);
echo "\n POST variables: \n";
var_dump($_POST);

echo "\n POST body: \n";
$post = file_get_contents('php://input');
var_dump($post);
?>
