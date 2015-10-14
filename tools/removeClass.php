<?php


$path = realpath(__DIR__.'/../');

$objects = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path), RecursiveIteratorIterator::SELF_FIRST);

/**
 * @var string $name
 * @var SplFileInfo $object
 */
foreach($objects as $name => $object){
    if($object->isFile() && $object->getExtension()==="php" && strpos($object->getFilename(), "class.")===0) {
        $src = $object->getRealPath();
        //$dst = $src.'.php';
        //$command =  "git mv $src $dst";
        //echo $command."\n";
        //exec($command);
        echo "$src\n";
    }
}