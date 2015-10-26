<?php namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\StringParser;

/**
 * The atkFileWriterAttribute is an attribute that reads data from / saves
 * data to a file, instead of the database.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage attributes
 *
 */
class FileWriterAttribute extends TextAttribute
{
    var $m_filename;

    /**
     * Constructor
     * @param String $name Name of the attribute
     * @param String $filename The name of the file to read/write.
     *                         Advanced use: This may be a template containing
     *                         fields from your class, for example:
     *                         "somedir/textfile_[id].txt". If a record has id '9',
     *                         this will read/write a file named
     *                         somedir/textfile_9.txt. Watch out when using fields
     *                         that can change; the attribute won't remove the old
     *                         files.
     * @param int $flags Flags for this attribute
     * @param mixed $size Size of the attribute
     */
    function __construct($name, $filename, $flags = 0, $size = 30)
    {
        $this->m_filename = $filename;

        parent::__construct($name, $size, $flags); // base class constructor
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * For this attribute the data is not written to a database but to a file
     *
     * @param array $record The record that holds this attribute's value.
     * @return String The database compatible value
     */
    function value2db($record)
    {
        // Note1 : regardless of mode (update or add), we always overwrite the 
        // file with the current contents.
        // Note 2: even if contents is "", we write the file, since the file 
        // might exist and contain old information.

        $contents = $record[$this->fieldName()];

        $parser = new StringParser($this->m_filename);

        if (!$parser->isComplete($record)) {
            // record does not contain all data. Let's lazy load.
            Tools::atkdebug("[atkfilewriter] Lazy loading rest of record to complete filename.");
            $record = $this->m_ownerInstance->select($record["atkprimkey"])->getFirstRow();
        }

        $filename = $parser->parse($record);

        $fp = @fopen($filename, "w");

        if ($fp == false) {
            Tools::atkerror("[" . $this->fieldName() . "] couldn't open $filename for writing!");
        } else {
            fwrite($fp, $contents);
            fclose($fp);
            Tools::atkdebug("[" . $this->fieldName() . "] succesfully wrote $filename..");
        }

        return $this->escapeSQL($contents);
    }

    /**
     * Converts a database value to an internal value.
     *
     * For this attribute the value will be read from a file (if possible)
     *
     * @param array $record The database record that holds this attribute's value
     * @return mixed The internal value
     */
    function db2value($record)
    {
        // determine filename.
        $parser = new StringParser($this->m_filename);
        $filename = $parser->parse($record);

        if (!file_exists($filename)) {
            Tools::atkdebug("[" . $this->fieldName() . "] warning: $filename doesn't exist");
            return $record[$this->fieldName()];
        } else {
            if ($record[$this->fieldName()] == "") {
                // db is empty. if file contains stuff, use that.          
                $contents = implode("", file($filename));
                Tools::atkdebug("[" . $this->fieldName() . "] succesfully read $filename");
                return $contents;
            } else {
                return $record[$this->fieldName()];
            }
        }
    }

    /**
     * This attribute does not support any search modes
     *
     * @return array empty array
     */
    function getSearchModes()
    {
        // exact match and substring search should be supported by any database.
        // (the LIKE function is ANSI standard SQL, and both substring and wildcard
        // searches can be implemented using LIKE)
        // Possible values
        //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
        return array();
    }

}


