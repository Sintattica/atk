<?php

$clean = trim($argv[1]);
echo "clean: $clean\n";
echo "hash: ".password_hash($clean, PASSWORD_DEFAULT)."\n";