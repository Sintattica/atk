<?php

$finder = Symfony\CS\Finder\DefaultFinder::create()
    ->exclude(
        array(
            'Resources',
        )
    )
    ->in(__DIR__.'/src');

return Symfony\CS\Config\Config::create()
    ->level(Symfony\CS\FixerInterface::PSR2_LEVEL)
    ->fixers(array('-phpdoc_params', 'concat_without_spaces', 'duplicate_semicolon'))
    ->finder($finder);