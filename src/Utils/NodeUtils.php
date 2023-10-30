<?php

namespace Sintattica\Atk\Utils;

class NodeUtils
{

    public static function getAtkModuleAndNodeFromUrl(string $url): array
    {
        $queryString = parse_url($url, PHP_URL_QUERY);
        parse_str($queryString, $queryParameters);

        if (isset($queryParameters['atknodeuri'])) {
            return explode(".", $queryParameters['atknodeuri']);
        }

        return [];
    }

}