<?php

namespace Sintattica\Atk\Utils;

use Sintattica\Atk\Core\Tools;
use ArrayAccess;

/**
 * Generic string parser.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class StringParser
{
    public $m_fields = [];
    public $m_string = '';

    /**
     * Create a new stringparser.
     *
     * @param string $string The string to parse
     */
    public function __construct($string)
    {
        $this->m_string = $string;
    }

    /**
     * Parse data into the string.
     *
     * @param array $data The data to parse in the string
     * @param bool $encode Wether or not to do a rawurlencode
     * @param bool $replaceUnknownFields Replace unknown fields with an empty string,
     *                                    if set to false unknown fields will be left
     *                                    untouched.
     *
     * @return string The parsed string
     */
    public function parse($data, $encode = false, $replaceUnknownFields = true)
    {
        $string = $this->m_string;

        $fields = $this->getFields();
        foreach ($fields as $field) {
            $value = $data;

            $elements = explode('.', $field);
            foreach ($elements as $i => $el) {
                if (is_array($value) || $value instanceof ArrayAccess) {
                    if (isset($value[$el])) {
                        $value = $value[$el];
                    } else {
                        if ($replaceUnknownFields) {
                            $value = '';
                            break;
                        } else {
                            // field not found, continue with next field without
                            // replacing the field in the template
                            continue 2;
                        }
                    }
                } else {
                    if (!$replaceUnknownFields) {
                        // field not found, continue with next field without
                        // replacing the field in the template
                        continue 2;
                    }
                }
            }

            if ($encode) {
                $value = rawurlencode($value);
            }

            $string = str_replace('['.$field.']', $value, $string);
        }

        return $string;
    }

    /**
     * Does the data contains everything needed to be parsed into the string?
     *
     * @param array $data
     *
     * @return bool
     */
    public function isComplete($data)
    {
        $fields = $this->getFields();
        for ($i = 0; $i < Tools::count($fields); ++$i) {
            $elements = explode('.', $fields[$i]);
            $databin = $data;
            for ($j = 0; $j < Tools::count($elements); ++$j) {
                $value = $databin[$elements[$j]];
                if (!isset($value)) {
                    return false;
                } // Missing value.
                $databin = $databin[$elements[$j]];
            }
            if (!isset($value)) {
                return false;
            } // Missing value.
        }

        return true;
    }

    /**
     * Get the [ ] Fields out of a String.
     *
     * <b>Example:</b>
     *        string: [firstname], [lastname] [city]
     *        would return array('firstname','lastname','city')
     *
     * @return array
     */
    public function getFields()
    {
        if (empty($this->m_fields)) {
            $matches = [];
            preg_match_all("/\[([^\]]*)\]+/", $this->m_string, $matches);
            $this->m_fields = $matches[1];
        }

        return $this->m_fields;
    }

    /**
     * Get all fields from a string.
     *
     * <b>Example:</b>
     *        string: [firstname], [lastname] [city]
     *        would return array('[firstname]',', ','[lastname]',' ','[city]')
     *
     * @return array
     */
    public function getAllFieldsAsArray()
    {
        $matches = [];
        preg_match_all("/\[[^\]]*\]|[^[]+/", $this->m_string, $matches);

        return $matches[0];
    }

    /**
     * Parse data into the string and return all fields as an array.
     *
     * @param array $data
     * @param bool $split_tags_and_fields return fields and separators separated in resultarray (separators are not used in query, so quotes aren't used)
     *
     * @return array
     */
    public function getAllParsedFieldsAsArray($data, $split_tags_and_fields = false)
    {
        $matches = $this->getAllFieldsAsArray();
        Tools::atk_var_dump($matches, 'MATCHES'.($split_tags_and_fields ? ' (split tags and separators)' : ''));

        $fields = [];
        if (is_array($matches)) {
            foreach ($matches as $match) {
                // Check if need to parse the match
                if (strpos($match, '[') !== false && strpos($match, ']') !== false) {
                    $parser = new self($match);

                    if ($split_tags_and_fields) {
                        $fields['tags'][] = $parser->parse($data);
                    } else {
                        $fields[] = $parser->parse($data);
                    }
                } else {
                    if ($split_tags_and_fields) {
                        $fields['separators'][] = $match;
                    } else {
                        $fields[] = "'".$match."'";
                    }
                }
            }
        }

        return $fields;
    }

    /**
     * Same as getFields but if a relation is referenced using
     * a dot only returns the attribute name before the dot.
     *
     * @return array attributes used in template
     */
    public function getAttributes()
    {
        $attrs = [];

        $fields = $this->getFields();
        foreach ($fields as $field) {
            list($attr) = explode('.', $field);
            $attrs[] = $attr;
        }

        return $attrs;
    }
}
