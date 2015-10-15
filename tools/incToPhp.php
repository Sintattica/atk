<?php


$path = realpath(__DIR__ . '/../');

$objects = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path), RecursiveIteratorIterator::SELF_FIRST);

/**
 * @var string $name
 * @var SplFileInfo $object
 */
foreach ($objects as $name => $object) {
    if ($object->isFile() && $object->getExtension() === 'inc') {
        $src = $object->getRealPath();
        $dst = substr($src, 0, -3) . 'php';
        $command = "git mv $src $dst";
        echo $command . "\n";
        exec($command);
    }
}