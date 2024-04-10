<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\Json;

class JsonAttribute extends TextAttribute
{
    private $jsonIndentChar = "&nbsp;&nbsp;&nbsp;&nbsp;";
    private $jsonNewlineChar = "<br />";

    public function display($record, $mode)
    {
        $displayContent = '';
        $fieldContent = $record[$this->fieldName()];

        if ($fieldContent !== null && $fieldContent !== '') {
            $encodedJson = Json::prettify(json_encode($fieldContent, JSON_PRETTY_PRINT), $this->jsonNewlineChar, $this->jsonIndentChar);
            $displayContent = $this->formatDisplay($encodedJson, $mode);
        }

        return $displayContent;
    }

    public function edit($record, $fieldprefix, $mode)
    {
        if (is_array($record[$this->fieldName()])) {
            $record[$this->fieldName()] = json_encode($record[$this->fieldName()] ?: []);
        }

        if ($record[$this->fieldName()]) {
            $record[$this->fieldName()] = Json::prettify($record[$this->fieldName()]);
        }

        return parent::edit($record, $fieldprefix, $mode);
    }

    public function validate(&$record, $mode)
    {
        if (is_array($record[$this->fieldName()])) {
            if (is_null($record[$this->fieldName()])) {
                $record[$this->fieldName()] = [];
            }
            $record[$this->fieldName()] = json_encode($record[$this->fieldName()]);
            if (json_last_error() != JSON_ERROR_NONE) {
                Tools::atkTriggerError($record, $this, 'error_invalid_json');
            }
        }

        if ($record[$this->fieldName()] && !Json::isValid($record[$this->fieldName()])) {
            Tools::atkTriggerError($record, $this, 'error_invalid_json');
        }
    }

    public function db2value($record)
    {
        $val = parent::db2value($record);
        if ($val and Json::isValid($val)) {
            return $val !== Json::EMPTY_STRING ? json_decode($val, true) : '';
        }
        return $val;
    }

    public function value2db(array $record)
    {
        if ($record[$this->fieldName()] and is_array($record[$this->fieldName()])) {
            $record[$this->fieldName()] = json_encode($record[$this->fieldName()]);
        }

        return $record[$this->fieldName()] ? Json::compact($record[$this->fieldName()]) : JSON::EMPTY_STRING;
    }

    /**
     * The character to use for the indentation of the Json string.
     * @return string
     */
    public function getJsonIndentChar(): string
    {
        return $this->jsonIndentChar;
    }

    /**
     *  The character to use for the indentation of the Json string.
     *  You can use a string that has repeated chars if you want. Like: nbsp;nbsp;nbsp;
     *  The method supports html elements too, but be aware that you will have to do the cleaning yourself
     *  if you want to save a minimizzed version on the database (without saving the separators and indentators).
     * @param string $jsonIndentChar
     * @return JsonAttribute
     */
    public function setJsonIndentChar(string $jsonIndentChar): self
    {
        $this->jsonIndentChar = $jsonIndentChar;
        return $this;
    }

    /**
     * @return string
     */
    public function getJsonNewlineChar(): string
    {
        return $this->jsonNewlineChar;
    }

    /**
     * @param string $jsonNewlineChar
     * @return JsonAttribute
     */
    public function setJsonNewlineChar(string $jsonNewlineChar): self
    {
        $this->jsonNewlineChar = $jsonNewlineChar;
        return $this;
    }

    /**
     * @param array|string $array
     * @param int $level
     * @param string $itemSep
     * @return string
     */
    public static function arrayToString($array, int $level = 0, string $itemSep = "<br>"): string
    {
        if (!is_array($array)) {
            if ($array === null) {
                $array = "null";
            } elseif ($array === false) {
                $array = "false";
            }
            return $array . $itemSep;
        }

        $result = '';
        $oldLevel = $level;
        foreach ($array as $key => $value) {
            if ($oldLevel == $level) {
                $level++;
            }
            $result = str_repeat("&nbsp", $level * 3);
            $result .= "<b>" . $key . "</b>: ";
            if (is_array($value)) {
                $result .= "<br>";
            }
            $result .= self::arrayToString($value, $level, $itemSep);
        }

        return $result;
    }
}
