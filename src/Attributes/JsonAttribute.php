<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\Json;

class JsonAttribute extends TextAttribute
{
    private string $jsonIndentChar = "&nbsp;&nbsp;&nbsp;&nbsp;";
    private string $jsonNewlineChar = "<br />";
    private bool $isRawModeEnabled = false;

    public function display(array $record, string $mode): string
    {
        $fieldContent = $record[$this->fieldName()] ?? null;
        if ($fieldContent === null || $fieldContent === '') {
            return '';
        }

        $asArray = null;
        if (is_array($fieldContent)) {
            $asArray = $fieldContent;
        } elseif (is_string($fieldContent) && Json::isValid($fieldContent)) {
            $decoded = json_decode($fieldContent, true);
            if (is_array($decoded)) {
                $asArray = $decoded;
            }
        }

        if (is_array($asArray) && !$this->isRawModeEnabled) {
            $encodedJson = $this->renderKeyValueList($asArray);
        } else {
            // default: JSON prettified
            $encodedJson = Json::prettify(json_encode($asArray, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE), $this->jsonNewlineChar, $this->jsonIndentChar);
        }

        return $this->formatDisplay($encodedJson, $mode);
    }

    public function edit($record, $fieldprefix, $mode)
    {
        if (isset($record[$this->fieldName()])) {
            if (is_array($record[$this->fieldName()])) {
                $record[$this->fieldName()] = json_encode($record[$this->fieldName()] ?: []);
            }
            if ($record[$this->fieldName()]) {
                $record[$this->fieldName()] = Json::prettify($record[$this->fieldName()]);
            }
        }

        return parent::edit($record, $fieldprefix, $mode);
    }

    public function validate(&$record, $mode)
    {
        if (isset($record[$this->fieldName()])) {
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
        if ($record[$this->fieldName()] && is_array($record[$this->fieldName()])) {
            $record[$this->fieldName()] = json_encode($record[$this->fieldName()]);
        }

        return $record[$this->fieldName()] ? Json::compact($record[$this->fieldName()]) : JSON::EMPTY_STRING;
    }

    /**
     * The character to use for the indentation of the Json string
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

    public function getJsonNewlineChar(): string
    {
        return $this->jsonNewlineChar;
    }

    public function setJsonNewlineChar(string $jsonNewlineChar): self
    {
        $this->jsonNewlineChar = $jsonNewlineChar;
        return $this;
    }

    public function isRawModeEnabled(): bool
    {
        return $this->isRawModeEnabled;
    }

    public function enableRawMode(): self
    {
        $this->isRawModeEnabled = true;
        return $this;
    }

    public function disableRawMode(): self
    {
        $this->isRawModeEnabled = false;
        return $this;
    }

    public static function arrayToString(array|bool|string|null $array, int $level = 0, string $itemSep = "<br />"): string
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
            $result .= "<strong>" . $key . "</strong>: ";
            if (is_array($value)) {
                $result .= $itemSep;
            }
            $result .= self::arrayToString($value, $level, $itemSep);
        }

        return $result;
    }

    /**
     * Recursive rendering in "key: value" format with indentation and line separator.
     * Avoids the problems of the base class's arrayToString that overwrites the output.
     */
    private function renderKeyValueList(array $value, int $level = 0): string
    {
        $indent = str_repeat($this->jsonIndentChar, $level * 3);

        $lines = [];
        foreach ($value as $key => $val) {
            $isAssocArray = !is_numeric($key);
            // adds the key only for assoc arrays
            $keyHtml = $isAssocArray
                ? '<strong>' . $this->text($this->escapeScalar($key)) . '</strong>: '
                : '';
            if (is_array($val)) {
                $lines[] = $indent . $keyHtml;
                $lines[] = $this->renderKeyValueList($val, $level + 1);
            } else {
                $lines[] = $indent . $keyHtml . $this->escapeScalar($val);
            }
        }
        return implode($this->jsonNewlineChar, $lines);
    }

    private function escapeScalar($val): string
    {
        if ($val === null) {
            return 'null';
        }
        if ($val === true) {
            return 'true';
        }
        if ($val === false) {
            return 'false';
        }
        // Keep symbols like °C; no special HTML conversion beyond basic
        return htmlspecialchars((string)$val, ENT_QUOTES | ENT_SUBSTITUTE, 'UTF-8');
    }
}
