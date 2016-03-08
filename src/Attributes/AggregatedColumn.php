<?php namespace Sintattica\Atk\Attributes;


use Sintattica\Atk\Utils\StringParser as StringParser;
use Sintattica\Atk\Core\Tools as Tools;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\RecordList\RecordList;
use Sintattica\Atk\Core\Config;

/**
 * The AggregatedColumn aggregates multiple attributes to one colunm in
 * list view. The attribute displays and sorts according to the $template
 * parameter and searches in fields, indicated in $searchfields array
 * parameter.
 * This attribute shows in recordlist only.
 *
 * @author Yury Golovnya <ygolovnya@kyiv.utel.com.ua>
 * @package atk
 * @subpackage attributes
 *
 */
class AggregatedColumn extends Attribute
{
    /**
     * The display/sort template
     * @var String
     * @access private
     */
    var $m_template;

    /**
     * The array with searchs fileds
     * @var array
     * @access private
     */
    var $m_searchfields = array();

    /**
     * The array with displays fileds
     * @var array
     * @access private
     */
    var $m_displayfields = array();

    /**
     * Constructor
     * @param string $name Name of the attribute
     * @param string $template Display/sort template.
     * @param int $flags Flags for this attribute
     * @param array $searchfields Array with fields, in which search will be perform
     *                             If ommited, fields from $template will be used
     */
    function __construct($name, $template, $flags = 0, $searchfields = array())
    {
        parent::__construct($name, $flags | self::AF_HIDE_EDIT | self::AF_HIDE_ADD | self::AF_HIDE_VIEW); // base class constructor
        $this->m_template = $template;

        $parser = new StringParser($template);
        $this->m_displayfields = $parser->getFields();

        if (!is_array($searchfields)) {
            $this->m_searchfields = $this->m_displayfields;
        } else {
            $this->m_searchfields = $searchfields;
        }
    }

    /**
     * The display function for this attribute
     *
     * @param array $record The record that holds the value for this attribute
     * @param string $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists, "edit" for
     *                     displaying in editscreens, "add" for displaying in
     *                     add screens. "csv" for csv files. Applications can
     *                     use additional modes.
     * @return string html code to display the value of this attribute
     */
    function display($record, $mode)
    {
        $rec = array();
        foreach ($this->m_displayfields as $field) {

            $p_attrib = $this->m_ownerInstance->getAttribute($field);

            $rec[$field] = $p_attrib->display($record[$this->fieldName()], $mode);
        }
        $parser = new StringParser($this->m_template);
        return $parser->parse($rec);
    }

    /**
     * Adds the attribute / field to the list header. This includes the column name and search field.
     *
     * @param string $action the action that is being performed on the node
     * @param array $arr reference to the the recordlist array
     * @param string $fieldprefix the fieldprefix
     * @param int $flags the recordlist flags
     * @param array $atksearch the current ATK search list (if not empty)
     * @param string $atkorderby Order by string
     * @see Node::listArray
     */
    function addToListArrayHeader($action, &$arr, $fieldprefix, $flags, $atksearch, $atkorderby)
    {
        if (!$this->hasFlag(self::AF_HIDE_LIST) && !($this->hasFlag(self::AF_HIDE_SELECT) && $action == "select")) {
            $arr["heading"][$fieldprefix . $this->fieldName()]["title"] = $this->label();

            if (!Tools::hasFlag($flags, RecordList::RL_NO_SORT) && !$this->hasFlag(self::AF_NO_SORT)) {
                $rec = array();
                foreach ($this->m_displayfields as $field) {
                    $rec[] = $this->m_ownerInstance->m_table . "." . $field;
                }
                $order = implode(", ", $rec);
                if ($atkorderby == $order) {
                    $order = implode(" DESC,", $rec);
                    $order .= " DESC";
                }
                $sm = SessionManager::getInstance();
                $arr["heading"][$fieldprefix . $this->fieldName()]["url"] = $sm->sessionUrl(Config::getGlobal('dispatcher') . '?atknodeuri=' . $this->m_ownerInstance->atkNodeUri() . '&atkaction=' . $action . '&atkorderby=' . rawurlencode($order));
            }

            if (!Tools::hasFlag($flags, RecordList::RL_NO_SEARCH) && $this->hasFlag(self::AF_SEARCHABLE)) {
                $arr["search"][$fieldprefix . $this->fieldName()] = $this->search($atksearch, false, $fieldprefix);
                $arr["search"][$fieldprefix . $this->fieldName()] .= '<input type="hidden" name="atksearchmode[' . $this->fieldName() . ']" value="' . $this->getSearchMode(false) . '">';
            }
        }
    }

    /**
     * We do not want this attribute to store anything in the database, so we implement an empty store function
     *
     * @return boolean to indicate if store went succesfull
     */
    function store()
    {
        return true;
    }


    function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        if ($mode !== 'add' && $mode != 'edit') {
            $allfields = Tools::atk_array_merge($this->m_displayfields, $this->m_searchfields);
            $alias = $fieldaliasprefix . $this->fieldName() . "_AE_";
            foreach ($allfields as $field) {
                /** @var Attribute $p_attrib */
                $p_attrib = $this->m_ownerInstance->m_attribList[$field];
                $p_attrib->addToQuery($query, $tablename, $alias, $record, $level, $mode);
            }
        }
    }

    /**
     * Creates a search condition for a given search value, and adds it to the
     * query that will be used for performing the actual search.
     *
     * @param Query $query The query to which the condition will be added.
     * @param string $table The name of the table in which this attribute
     *                      is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
     * @param string $fieldaliasprefix optional prefix for the fiedalias in the table
     */
    function searchCondition(&$query, $table, $value, $searchmode, $fieldaliasprefix = '')
    {
        $searchcondition = $this->getSearchCondition($query, $table, $value, $searchmode);
        if (!empty($searchcondition)) {
            $query->addSearchCondition($searchcondition);
        }
    }

    function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        $searchconditions = array();
        // Get search condition for all searchFields
        foreach ($this->m_searchfields as $field) {
            $p_attrib = $this->m_ownerInstance->getAttribute($field);

            if (is_object($p_attrib)) {
                $condition = $p_attrib->getSearchCondition($query, $table, $value, $searchmode);
                if (!empty($condition)) {
                    $searchconditions[] = $condition;
                }
            }
        }

        // When searchmode is substring also search the value in a concat of all searchfields
        if ($searchmode == 'substring') {
            $value = $this->escapeSQL(trim($value));

            $data = array();
            foreach ($this->m_searchfields as $field) {
                if (strpos($field, '.') == false) {
                    $data[$field] = $table . "." . $field;
                } else {
                    $data[$field] = $field;
                }
            }

            $parser = new StringParser($this->m_template);
            $concatFields = $parser->getAllParsedFieldsAsArray($data, true);
            $concatTags = $concatFields['tags'];
            $concatSeparators = $concatFields['separators'];

            // to search independent of characters between tags, like spaces and comma's,
            // we remove all these separators (defined in the node with new atkAggregatedColumn)
            // so we can search for just the concatenated tags in concat_ws [Jeroen]
            foreach ($concatSeparators as $separator) {
                $value = str_replace($separator, "", $value);
            }

            $db = $this->getDb();
            $condition = "UPPER(" . $db->func_concat_ws($concatTags, "", true) . ") LIKE UPPER('%" . $value . "%')";

            $searchconditions[] = $condition;
        }
        return "(" . implode(" OR ", $searchconditions) . ")";
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * @return array List of supported searchmodes
     */
    function getSearchModes()
    {
        return array("exact", "substring", "wildcard", "regexp");
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.
     */
    function dbFieldType()
    {
        return "";
    }

}

