<?php

namespace Sintattica\Atk\Attributes;

use Exception;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\RecordList\ColumnConfig;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Utils\EditFormModifier;
use Sintattica\Atk\Utils\Json;

/**
 * The Attribute class represents an attribute of an Node.
 * An Attribute has a name and a set of parameters that
 * control its behaviour, like whether an Attribute
 * is obligatory, etc.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class Attribute
{
    /**
     * Attributeflags. The following flags can be used for attributes.
     *
     * @internal WARNING: flags may *not* exceed 2^31 (2147483648), because
     * that's the integer limit beyond which the bitwise operators won't
     * work anymore!
     */
    /**
     * Value must be entered.
     *
     * "database-level" processing flag
     */
    const AF_OBLIGATORY = 1;

    /**
     * Value must be unique.
     *
     * "database-level" processing flag
     */
    const AF_UNIQUE = 2;

    /**
     * Part of the primary-key node, also makes it obligatory.
     *
     * "database-level" processing flag
     */
    const AF_PRIMARY = 5;

    /**
     * Auto-increment field.
     *
     * "database-level" processing flag
     */
    const AF_AUTO_INCREMENT = 8;

    /**
     * Alias for self::AF_AUTO_INCREMENT (auto-increment flag is often mistyped).
     *
     * "database-level" processing flag
     */
    const AF_AUTOINCREMENT = 8;

    /**
     * Don't show in record lists.
     *
     * hide flag
     */
    const AF_HIDE_LIST = 16;

    /**
     * Don't show on add pages.
     *
     * hide flag
     */
    const AF_HIDE_ADD = 32;

    /**
     * Don't show on edit pages.
     *
     * hide flag
     */
    const AF_HIDE_EDIT = 64;

    /**
     * Don't show on select pages.
     *
     * hide flag
     */
    const AF_HIDE_SELECT = 128;

    /**
     * Don't show on view pages.
     *
     * hide flag
     */
    const AF_HIDE_VIEW = 256;

    /**
     * Not searchable in extended search.
     *
     * hide flag
     */
    const AF_HIDE_SEARCH = 512; // not searchable in extended search

    /**
     * Load always, even if not displayed anywhere.
     *
     * hide flag
     */
    const AF_FORCE_LOAD = 1024; // load always, even if not displayed anywhere

    /**
     * Attribute is totally hidden.
     *
     * hide flag
     * self::AF_HIDE_EDIT | self::AF_HIDE_ADD | self::AF_HIDE_LIST | self::AF_HIDE_SEARCH | self::AF_HIDE_VIEW | self::AF_HIDE_SELECT
     */
    const AF_HIDE = 1008;

    /**
     * Readonly in add.
     *
     * readonly flag
     */
    const AF_READONLY_ADD = 2048;

    /**
     * Readonly when edited.
     *
     * readonly flag
     */
    const AF_READONLY_EDIT = 4096;

    /**
     * Always readonly.
     *
     * readonly flag
     * self::AF_READONLY_EDIT | self::AF_READONLY_ADD;
     */
    const AF_READONLY = 6144;

    /**
     * No label in forms.
     *
     * display-related processing flag
     */
    const AF_NO_LABEL = 8192;

    /**
     * Alias for self::AF_NO_LABEL (mistyped).
     *
     * display-related processing flag
     */
    const AF_NOLABEL = 8192;

    /**
     * Blank label in forms.
     *
     * display-related processing flag
     */
    const AF_BLANK_LABEL = 16384;

    /**
     * Alias for self::AF_BLANK_LABEL (mistyped).
     *
     * display-related processing flag
     */
    const AF_BLANKLABEL = 16384;

    /**
     * Cannot be sorted in recordlists.
     *
     * display-related processing flag
     */
    const AF_NO_SORT = 32768;

    /**
     * Alias for self::AF_NO_SORT (mistyped).
     *
     * display-related processing flag
     */
    const AF_NOSORT = 32768;

    /**
     * Attribute is searchable in list views.
     *
     * display-related processing flag
     */
    const AF_SEARCHABLE = 65536;

    /**
     * The attribute will have a 'total' column in lists.
     *
     * display-related processing flag
     */
    const AF_TOTAL = 131072;

    /**
     * If supported, use pop-up window.
     *
     * display-related processing flag
     */
    const AF_POPUP = 262144;

    /**
     * Delete function is called when owning node is deleted.
     *
     * miscellaneous processing flag
     */
    const AF_CASCADE_DELETE = 524288;

    /**
     * Will have a large amount of recors (relation).
     *
     * Instead of showing a listbox with all the records it will show an add link to a select page
     *
     * miscellaneous processing flag
     */
    const AF_LARGE = 1048576;

    /**
     * Ignore filters when selecting records (relation).
     *
     * miscellaneous processing flag
     */
    const AF_NO_FILTER = 2097152;

    /**
     * Parent field for parent child relations (treeview).
     *
     * miscellaneous processing flag
     */
    const AF_PARENT = 4194304;

    /**
     * No quotes are used when adding to the database.
     *
     * DEPRECATED : values are passed as parameters, quotes are not needed anymore.
     *
     * miscellaneous processing flag
     */
    const AF_NO_QUOTES = 8388608;

    /**
     * Shortcut for hidden auto-incremented primary key.
     *
     * miscellaneous processing flag
     * self::AF_PRIMARY | self::AF_HIDE | self::AF_AUTOINCREMENT;
     */
    const AF_AUTOKEY = 1021;

    /**
     * Do not store this attribute.
     *
     * Storage type flag, used by the storageType() and related methods
     */
    const NOSTORE = 0;

    /**
     * Do not load this attribute.
     *
     * Storage type flag, used by the storageType() and related methods
     */
    const NOLOAD = 0;

    /**
     * Store before all other ('normal') attributes (?).
     *
     * Storage type flag, used by the storageType() and related methods
     */
    const PRESTORE = 1;

    /**
     * Call load before select().
     *
     * Storage type flag, used by the storageType() and related methods
     */
    const PRELOAD = 1;

    /**
     * Store after all other ('normal') attributes (?).
     *
     * Storage type flag, used by the storageType() and related methods
     */
    const POSTSTORE = 2;

    /**
     * Call load after select().
     *
     * Storage type flag, used by the storageType() and related methods
     */
    const POSTLOAD = 2;

    /**
     * Do addToQuery() of this attribute.
     *
     * Storage type flag, used by the storageType() and related methods
     */
    const ADDTOQUERY = 4;

    /**
     * Attribute is disable in view mode.
     */
    const DISABLED_VIEW = 1;

    /**
     * Attribute is disable in edit mode.
     */
    const DISABLED_EDIT = 2;

    /**
     * Attribute is disabled in view and edit mode
     * self::DISABLED_VIEW | self::DISABLED_EDIT;.
     */
    const DISABLED_ALL = 3;

    /*
     * The name of the attribute
     * @access private
     * @var String
     */
    public $m_name;

    /*
     * The attribute flags (see above)
     * @access private
     * @var int
     */
    public $m_flags = 0;

    /*
     * The name of the Node that owns this attribute (set by atknode)
     * @access private
     * @var String
     */
    public $m_owner = '';

    /*
     * The module of the attribute (if empty, the module from the owner node
     * should be assumed).
     * @access private
     * @var String
     *
     */
    public $m_module = '';


    /** @var Node $m_ownerInstance */
    public $m_ownerInstance;

    /*
     * The size the attribute's field.
     * @access private
     * @var int
     */
    public $m_size = 0;

    /*
     * The size the attribute's search input field.
     * @access private
     * @var int
     */
    public $m_searchsize = 0;

    /*
     * The maximum size the attribute's value may have in the database.
     * @access private
     * @var int
     */
    public $m_maxsize = 0;

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = null;

    /*
     * The order of the attribute within its node.
     * @access private
     * @var int
     */
    public $m_order = 0;

    /*
     * Index of the attribute within its node.
     * @access private
     * @var int
     */
    public $m_index = 0;

    /*
     * The tab(s) on which the attribute lives.
     * @access private
     * @var mixed
     */
    public $m_tabs = '*';

    /*
     * The section(s) on which the attribute lives.
     * @access private
     * @var mixed
     */
    public $m_sections = '*';

    /*
     * The id of the attribute in the HTML
     * @var String
     */
    public $m_htmlid;

    /*
     * The name of the attribute in the HTML
     * @var String
     */
    public $m_htmlname;

    /*
     * The css classes of the attribute
     * @var array
     */
    public $m_cssclasses = [];

    /*
     * The css classes of the container of the attribute
     * @var array
     */
    public $m_rowCssClasses = [];

    /*
     * The label of the attribute.
     * @access private
     * @var String
     */
    public $m_label = '';

    /*
     * The postfix label of the attribute.
     * @access private
     * @var String
     */
    public $m_postfixlabel = '';

    /*
     * The searchmode for this attribute
     *
     * This var exists so that you can assign searchmodes to specific
     * attributes instead of having a general searchmode for the entire
     * search. This can be any one of the supported modes, as returned by
     * the attribute's getSearchModes() method.
     * @access private
     * @var String
     */
    public $m_searchmode = '';

    /*
     * Wether to force an insert of the attribute
     * @access private
     * @var bool
     */
    public $m_forceinsert = false;

    /*
     * Wether to force a reload of the attribute ignoring the saved session data
     *
     * @access private
     * @var bool
     */
    public $m_forcereload = false;

    /*
     * Wether to force an update of the attribute
     * @access private
     * @var bool
     */
    public $m_forceupdate = false;

    /*
     * Array for containing onchange javascript code
     * @access private
     * @var array
     */
    public $m_onchangecode = [];

    /*
     * Variable to store initialisation javascript code
     * in for the changehandler.
     * @access private
     * @var String
     */
    public $m_onchangehandler_init = '';

    /**
     * Variable to store dependency callbacks.
     *
     * @var array
     */
    private $m_dependencies = [];

    /*
     * Attribute to store disabled modes.
     * @access private
     * @var int
     */
    public $m_disabledModes = 0;

    /*
     * Whether to hide initially or not
     * @access private
     * @var bool
     */
    public $m_initial_hidden = false;

    /*
     * Storage type.
     * @access private
     * @var int
     * @see setStorageType
     */
    public $m_storageType = [];

    /*
     * Load type.
     * @access private
     * @var int
     * @see setLoadType
     */
    public $m_loadType = [];

    /*
     * Initial value.
     * @access private
     * @var mixed
     * @see setInitialValue
     */
    public $m_initialValue = null;

    /**
     * Column.
     *
     * @var string
     */
    private $m_column;


    /**
     * View callback.
     *
     * @var mixed
     */
    private $m_viewCallback = null;

    /**
     * Edit callback.
     *
     * @var mixed
     */
    private $m_editCallback = null;

    protected $cssStyles = [];

    protected $m_select2Options = ['edit' => [], 'search' => []];

    /**
     * Help text
     * @var string
     */
    protected $m_help = '';

    /**
     * Placeholder text
     * @var string
     */
    protected $m_placeholder = '';

    /**
     * Constructor.
     *
     * <b>Example:</b>
     *        $this->add(new Attribute("name",self::AF_OBLIGATORY, 30));
     *
     * Note: If you want to use the db/ddl utility class to
     *       automatically generate the table, the $size parameter must be
     *       set, for it will use the size specified here to determine the
     *       field length. (Derived classes might have reasonable default
     *        values, but the standard Attribute doesn't.)
     *
     * @param string $name Name of the attribute (unique within a node, and
     *                      for most attributes, corresponds to a field in
     *                      the database.
     * @param int $flags Flags for the attribute.
     */
    public function __construct($name, $flags = 0)
    {
        $this->m_name = $name;
        $this->setFlags((int)$flags);

        // default class
        $this->addCSSClass($this->get_class_name());
    }

    public function get_class_name()
    {
        $classname = get_class($this);
        if ($pos = strrpos($classname, '\\')) {
            return substr($classname, $pos + 1);
        }

        return $pos;
    }

    /**
     * Returns the owner instance.
     *
     * @return Node owner instance
     */
    public function getOwnerInstance()
    {
        return $this->m_ownerInstance;
    }

    /**
     * Sets the owner instance.
     *
     * @param Node $instance
     */
    public function setOwnerInstance($instance)
    {
        $this->m_ownerInstance = $instance;
    }

    /**
     * Check if the attribute has a certain flag.
     *
     * @param int $flag The flag you want to check
     *
     * @return bool
     */
    public function hasFlag($flag)
    {
        return ($this->m_flags & $flag) == $flag;
    }

    /**
     * Returns the full set of flags of the attribute.
     *
     * @return int $m_flags The full set of flags
     */
    public function getFlags()
    {
        return $this->m_flags;
    }

    /**
     * Adds a flag to the attribute.
     * Note that adding flags at any time after the constructor might not
     * always work. There are flags that are processed only at
     * constructor time.
     *
     * @param int $flag The flag to add to the attribute
     *
     * @return Attribute The instance of this Attribute
     */
    public function addFlag($flag)
    {
        $this->m_flags |= $flag;

        if (!$this->hasFlag(self::AF_PRIMARY) && is_object($this->m_ownerInstance)) {
            if (Tools::hasFlag($flag, self::AF_HIDE_LIST) && !in_array($this->fieldName(), $this->m_ownerInstance->m_listExcludes)) {
                $this->m_ownerInstance->m_listExcludes[] = $this->fieldName();
            }

            if (Tools::hasFlag($flag, self::AF_HIDE_VIEW) && !in_array($this->fieldName(), $this->m_ownerInstance->m_viewExcludes)) {
                $this->m_ownerInstance->m_viewExcludes[] = $this->fieldName();
            }
        }

        return $this;
    }

    /**
     * Sets the flags of the attribute.
     *
     * Note that if you assign nothing or 0, this will remove all the flags
     * from the attribute. You can assign multiple flags by using the pipe
     * symbol. Setting the flags will overwrite all previous flag-settings.
     *
     * @param int $flags The flags to be set to the attribute.
     *
     * @return Attribute The instance of this Attribute
     */
    public function setFlags($flags = 0)
    {
        $this->m_flags = 0;
        $this->addFlag($flags); // always call addFlag
        return $this;
    }

    /**
     * Removes a flag from the attribute.
     *
     * Note that removing flags at any time after the constructor might not
     * always work. There are flags that have already been processed at
     * constructor time, so removing them will be futile.
     *
     * @param int $flag The flag to remove from the attribute
     *
     * @return Attribute The instance of this Attribute
     */
    public function removeFlag($flag)
    {
        if ($this->hasFlag($flag)) {
            $this->m_flags ^= $flag;
        }

        if (!$this->hasFlag(self::AF_PRIMARY) && is_object($this->m_ownerInstance)) {
            if (Tools::hasFlag($flag, self::AF_HIDE_LIST) && in_array($this->fieldName(), $this->m_ownerInstance->m_listExcludes)) {
                $key = array_search($this->fieldName(), $this->m_ownerInstance->m_listExcludes);
                unset($this->m_ownerInstance->m_listExcludes[$key]);
            }

            if (Tools::hasFlag($flag, self::AF_HIDE_VIEW) && in_array($this->fieldName(), $this->m_ownerInstance->m_viewExcludes)) {
                $key = array_search($this->fieldName(), $this->m_ownerInstance->m_viewExcludes);
                unset($this->m_ownerInstance->m_viewExcludes[$key]);
            }
        }

        return $this;
    }

    /**
     * Adds a disabled mode flag to the attribute  (use DISABLED_VIEW and DISABLED_EDIT flags).
     *
     * @param int $flag The flag to add to the attribute
     *
     * @return Attribute The instance of this Attribute
     */
    public function addDisabledMode($flag)
    {
        $this->m_disabledModes |= $flag;

        return $this;
    }

    /**
     * Check if the attribute is disabled in some mode (use DISABLED_VIEW and DISABLED_EDIT flags).
     *
     * @param int $flag The flag you want to check
     *
     * @return bool
     */
    public function hasDisabledMode($flag)
    {
        return ($this->m_disabledModes & $flag) == $flag;
    }

    /**
     * Sets the disabled mode flag of the attribute.
     *
     * Note that if you assign nothing or 0, this will remove all the flags
     * from the attribute. You can assign multiple flags by using the pipe
     * symbol. Setting the flags will overwrite all previous flag-settings.
     *
     * @param int $flags The flags to be set to the attribute.
     *
     * @return Attribute The instance of this Attribute
     */
    public function setDisabledModes($flags = 0)
    {
        $this->m_disabledModes = $flags;

        return $this;
    }

    /**
     * Removes a disabled mode from the attribute.
     *
     * @param int $flag The flag to remove from the attribute
     *
     * @return Attribute The instance of this Attribute
     */
    public function removeDisabledMode($flag)
    {
        if ($this->hasDisabledMode($flag)) {
            $this->m_disabledModes ^= $flag;
        }

        return $this;
    }

    /**
     * Returns the name of the attribute.
     *
     * For most attributes, this corresponds to the name of the field in the
     * database. For some attributes though (like one2many relations), the
     * name is a mere identifier within a node. This method always returns
     * the attribute name, despite the 'field' prefix of the method.
     *
     * @return string fieldname
     */
    public function fieldName()
    {
        return $this->m_name;
    }

    /**
     * Check if a record has an empty value for this attribute.
     *
     * @param array $record The record that holds this attribute's value.
     *
     * @return bool
     */
    public function isEmpty($record)
    {
        return !isset($record[$this->fieldName()]) || $record[$this->fieldName()] === '';
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * For the regular Attribute, it's simply returning the value.
     * Derived attributes may reimplement this for their own conversion.
     * This is the exact opposite of the db2value method.
     *
     * @param array $rec The record that holds this attribute's value.
     *
     * @return string The database compatible value
     */
    public function value2db($rec)
    {
        if (is_array($rec) && isset($rec[$this->fieldName()])) {
            return $rec[$this->fieldName()];
        }

        return;
    }

    /**
     * Converts a database value to an internal value.
     *
     * For the regular Attribute
     * Derived attributes may reimplement this for their own conversion.
     * (In which case, the return type might be 'mixed')
     *
     * This is the exact opposite of the value2db method.
     *
     * @param array $rec The database record that holds this attribute's value
     *
     * @return mixed The internal value
     */
    public function db2value($rec)
    {
        if (isset($rec[$this->fieldName()])) {
            return $rec[$this->fieldName()];
        }

        return;
    }

    /**
     * Is there a value posted for this attribute?
     *
     * @param array $postvars
     *
     * @return bool posted?
     */
    public function isPosted($postvars)
    {
        return is_array($postvars) && isset($postvars[$this->getHtmlName()]);
    }

    /**
     * Set initial value for this attribute.
     *
     * NOTE: the initial value only works if there is no initial_values override
     *       in the node or if the override properly calls parent::initial_values!
     *
     * @param mixed $value initial value
     *
     * @return Attribute The instance of this Attribute
     */
    public function setInitialValue($value)
    {
        $this->m_initialValue = $value;

        return $this;
    }

    /**
     * Initial value. Returns the initial value for this attribute
     * which will be used in the add form etc.
     *
     * @return mixed initial value for this attribute
     */
    public function initialValue()
    {
        return $this->m_initialValue;
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * For the regular Attribute, this means getting the field with the
     * same name as the attribute from the html posting.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     *
     * @return string|null The internal value
     */
    public function fetchValue($postvars)
    {
        if ($this->isPosted($postvars)) {
            return $postvars[$this->getHtmlName()];
        }

        return null;
    }


    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string A piece of htmlcode for editing this attribute
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $id = $this->getHtmlId($fieldprefix);

        if (Tools::count($this->m_onchangecode)) {
            $onchange = 'onChange="'.$id.'_onChange(this);"';
            $this->_renderChangeHandler($fieldprefix);
        } else {
            $onchange = '';
        }

        $value = (isset($record[$this->fieldName()]) && !is_array($record[$this->fieldName()]) ? htmlspecialchars($record[$this->fieldName()]) : '');

        $style = '';
        foreach($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }

        $result = '';
        $result .= '<input type="text" id="'.$id.'"';
        $result .= ' name="'.$this->getHtmlName($fieldprefix).'"';
        $result .= ' '.$this->getCSSClassAttribute(array('form-control'));
        $result .= ' value="'.$value.'"';
        if($this->m_size > 0){
            $result .= ' size="'.$this->m_size.'"';
        }
        if($this->m_maxsize > 0){
            $result .= ' maxlength="'.$this->m_maxsize.'"';
        }
        if($onchange){
            $result .= ' '.$onchange;
        }
        if($placeholder = $this->getPlaceholder()){
            $result .= ' placeholder="'.htmlspecialchars($placeholder).'"';
        }
        if($style != ''){
            $result .= ' style="'.$style.'"';
        }
        $result .= ' />';

        return $result;
    }

    /**
     * Add a javascript onchange event handler.
     *
     * @param string $jscode A block of valid javascript code.
     *
     * @return Attribute Returns the instance of this attribute.
     */
    public function addOnChangeHandler($jscode)
    {
        if (!in_array($jscode, $this->m_onchangecode)) {
            $this->m_onchangecode[] = $jscode;
        }

        return $this;
    }

    /**
     * Renders the onchange code on the page.
     *
     * @param string $fieldprefix The prefix to the field
     * @param string $elementNr The number of the element when attribute contains multiple options
     */
    public function _renderChangeHandler($fieldprefix, $elementNr = '')
    {
        if (Tools::count($this->m_onchangecode)) {
            $page = Page::getInstance();
            $page->register_scriptcode('
    function '.$this->getHtmlId($fieldprefix).$elementNr."_onChange(el)
    {
      {$this->m_onchangehandler_init}
      ".implode("\n      ", $this->m_onchangecode)."
    }\n");
        }
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">).
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        $method = $this->fieldName().'_hide';
        if(method_exists($this->m_ownerInstance, $method)){
            return $this->m_ownerInstance->$method($record, $fieldprefix, $mode);
        }

        // the next if-statement is a workaround for derived attributes which do
        // not override the hide() method properly. This will not give them a
        // working hide() functionality but at least it will not give error messages.
        $value = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null;
        if (!is_array($value)) {
            $result = '<input type="hidden" id="'.$this->getHtmlId($fieldprefix).'" name="'.$this->getHtmlName($fieldprefix).'" value="'.htmlspecialchars($value).'">';

            return $result;
        } else {
            Tools::atkdebug('Warning attribute '.$this->m_name.' has no proper hide method!');
        }

        return '';
    }

    /**
     * Return the html identifier (id="") of the attribute. (unique within a page).
     *
     * @param string $fieldprefix The fieldprefix to put in front of the id of any html form element for this attribute.
     *
     * @return string the HTML identifier.
     */
    public function getHtmlId($fieldprefix)
    {
        if (!isset($this->m_htmlid)) {
            $uri = '';
            if ($this->getOwnerInstance()) {
                $uri = str_replace('.', '_', $this->getOwnerInstance()->atkNodeUri()).'_';
            }
            $this->m_htmlid = $uri.$this->getHtmlName($fieldprefix);
        }

        return $this->m_htmlid;
    }

    /**
     * Return the name identifier (name="") of the attribute.
     *
     * Note: Without $fieldprefix argument, you get the index in decoded
     * postvars arrays.
     *
     * @param string $fieldprefix The fieldprefix to put in front of the name of
     *                            any html form element for this attribute. We assume
     *                            that it respects constraints.
     *
     * @return string the HTML identifier.
     */
    public function getHtmlName(string $fieldprefix = '') : string
    {
        if (!isset($this->m_htmlname)) {
            $this->m_htmlname = Tools::htmlName($this->m_name);
        }
        return $fieldprefix.$this->m_htmlname;
    }

    /**
     * Adds the attribute's view / hide HTML code to the view array.
     *
     * This method is called by the node if it wants the data needed to create
     * a view form.
     *
     * This is a framework method, it should never be called directly.
     *
     * @param string $mode the mode ("view")
     * @param array $arr pointer to the view array
     * @param array $defaults pointer to the default values array
     */
    public function addToViewArray($mode, &$arr, &$defaults)
    {
        if (!$this->hasFlag(self::AF_HIDE_VIEW)) {
            $entry = array('name' => $this->m_name, 'attribute' => $this);

            /* label? */
            $entry['label'] = $this->getLabel($defaults, $mode);
            // on which tab?
            $entry['tabs'] = $this->getTabs();
            //on which sections?
            $entry['sections'] = $this->getSections();
            /* the actual edit contents */
            $entry['html'] = $this->getView($mode, $defaults);
            $arr['fields'][] = $entry;
        }
    }

    /**
     * Prepare for edit. Is called before all attributes are added to the
     * edit array and allows for last minute manipulations based on the
     * record but also manipulations on the record itself.
     *
     * @param array $record reference to the record
     * @param string $fieldPrefix field prefix
     * @param string $mode edit mode
     */
    public function preAddToEditArray(&$record, $fieldPrefix, $mode)
    {
    }

    /**
     * Prepare for view. Is called before all attributes are added to the
     * view array and allows for last minute manipulations based on the
     * record but also manipulations on the record itself.
     *
     * @param array $record reference to the record
     * @param string $mode view mode
     */
    public function preAddToViewArray(&$record, $mode)
    {
    }

    /**
     * Adds the attribute's edit / hide HTML code to the edit array.
     *
     * This method is called by the node if it wants the data needed to create
     * an edit form.
     *
     * This is a framework method, it should never be called directly.
     *
     * @param string $mode the edit mode ("add" or "edit")
     * @param array $arr pointer to the edit array
     * @param array $defaults pointer to the default values array
     * @param array $error pointer to the error array
     * @param string $fieldprefix the fieldprefix
     */
    public function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
        /* hide */
        if (($mode == 'edit' && $this->hasFlag(self::AF_HIDE_EDIT)) || ($mode == 'add' && $this->hasFlag(self::AF_HIDE_ADD))) {
            /* when adding, there's nothing to hide, unless we're dealing with atkHiddenAttribute... */
            if ($mode == 'edit' || ($mode == 'add' && (!$this->isEmpty($defaults) || $this instanceof HiddenAttribute))) {
                $arr['hide'][] = $this->hide($defaults, $fieldprefix, $mode);
            }
        } /* edit */ else {
            global $ATK_VARS;


            $entry = array(
                'name' => $this->m_name,
                'obligatory' => $this->hasFlag(self::AF_OBLIGATORY),
                'attribute' => &$this,
            );

            $entry['class'] = $this->m_rowCssClasses;

            $entry['id'] = $this->getHtmlId($fieldprefix);

            /* label? */
            $entry['label'] = $this->getLabel($defaults, $mode);
            /* error? */
            $entry['error'] = $this->getError($error) || (isset($ATK_VARS['atkerrorfields']) && Tools::atk_in_array($entry['id'], $ATK_VARS['atkerrorfields']));
            // on which tab?
            $entry['tabs'] = $this->getTabs();
            //on which sections?
            $entry['sections'] = $this->getSections();
            // the actual edit contents
            $entry['html'] = $this->getEdit($mode, $defaults, $fieldprefix);
            // initially hidden
            $entry['initial_hidden'] = $this->isInitialHidden();
            $arr['fields'][] = $entry;
        }
    }

    /**
     * Put the attribute on one or more tabs.
     *
     * @param array $tabs An array of tabs on which the attribute should
     *                    be displayed.
     *
     * @return Attribute The instance of this Attribute
     */
    public function setTabs($tabs)
    {
        if (empty($tabs) && isset($this->m_ownerInstance) && is_object($this->m_ownerInstance)) {
            $tabs = array($this->m_ownerInstance->m_default_tab);
        } else {
            if (empty($tabs)) {
                $tabs = array('default');
            }
        }

        $this->m_tabs = $tabs;

        return $this;
    }

    /**
     * retrieve the tabs for this attribute.
     *
     * @return array
     */
    public function getTabs()
    {
        return $this->m_tabs;
    }

    /**
     * Put the attribute on one or more tabs and/or sections.
     *
     * Example:
     * <code>$attribute->setSections(array('tab.section','tab.othersection));</code>
     *
     * @param array $sections An array of tabs and/or sections on which the attribute should
     *                        be displayed.
     *
     * @return Attribute The instance of this Attribute
     */
    public function setSections($sections)
    {
        if ($sections == null) {
            $this->m_sections = [];
        } else {
            $this->m_sections = $sections;
        }

        return $this;
    }

    /**
     * retrieve the tabs and/or sections for this attribute.
     *
     * @return array
     */
    public function getSections()
    {
        return $this->m_sections;
    }

    /**
     * Get column.
     *
     * @return string column name
     */
    public function getColumn()
    {
        return $this->m_column;
    }

    /**
     * Set column.
     *
     * @param string $name column name
     *
     * @return Attribute The instance of this Attribute
     */
    public function setColumn($name)
    {
        $this->m_column = $name;

        return $this;
    }

    /**
     * Returns the view callback (if set).
     *
     * @return mixed callback method
     */
    protected function getViewCallback()
    {
        return $this->m_viewCallback;
    }

    /**
     * Sets the view callback.
     *
     * The callback is called instead of the regular display method of the
     * attribute.
     *
     * @param mixed $callback callback method
     */
    public function setViewCallback($callback)
    {
        $this->m_viewCallback = $callback;
    }

    /**
     * Retrieve the html code for placing this attribute in a view page.
     *
     * Method is 'smart' and can be overridden in the node using the
     * <attributename>_display() methods.
     *
     * Framework method, it should not be necessary to call this method
     * directly.
     *
     * @param string $mode The mode ("view")
     * @param array $defaults The record holding the values for this attribute
     *
     * @return string the HTML code for this attribute that can be used in a
     *                viewpage.
     */
    public function getView($mode, &$defaults)
    {
        $method = $this->m_name.'_display';

        if ($this->getViewCallback() != null) {
            $ret = call_user_func($this->getViewCallback(), $defaults, $mode, $this);
        } elseif (method_exists($this->m_ownerInstance, $method)) {
            $ret = $this->m_ownerInstance->$method($defaults, $mode);
        } else {
            $ret = $this->display($defaults, $mode);
            if ($ret != '' && strlen($this->m_postfixlabel) > 0) {
                $ret .= '&nbsp;'.$this->m_postfixlabel;
            }
        }

        if (in_array($mode, ['csv', 'plain', 'list'])) {
            return $ret;
        }

        $result = '<span class="form-control-static">'.$ret.'</span>';

        $helpText = $this->getHelp();
        if($helpText !== ''){
            $result .= '<p class="help-block">'.htmlspecialchars($helpText).'</p>';
        }

        return $result;
    }

    /**
     * Returns the edit callback (if set).
     *
     * @return mixed callback method
     */
    protected function getEditCallback()
    {
        return $this->m_editCallback;
    }

    /**
     * Sets the edit callback.
     *
     * The callback is called instead of the regular display method of the
     * attribute.
     *
     * @param mixed $callback callback method
     */
    public function setEditCallback($callback)
    {
        $this->m_editCallback = $callback;
    }

    /**
     * Retrieve the HTML code for placing this attribute in an edit page.
     *
     * The difference with the edit() method is that the edit() method just
     * generates the HTML code for editing the attribute, while the getEdit()
     * method is 'smart', and implements a hide/readonly policy based on
     * flags and/or custom override methodes in the node.
     * (<attributename>_edit() and <attributename>_display() methods)
     *
     * Framework method, it should not be necessary to call this method
     * directly.
     *
     * @param string $mode The edit mode ("add" or "edit")
     * @param array $defaults The record holding the values for this attribute
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     *
     * @return string the HTML code for this attribute that can be used in an
     *                editpage.
     */
    public function getEdit($mode, &$defaults, $fieldprefix)
    {
        // readonly
        if ($this->isReadonlyEdit($mode)) {
            return $this->hide($defaults, $fieldprefix, $mode).$this->getView($mode, $defaults);
        }

        $method = $this->m_name.'_edit';
        if ($this->getEditCallback() != null) {
            $result = call_user_func($this->getEditCallback(), $defaults, $fieldprefix, $mode, $this);
        } else {
            if ($this->m_name != 'action' && method_exists($this->m_ownerInstance, $method)) {
                // we can't support the override for attributes named action, because of a conflict with
                // a possible edit action override (in both cases the method is called action_edit)
                $result = $this->m_ownerInstance->$method($defaults, $fieldprefix, $mode);
            } else {
                $result = $this->edit($defaults, $fieldprefix, $mode).(strlen($this->m_postfixlabel) > 0 ? '&nbsp;'.$this->m_postfixlabel : '');
            }
        }

        $helpText = $this->getHelp();
        if($helpText !== ''){
            $result .= '<p class="help-block">'.htmlspecialchars($helpText).'</p>';
        }

        return $result;
    }

    public function isReadonlyEdit($mode)
    {
        return ($mode == 'edit' && $this->hasFlag(self::AF_READONLY_EDIT)) || ($mode == 'add' && $this->hasFlag(self::AF_READONLY_ADD));
    }

    /**
     * Check if this attribute has errors in the specified error list.
     *
     * @param array $errors The error list is one that is stored in the
     *                      "atkerror" section of a record, for example
     *                      generated by validate() methods.
     *
     * @return bool
     */
    public function getError($errors)
    {
        for ($i = 0; $i < Tools::count($errors); ++$i) {
            if ($errors[$i]['attrib_name'] == $this->fieldName() || Tools::atk_in_array($this->fieldName(), $errors[$i]['attrib_name'])) {
                return true;
            }
        }

        return false;
    }

    /**
     * Adds the attribute / field to the list header. This includes the column name and search field.
     *
     * Framework method. It should not be necessary to call this method directly.
     *
     * @param string $action the action that is being performed on the node
     * @param array $arr reference to the the recordlist array
     * @param string $fieldprefix the fieldprefix
     * @param int $flags the recordlist flags
     * @param array $atksearch the current ATK search list (if not empty)
     * @param ColumnConfig $columnConfig Column configuration object
     * @param DataGrid $grid The DataGrid this attribute lives on.
     * @param string $column child column (null for this attribute, * for this attribute and all childs)
     *
     * @throws Exception on invalid list column
     */
    public function addToListArrayHeader(
        $action,
        &$arr,
        $fieldprefix,
        $flags,
        $atksearch,
        $columnConfig,
        DataGrid $grid = null,
        $column = '*'
    ) {
        if ($column != null && $column != '*') {
            throw new Exception("Invalid list column {$column} for ".get_class($this).' '.$this->getOwnerInstance()->atkNodeUri().'::'.$this->fieldName());
        }

        if (!$this->hasFlag(self::AF_HIDE_LIST) && !($this->hasFlag(self::AF_HIDE_SELECT) && $action == 'select')) {
            $key = $this->getHtmlName($fieldprefix);

            $arr['heading'][$key]['title'] = $this->label();

            if ($grid->hasFlag(DataGrid::SORT) && !$this->hasFlag(self::AF_NO_SORT)) {
                $arr['heading'][$key]['order'] = $this->listHeaderSortOrder($columnConfig);
            }

            if ($grid->hasFlag(DataGrid::EXTENDED_SORT)) {
                $arr['sort'][$key] = $this->extendedSort($columnConfig, $fieldprefix, $grid);
            }

            if ($grid->hasFlag(DataGrid::SEARCH) && $this->hasFlag(self::AF_SEARCHABLE)) {
                $fn = $this->fieldName().'_search';
                if (method_exists($this->m_ownerInstance, $fn)) {
                    $arr['search'][$key] = $this->m_ownerInstance->$fn($atksearch, false, $fieldprefix, $grid);
                } else {
                    $arr['search'][$key] = $this->search($atksearch, false, $fieldprefix, $grid);
                }
                $arr['search'][$key] .= $this->searchMode(false, $fieldprefix);
            }
        }
    }

    /**
     * Adds the attribute / field to the list row. And if the row is totalisable also to the total.
     *
     * Framework method. It should not be necessary to call this method directly.
     *
     * @param string $action the action that is being performed on the node
     * @param array $arr reference to the the recordlist array
     * @param int $nr the current row number
     * @param string $fieldprefix the fieldprefix
     * @param int $flags the recordlist flags
     * @param bool $edit editing?
     * @param DataGrid $grid data grid
     * @param string $column child column (null for this attribute, * for this attribute and all childs)
     * @throws Exception
     */
    public function addToListArrayRow(
        $action,
        &$arr,
        $nr,
        $fieldprefix,
        $flags,
        $edit = false,
        DataGrid $grid = null,
        $column = '*'
    ) {
        if ($column != null && $column != '*') {
            throw new Exception("Invalid list column {$column} for ".get_class($this).' '.$this->getOwnerInstance()->atkNodeUri().'::'.$this->fieldName());
        }

        if (!$this->hasFlag(self::AF_HIDE_LIST) && !($this->hasFlag(self::AF_HIDE_SELECT) && $action == 'select')) {
            if ($edit) {
                $arr['rows'][$nr]['data'][$fieldprefix.$this->fieldName()] = $this->getEdit('list', $arr['rows'][$nr]['record'],
                    'atkdatagriddata_AE_'.$nr.'_AE_');
            } else {
                $arr['rows'][$nr]['data'][$fieldprefix.$this->fieldName()] = $this->getView('list', $arr['rows'][$nr]['record']);
            }

            /* totalisable? */
            if ($this->hasFlag(self::AF_TOTAL)) {
                $sum = $this->sum($arr['totalraw'], $arr['rows'][$nr]['record']);
                $arr['totalraw'][$this->fieldName()] = $sum[$this->fieldName()];
                $arr['total'][$fieldprefix.$this->fieldName()] = $this->getView('list', $sum);
            }
        }
    }

    /**
     * Returns a piece of html code that can be used to get search terms input
     * from the user.
     *
     * The framework calls this method to display the searchbox
     * in the search bar of the recordlist, and to display a more extensive
     * search in the 'extended' search screen.
     * The regular Attributes returns a simple text input box for entering
     * a keyword to search for.
     *
     * @todo  find a better way to search on onetomanys that does not require
     *        something evil in Attribute
     *
     * @param array $atksearch Array with values from POST request
     * @param bool $extended if set to false, a simple search input is
     *                            returned for use in the searchbar of the
     *                            recordlist. If set to true, a more extended
     *                            search may be returned for the 'extended'
     *                            search page. The Attribute does not
     *                            make a difference for $extended is true, but
     *                            derived attributes may reimplement this.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     * @param DataGrid $grid
     *
     * @return string A piece of html-code
     */
    public function search($atksearch, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getSearchFieldName($fieldprefix);

        $value = $atksearch[$this->getHtmlName()] ?? '';

        $style = '';
        $type = $extended ? 'extended_search':'search';
        foreach($this->getCssStyles($type) as $k => $v) {
            $style .= "$k:$v;";
        }

        $class = $this->getCSSClassAttribute(['form-control']);
        $result = '';

        $result .= '<input type="text"';
        $result .= ' id="'.$id.'"';
        $result .= ' '.$class;
        $result .= ' name="'.$name.'"';
        $result .= ' value="'.htmlentities($value).'"';
        $result .= $this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '';
        $result .= $style != '' ? ' style="'.$style.'"': '';
        $result .= ' />';

        return $result;
    }


    /**
     * Returns piece of html which is used for setting/selecting the search
     * mode for this attribute.
     *
     * It will show a pulldown if using extended search and multiple
     * searchmodes are supported otherwise the default searchmode is selected.
     *
     * @param bool $extended using extended search?
     * @param string $fieldprefix optional fieldprefix
     *
     * @return string html which is used for selecting searchmode
     */
    public function searchMode($extended = false, $fieldprefix = '')
    {
        $searchModes = $this->getSearchModes();
        $dbSearchModes = $this->getDb()->getSearchModes();
        $searchModes = array_values(array_intersect($searchModes, $dbSearchModes));

        $searchMode = $this->getSearchMode();
        // Set current searchmode to first searchmode if not searching in extended form or no searchmode is set
        if (!$extended || empty($searchMode) || !in_array($searchMode, $searchModes)) {
            $searchMode = isset($searchModes[0]) ? $searchModes[0] : null;
        }

        if ($extended && Tools::count($searchModes) > 1) {
            $field = '<select class="form-control select-standard" name="'.$this->getSearchModeFieldname($fieldprefix).'">';

            foreach ($searchModes as $value) {
                $selected = $searchMode == $value ? ' selected="selected"' : '';
                $field .= '<option value="'.$value.'"'.$selected.'>'.htmlentities($this->text('search_'.$value)).'</option>';
            }

            $field .= '</select>';
        } else {
            $field = '<input type="hidden" name="'.$this->getSearchModeFieldname($fieldprefix).'" value="'.$searchMode.'">'.($extended ? Tools::atktext('search_'.$searchMode) : '');
        }

        return $field;
    }

    /**
     * Retrieve the current set or default searchmode of this attribute.
     *
     *
     * @return string the default searchmode for this attribute.
     */
    public function getSearchMode()
    {
        $searchmode = $this->m_ownerInstance->getSearchMode();

        if (is_array($searchmode)) {
            return isset($searchmode[$this->fieldName()]) ? $searchmode[$this->fieldName()] : null;
        }

        return $searchmode;
    }

    /**
     * Creates a smart search condition for a given search value, and adds it
     * to the query that will be used for performing the actual search.
     *
     * @param int $id The unique smart search criterium identifier.
     * @param int $nr The element number in the path.
     * @param array $path The remaining attribute path.
     * @param Query $query The query to which the condition will be added.
     * @param string $ownerAlias The owner table alias to use.
     * @param mixed $value The value the user has entered in the searchbox.
     * @param string $mode The searchmode to use.
     */
    public function smartSearchCondition($id, $nr, $path, $query, $ownerAlias, $value, $mode)
    {
        // default implementation doesn't supported nested paths, this method
        // should be overriden by relations etc. if they want to support this
        if (Tools::count($path) > 0) {
            Tools::atk_var_dump($path, 'Invalid search path for '.$this->m_ownerInstance->atkNodeUri().'#'.$this->fieldName().', ignoring criterium!');
        } else {
            $this->searchCondition($query, $ownerAlias, $value, $mode);
        }
    }

    /**
     * Creates a search condition for a given search value, and adds it to the
     * query that will be used for performing the actual search.
     *
     * @param Query $query The query to which the condition will be added.
     * @param string $table The name of the table in which this attribute
     *                           is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
     *
     * @param string $fieldaliasprefix
     */
    public function searchCondition($query, $table, $value, $searchmode, $fieldaliasprefix = '')
    {
        $searchCondition = $this->getSearchCondition($query, $table, $value, $searchmode, $fieldaliasprefix);
        if ($searchCondition) {
            $query->addSearchCondition($searchCondition);
        }
    }

    /**
     * Creates a searchcondition for the field and returns it.
     *
     * Was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * Side effect : it may add some joins into $query needed to perform searches.
     *
     * @param Query $query The query object where the search condition should be placed on
     * @param string $table The name of the table in which this attribute
     *                           is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
     * @param string $fieldname prefix for joined tables.
     *
     * @return QueryPart the search condition or null if no condition was returned
     */
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        // If we are accidentally mistaken for a relation and passed an array
        // we only take our own attribute value from the array
        if (is_array($value)) {
            $value = $value[$this->fieldName()];
        }

        if ($this->m_searchmode) {
            $searchmode = $this->m_searchmode;
        }

        // @todo Is this really needed?
        if (strpos($value, '*') !== false && Tools::atk_strlen($value) > 1) {
            // auto wildcard detection
            $searchmode = 'wildcard';
        }

        $func = $searchmode.'Condition';
        if (method_exists($query, $func) && ($value || ($value == 0))) {
            return $query->$func(Db::quoteIdentifier($table, $this->fieldName()), $value);
        } elseif (!method_exists($query, $func)) {
            Tools::atkdebug("Database doesn't support searchmode '$searchmode' for ".$this->fieldName().', ignoring condition.');
        }

        return null;
    }

    /**
     * Sets the searchmode for an attribute
     * This will cause attributes that respect this
     * to use the attributes searchmode for that particulair attribute
     * instead of the general searchmode.
     *
     * @param string $searchmode The searchmode we want to set on the attribute
     *
     * @return Attribute The instance of this Attribute
     */
    public function setAttributeSearchmode($searchmode)
    {
        $this->m_searchmode = $searchmode;

        return $this;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * The regular Attribute uses PHP's nl2br() and htmlspecialchars()
     * methods to prepare a value for display, unless $mode is "cvs".
     *
     * @param array $record The record that holds the value for this attribute
     * @param string $mode The display mode ("view" for viewpages, or "list"
     *                       for displaying in recordlists, "edit" for
     *                       displaying in editscreens, "add" for displaying in
     *                       add screens. "csv" for csv files. Applications can
     *                       use additional modes.
     *
     * @return string HTML String
     */
    public function display($record, $mode)
    {
        // the next if-statement is a workaround for derived attributes which do
        // not override the display() method properly. This will not give them a
        // working display() functionality but at least it will not give error messages.
        $value = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null;

        if (!is_array($value)) {
            // default behaviour is that we display a value 'as is'.
            if (($mode == 'csv') || ($mode == 'plain')) {
                return $value;
            }

            return nl2br(htmlspecialchars($value));
        }

        return '';
    }

    /**
     * Checks if a value is valid.
     *
     * The regular Attribute has no specific validation. Derived attributes
     * may override this method to perform custom validation.
     * Note that obligatory and unique fields are checked by the
     * atkNodeValidator, and not by the validate() method itself.
     *
     * @param array $record The record that holds the value for this
     *                       attribute. If an error occurs, the error will
     *                       be stored in the 'atkerror' field of the record.
     * @param string $mode The mode for which should be validated ("add" or
     *                       "update")
     */
    public function validate(&$record, $mode)
    {
    }

    /**
     * Checks if this attribute is really not null in the database.
     * This method does not look at the self::AF_OBLIGATORY flag, it only
     * checks in the database if the attribute's column is really not null.
     *
     * @return bool attribute's database column not null?
     */
    public function isNotNullInDb()
    {
        $db = $this->getDb();
        $meta = $db->tableMeta($this->m_ownerInstance->m_table);

        return Tools::hasFlag($meta[$this->fieldName()]['flags'], Db::MF_NOT_NULL);
    }

    /**
     * Adds this attribute to database queries.
     *
     * Database queries (select, insert and update) are passed to this method
     * so the attribute can 'hook' itself into the query.
     *
     * Framework method. It should not be necessary to call this method
     * directly. Derived attributes that consist of more than a single simple
     * database field (like relations for example), may have to reimplement
     * this method.
     *
     * @param Query $query The SQL query object
     * @param string $tablename The name of the table of this attribute
     * @param string $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
     * @param array $record The record that contains the value of this attribute.
     * @param int $level Recursion level if relations point to eachother, an
     *                                 endless loop could occur if they keep loading
     *                                 eachothers data. The $level is used to detect this
     *                                 loop. If overriden in a derived class, any subcall to
     *                                 an addToQuery method should pass the $level+1.
     * @param string $mode Indicates what kind of query is being processing:
     *                                 This can be any action performed on a node (edit,
     *                                 add, etc) Mind you that "add" and "update" are the
     *                                 actions that store something in the database,
     *                                 whereas the rest are probably select queries.
     */
    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        if ($mode == 'add' || $mode == 'update') {
            if ($mode == 'add' && $this->hasFlag(self::AF_AUTO_INCREMENT)) {
                $query->addSequenceField($this->fieldName(), $record[$this->fieldName()], $this->getOwnerInstance()->m_seq);

                return;
            }

            if ($this->isEmpty($record) && !$this->hasFlag(self::AF_OBLIGATORY) && !$this->isNotNullInDb()) {
                $query->addField($this->fieldName(), null);
            } else {
                $query->addField($this->fieldName(), $this->value2db($record));
            }
        } else {
            $query->addField($this->fieldName(), '', $tablename, $fieldaliasprefix);
        }
    }

    /**
     * The delete method is called by the framework to inform the attribute
     * that a record is deleted (BEFORE the query execution).
     *
     * The regular Attribute has no implementation for this method, but
     * derived attributes may override this, to take care of cleanups, cascade
     * deletes etc.
     * Note, that the framework only calls this method if the attribute has
     * the self::AF_CASCADE_DELETE flag.
     *
     * @param array $record
     *
     * @return bool true if cleanup was successful, false otherwise.
     */
    public function delete($record)
    {
        // delete is only of interest for special attributes like relations, or file attributes.
        return true;
    }

    /**
     * The postDelete method is called by the framework to inform the attribute
     * that a record is deleted (AFTER the query execution).
     *
     * The regular Attribute has no implementation for this method, but
     * derived attributes may override this, to take care of cleanups, cascade
     * deletes etc.
     * Note, that the framework only calls this method if the attribute has
     * the self::AF_CASCADE_DELETE flag.
     *
     * @param array $record
     *
     * @return bool true if cleanup was successful, false otherwise.
     */
    public function postDelete($record)
    {
        // postDelete is only of interest for special attributes like relations, or file attributes.
        return true;
    }

    /**
     * Calculate the sum of 2 records.
     *
     * This is called by the framework for the auto-totalling feature. Two
     * records are passed, and a record is returned. The reason that the
     * params are entire records instead of plain values, is that derived
     * classes or custom attributes may need information from other attributes
     * too.
     *
     * @param array $rec1 The first record
     * @param array $rec2 The second record
     *
     * @return array A record containing the sum of $rec1 and $rec2
     */
    public function sum($rec1, $rec2)
    {
        $value1 = (isset($rec1[$this->fieldName()]) ? $rec1[$this->fieldName()] : 0);
        $value2 = (isset($rec2[$this->fieldName()]) ? $rec2[$this->fieldName()] : 0);

        return array($this->fieldName() => ($value1 + $value2));
    }

    /**
     * Fetch the metadata about this attrib from the table metadata, and
     * process it.
     *
     * Lengths for the edit and searchboxes, and maximum lengths are retrieved
     * from the table metadata by this method.
     * Db field type is alse retrieved from this method if not previously defined.
     *
     * @param array $metadata The table metadata from the table for this
     *                        attribute.
     */
    public function fetchMeta($metadata)
    {
        $attribname = $this->fieldName();

        // maxsize (the maximum size that can be entered)
        if (isset($metadata[$attribname])) {
            if ($this->m_maxsize > 0) {
                // if the size is explicitly set, but the database simply can't handle it, we use the smallest value
                $this->m_maxsize = min($this->m_maxsize, $metadata[$attribname]['len']);
            } else {
                // no size explicitly set, so use the one we retrieved from the database
                $this->m_maxsize = $metadata[$attribname]['len'];
            }
            // Set dbfieldtype from metadata if not set from specific attribute definition.
            if ($m_dbfieldtype == Db::FT_UNSUPPORTED) {
                $this->m_dbfieldtype = $metadata[$attribname]['gentype'];
            }
        }

        // size (the size of the input box in add/edit forms)
        if (!$this->m_size) {
            $this->m_size = min($this->m_maxsize, $this->maxInputSize());
        }

        // searchsize (the size of the search box)
        if (!$this->m_searchsize) {
            $this->m_searchsize = min($this->m_maxsize, $this->maxSearchInputSize());
        }
    }

    /**
     * This function is called right after the attribute is added to the node.
     *
     * The regular Attribute has no implementation for this method, but
     * derived attributes may override this method to perform custom
     * initialization.
     */
    public function init()
    {
    }

    /**
     * This function is called at the end of the node's init method.
     *
     * The regular Attribute has no implementation for this method, but
     * derived attributes may override this method to perform custom
     * initialization.
     */
    public function postInit()
    {
    }

    /**
     * This function is called to compare if two records are considered equal
     * by this attribute.
     *
     * The regular Attribute performs a simple string match; derived
     * classes may override this method to perform more complex comparisons.
     *
     * @param array $recA The first record holding a value for this attribute.
     * @param array $recB The second record holding a value for the attribute.
     *
     * @return bool True if the attribute considers the records equal,
     *              false if not.
     */
    public function equal($recA, $recB)
    {
        return $recA[$this->fieldName()] == $recB[$this->fieldName()];
    }

    /**
     * Used to force an attribute to update with every addDb() call.
     *
     * @param bool $force Wether or not to force the attribute to insert
     *
     * @return Attribute Returns the instance of this attribute
     */
    public function setForceInsert($force)
    {
        $this->m_forceinsert = $force;

        return $this;
    }

    /**
     * Getter for wether or not an attribute is forced to insert.
     *
     * @return bool Wether or not an attribute is forced to insert
     */
    public function getForceInsert()
    {
        return $this->m_forceinsert;
    }

    /**
     * Used to force an attribute to update from the db regardless of the
     * attribute being present in the postvars/session.
     *
     * @param bool $force Wether or not to force the attribute to reload
     *
     * @return Attribute Returns the instance of this attribute
     */
    public function setForceReload($force)
    {
        $this->m_forcereload = $force;

        return $this;
    }

    /**
     * Used to force an attribute to update with every updateDb() call.
     *
     * @param bool $force Wether or not to force the attribute to update
     *
     * @return Attribute Returns the instance of this attribute
     */
    public function setForceUpdate($force)
    {
        $this->m_forceupdate = $force;

        return $this;
    }

    /**
     * Getter for wether or not an attribute is forced to update.
     *
     * @return bool Wether or not an attribute is forced to update
     */
    public function getForceUpdate()
    {
        return $this->m_forceupdate;
    }

    /**
     * This function is called by the framework to determine if the attribute
     * needs to be saved to the database in an addDb call.
     *
     * The regular Attribute returns false if the value is empty, or if
     * self::AF_HIDE is set; true in other cases. Exception: when self::AF_AUTO_INCREMENT
     * is set, the method always returns true. Derived attributes may override
     * this behavior.
     *
     * @param array $record The record that is going to be saved
     *
     * @return bool True if this attribute should participate in the add
     *              query; false if not.
     */
    public function needsInsert($record)
    {
        return !$this->hasFlag(self::AF_HIDE_ADD) || $this->hasFlag(self::AF_AUTO_INCREMENT) || !$this->isEmpty($record) || $this->m_forceinsert;

        // If we are set to hide_add, we will only insert into the
        // db if a value has been set (for example by an initial_values
        // method). Also, autoincrement fields might be hidden, and their
        // value is still empty, but they do need to be inserted lateron.
    }

    /**
     * This function is called by the framework to determine if the attribute
     * needs to be saved to the database in an updateDb call.
     *
     * The regular Attribute returns false if self::AF_READONLY_EDIT or
     * self::AF_HIDE_EDIT are set, but derived attributes may override this
     * behavior.
     *
     * @param array $record The record that is going to be saved.
     * @return bool True if this attribute should participate in the update
     *              query; false if not.
     */
    public function needsUpdate($record)
    {
        return (!$this->hasFlag(self::AF_READONLY_EDIT) && !$this->hasFlag(self::AF_HIDE_EDIT)) || $this->m_forceupdate;
    }

    /**
     * This function is called by the framework to determine if the attribute
     * needs to be updated from the db regardless of the attribute being present
     * in the postvars/session.
     *
     * @return bool True if this attribute should be reloaded; false if not.
     */
    public function needsReload()
    {
        return $this->m_forcereload;
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * Note that not all modes may be supported by the database driver.
     * Compare this list to the one returned by the databasedriver, to
     * determine which searchmodes may be used.
     *
     * @return array List of supported searchmodes
     */
    public function getSearchModes()
    {
        // exact match and substring search should be supported by any database.
        // (the LIKE function is ANSI standard SQL, and both substring and wildcard
        // searches can be implemented using LIKE)
        return array('substring', 'exact', 'wildcard', 'regexp');
    }

    /**
     * Set the size(s) of the attribute.
     *
     * @param mixed $size The max. number of characters that can be entered.
     *                    If not specified, or set to 0, the max. size is automatically
     *                    retrieved from the table metadata.
     *
     *        By default, the size of the edit box is the same as the maximum
     *        number of chars that can be entered (as long as it fits on
     *        screen). You can however pass an array of 2 or 3 numbers instead
     *        of a single number. In this case, the array is interpreted as
     *        follows:
     *        - $size[0] - The maximum size that can be entered
     *        - $size[1] - The size of the input box in add/edit forms
     *        - $size[2] - The size of the search box
     *
     *        If $size[2] is not specified, $size[1] will be used instead.
     *        If $size[1] is not specified, or the passed value is not an
     *        array, all 3 sizes will default to the first value.
     *
     *        Note: The sizes that are actually used depend both on the
     *        specified size and the size of the field in the database.
     *        Usually, these are the same. In the case they differ, the
     *        smallest of the 2 will be used.
     *
     * @return Attribute The instance of this Attribute
     */
    public function setAttribSize($size)
    {
        if (is_array($size) && Tools::count($size) > 0) {
            if (!empty($size[2])) {
                $this->m_searchsize = $size[2];
            } else {
                $this->m_searchsize = (empty($size[1]) ? $size[0] : $size[1]);
            }
            $this->m_size = (empty($size[1]) ? $size[0] : $size[1]);
            $this->m_maxsize = $size[0];
        } else {
            if ($size > 0) {
                $this->m_maxsize = $this->m_size = $this->m_searchsize = $size;
            }
        }

        return $this;
    }

    /**
     * Return the database field type of the attribute.
     *
     * Note that the type returned is a 'generic' type.
     *
     * If the type was read from the table metadata, that value will
     * be used. Else, the attribute will analyze its flags to guess
     * what type it should be. If self::AF_AUTO_INCREMENT is set, the field
     * is probaly "number". If not, it's probably "string".
     *
     * Note: Derived attributes should set m_dbfieldtype property if they
     *       use other field types than string or number. If the
     *       derived attribute is one that can not be stored in the
     *       database, Db::FT_UNSUPPORTED should be returned.
     *
     * @return int The 'generic' type of the database field for this
     *                attribute.
     */
    public function dbFieldType()
    {
        if (is_null($this->m_dbfieldtype)) {
            $this->m_dbfieldtype = ($this->hasFlag(self::AF_AUTO_INCREMENT) ? Db::FT_NUMBER : Db::FT_STRING);
        }

        return $this->m_dbfieldtype;
    }

    /**
     * Return the size of the field in the database.
     *
     * If 0 is returned, the size is unknown. In this case, the
     * return value should not be used to create table columns.
     *
     * Ofcourse, the size does not make sense for every field type.
     * So only interpret the result if a size has meaning for
     * the field type of this attribute. (For example, if the
     * database field is of type 'date', the size has no meaning)
     *
     * Note that derived attributes might set a dot separated size,
     * for example to store decimal numbers. The number after the dot
     * should be interpreted as the number of decimals.
     *
     * @return int The database field size
     */
    public function dbFieldSize()
    {
        if ($this->m_maxsize != 0) {
            return $this->m_maxsize;
        } else {
            if ($this->dbFieldType() == Db::FT_NUMBER) {
                return '10'; // default for numbers.
            } else {
                return '100'; // default for strings.
            }
        }
    }

    /**
     * Return the label of the attribute.
     *
     * The regular Attribute does not make use of the $record parameter;
     * The label is based on the attribute name, but is automatically
     * translated. Derived attributes may override this behavior.
     *
     * @return string HTML compatible label for this attribute
     */
    public function label()
    {
        return $this->m_label != '' ? $this->m_label : $this->text($this->fieldName());
    }

    /**
     * Set the label of the attribute.
     *
     * @param string $label
     *
     * @return Attribute The instance of this Attribute
     */
    public function setLabel($label)
    {
        $this->m_label = $label;

        return $this;
    }

    /**
     * Set the label of the attribute.
     *
     * @param string $label
     *
     * @return Attribute The instance of this Attribute
     */
    public function setPostFixLabel($label)
    {
        $this->m_postfixlabel = $label;

        return $this;
    }

    /**
     * Return the help text of the attribute.
     *
     *
     * @return string HTML compatible help text for this attribute
     */
    public function getHelp()
    {
        if ($this->m_help != '') {
            return $this->text($this->m_help);
        }

        return '';
    }

    /**
     * Set the help of the attribute.
     *
     * @param string $help
     *
     * @return Attribute The instance of this Attribute
     */
    public function setHelp($help)
    {
        $this->m_help = $help;

        return $this;
    }

    /**
     * Return the placeholder text of the attribute.
     *
     * @return string HTML compatible label for this attribute
     */
    public function getPlaceholder()
    {
        if ($this->m_placeholder != '') {
            return $this->text($this->m_placeholder);
        }

        return '';
    }

    /**
     * Set the placeholder text of the attribute.
     *
     * @param string $placeholder
     *
     * @return Attribute The instance of this Attribute
     */
    public function setPlaceholder($placeholder)
    {
        $this->m_placeholder = $placeholder;

        return $this;
    }

    /**
     * Get the module that this attribute originated from.
     *
     * By default, this is the module of the owning node of this attribute.
     * However, if the attribute was added using a modifier from a different
     * module, then the module that added the attribute is returned.
     *
     * @return string The name of the module of this attribute
     */
    public function getModule()
    {
        if ($this->m_module != '') {
            return $this->m_module;
        } elseif (is_object($this->m_ownerInstance)) {
            return $this->m_ownerInstance->m_module;
        }

        return '';
    }

    /**
     * Get the HTML label of the attribute.
     *
     * The difference with the label() method is that the label method always
     * returns the HTML label, while the getLabel() method is 'smart', by
     * taking the self::AF_NOLABEL and self::AF_BLANKLABEL flags into account.
     *
     * @param array $record The record holding the value for this attribute.
     * @param string $mode The mode ("add", "edit" or "view")
     *
     * @return string The HTML compatible label for this attribute, or an
     *                empty string if the label should be blank, or NULL if no
     *                label at all should be displayed.
     */
    public function getLabel($record = [], $mode = '')
    {
        if ($this->hasFlag(self::AF_NOLABEL)) {
            return 'AF_NO_LABEL';
        } else {
            if ($this->hasFlag(self::AF_BLANKLABEL)) {
                return;
            } else {
                return $this->label();
            }
        }
    }

    /**
     * Sets the storage type.
     *
     * @param int $type Bitmask containg information about storage requirements.
     * @param string $mode The storage mode ("add", "update" or null for all)
     *
     * @see storageType
     *
     * @return Attribute The instance of this Attribute
     */
    public function setStorageType($type, $mode = null)
    {
        $this->m_storageType[$mode] = $type;

        return $this;
    }

    /**
     * Determine the storage type of this attribute.
     *
     * With this method, the attribute tells the framework whether it wants
     * to be stored in the main query (addToQuery) or whether the attribute
     * has its own store() implementation. The regular Attribute checks if
     * a store() method is present, and returns self::POSTSTORE in this case, or
     * self::ADDTOQUERY otherwise. Derived attributes may override this behavior.
     *
     * Framework method. It should not be necesary to call this method
     * directly.
     *
     * @param string $mode The type of storage ("add" or "update")
     *
     * @return int Bitmask containing information about storage requirements.
     *             Note that since it is a bitmask, multiple storage types
     *             could be returned at once.
     *             self::POSTSTORE  - store() method must be called, after the
     *             master record is saved.
     *             self::PRESTORE   - store() must be called, before the master
     *             record is saved.
     *             self::ADDTOQUERY - addtoquery() must be called, so the attribute
     *             can nest itself in the master query.
     *             self::NOSTORE    - nor store(), nor addtoquery() should be
     *             called (attribute can not be stored in the
     *             database)
     */
    public function storageType($mode = null)
    {
        // Mode specific storage type.
        if (isset($this->m_storageType[$mode]) && $this->m_storageType[$mode] !== null) {
            return $this->m_storageType[$mode];
        } // Global storage type (key null is special!)
        else {
            if (isset($this->m_storageType[null]) && $this->m_storageType[null] !== null) {
                return $this->m_storageType[null];
            } // Default backwardscompatible behaviour:
            else {
                if (method_exists($this, 'store')) {
                    return self::POSTSTORE;
                } else {
                    return self::ADDTOQUERY;
                }
            }
        }
    }

    /**
     * Sets the load type.
     *
     * @param int $type Bitmask containg information about load requirements.
     * @param string $mode The load mode ("view", "admin" etc. or null for all)
     *
     * @see loadType
     *
     * @return Attribute The instance of this Attribute
     */
    public function setLoadType($type, $mode = null)
    {
        $this->m_loadType[$mode] = $type;

        return $this;
    }

    /**
     * Determine the load type of this attribute.
     *
     * With this method, the attribute tells the framework whether it wants
     * to be loaded in the main query (addToQuery) or whether the attribute
     * has its own load() implementation. The regular Attribute checks if a
     * load() method is present, and returns POSTLOAD in this case, or
     * self::ADDTOQUERY otherwise. Derived attributes may override this behavior.
     *
     * Framework method. It should not be necesary to call this method
     * directly.
     *
     * @param string $mode The type of load (view,admin,edit etc)
     *
     * @return int Bitmask containing information about load requirements.
     *             Note that since it is a bitmask, multiple load types
     *             could be returned at once.
     *             self::POSTLOAD   - load() method must be called, after the
     *             master record is loaded.
     *             self::PRELOAD    - load() must be called, before the master
     *             record is loaded.
     *             self::ADDTOQUERY - addtoquery() must be called, so the attribute
     *             can nest itself in the master query.
     *             self::NOLOAD     - nor load(), nor addtoquery() should be
     *             called (attribute can not be loaded from the
     *             database)
     */
    public function loadType($mode)
    {
        if (isset($this->m_loadType[$mode]) && $this->m_loadType[$mode] !== null) {
            return $this->m_loadType[$mode];
        } else {
            if (isset($this->m_loadType[null]) && $this->m_loadType[null] !== null) {
                return $this->m_loadType[null];
            } // Default backwardscompatible behaviour:
            else {
                if (method_exists($this, 'load')) {
                    return self::POSTLOAD;
                } else {
                    return self::ADDTOQUERY;
                }
            }
        }
    }

    /**
     * Determine the maximum length an input field may be.
     *
     * @return int
     */
    public function maxInputSize()
    {
        return Config::getGlobal('max_input_size');
    }

    /**
     * Determine the maximum length an input search field may be.
     *
     * @return int
     */
    public function maxSearchInputSize()
    {
        return Config::getGlobal('max_searchinput_size');
    }

    /**
     * Get list of additional tabs.
     *
     * Attributes can add new tabs to tabbed screens. This method will be
     * called to retrieve the tabs. The regular Attribute has no
     * implementation for this method. Derived attributes may override this.
     *
     * @param string $action The action for which additional tabs should be loaded.
     *
     * @return array The list of tabs to add to the screen.
     */
    public function getAdditionalTabs($action = null)
    {
        return [];
    }

    /**
     * Check if the attribute wants to be shown on a certain tab.
     *
     * @param string $tab The name of the tab to check.
     *
     * @return bool
     */
    public function showOnTab($tab)
    {
        return $this->getTabs() == '*' || Tools::atk_in_array($tab, $this->getTabs());
    }

    /**
     * Check if delete of the record is allowed.
     *
     * This method is called by the framework to check if the attribute
     * allows the record to be deleted. The default implementation always
     * returns true, but derived attributes may implement their own
     * logic to prevent deletion of the record.
     *
     * @return bool True if delete is allowed, false if not.
     */
    public function deleteAllowed()
    {
        return true;
    }

    /**
     * Convert a String representation into an internal value.
     *
     * Used by CSV imports and the like, to convert string values to internal
     * values. This is somewhat similar to db2value, but this method should,
     * when derived in other attributes, act 'smart' and treat the
     * value as a user string.
     * The default implementation returns the string unmodified, but derived
     * classes may override this method to alter that behaviour.
     *
     * @param string $stringvalue The value to parse.
     *
     * @return mixed Internal value
     */
    public function parseStringValue($stringvalue)
    {
        return $stringvalue;
    }

    /**
     * Adds the needed searchbox(es) for this attribute to the fields array. This
     * method should only be called by the SearchHandler.
     *
     * @param array $fields The array containing fields to use in the
     *                            extended search
     * @param Node $node The node where the field is in
     * @param array $record A record containing default values to put
     *                            into the search fields.
     * @param string $fieldprefix search / mode field prefix
     * @param bool $extended enable extended search mode
     *
     * @return Attribute The instance of this Attribute
     */
    public function addToSearchformFields(&$fields, $node, &$record, $fieldprefix = '', $extended = true)
    {
        $field = [];
        $defaults = $record;

        // set "widget" value:
        $funcname = $this->m_name.'_search';

        if (method_exists($node, $funcname)) {
            $field['widget'] = $node->$funcname($defaults, $extended, $fieldprefix);
        } else {
            $field['widget'] = $this->search($defaults, $extended, $fieldprefix); // second param indicates extended search.
        }

        // pre-emptive set "full" value:
        $field['full'] = $field['widget']; // lateron, we might add more to full
        // set "searchmode" value:
        $field['searchmode'] = $this->searchMode($extended, $fieldprefix);

        // set "label" value:
        $field['label'] = $this->label();

        // add $field to fields array
        $fields[] = $field;

        return $this;
    }

    /**
     * Retrieve the fieldname of the attribute in an atksearch form.
     *
     * @param string $prefix The prefix
     *
     * @return string Name of the attribute in an atksearch
     */
    public function getSearchFieldName($prefix)
    {
        return 'atksearch_AE_'.$this->getHtmlName($prefix);
    }

    /**
     * Retrieve the searchmode name of the attribute in an atksearch form.
     *
     * @param string $prefix The prefix
     *
     * @return string Name of the attribute in an atksearch
     */
    public function getSearchModeFieldname($prefix)
    {
        return 'atksearchmode_AE_'.$this->getHtmlName($prefix);
    }

    /**
     * Retrieves the sort options and sort order.
     *
     * @param ColumnConfig $columnConfig The config that contains options for
     *                                   extended sorting and grouping to a
     *                                   recordlist.
     * @param string $fieldprefix The prefix of the attribute
     * @param DataGrid $grid The grid that this attribute lives on.
     *
     * @return string HTML
     */
    public function extendedSort($columnConfig, $fieldprefix = '', $grid = null)
    {
        $result = $this->sortOptions($columnConfig, $fieldprefix, $grid).' '.$this->sortOrder($columnConfig, $fieldprefix, $grid);

        return $result;
    }

    /**
     * Retrieves the sort options for this attribute which is used in recordlists
     * and search actions.
     *
     * @param ColumnConfig $columnConfig The config that contains options for
     *                                   extended sorting and grouping to a
     *                                   recordlist.
     * @param string $fieldprefix The prefix of the attribute
     * @param DataGrid $grid The grid that this attribute lives on.
     *
     * @return string HTML
     */
    public function sortOptions($columnConfig, $fieldprefix = '', $grid = null)
    {
        if (!$this->hasFlag(self::AF_TOTAL) && $columnConfig->totalizable()) {
            // if we are not the sumcolumn itself, but there are totalcolumns present, we can perform subtotalling
            $cmd = ($columnConfig->hasSubTotal($this->fieldName()) ? 'unsubtotal' : 'subtotal');
            if ($grid == null) {
                return Tools::href(Config::getGlobal('dispatcher').'?'.$columnConfig->getUrlCommand($this->fieldName(), $cmd),
                        Tools::atktext('column_'.$cmd)).' ';
            } else {
                $call = $grid->getUpdateCall($columnConfig->getUrlCommandParams($this->fieldName(), $cmd));

                return '<a href="javascript:void(0)" onclick="'.htmlentities($call).'">'.$this->text('column_'.$cmd).'</a>';
            }
        }

        return '';
    }

    /**
     * Sets the sortorder options for this attribute which is used in recordlists
     * and search actions.
     *
     * @param ColumnConfig $columnConfig The config that contains options for
     *                                   extended sorting and grouping to a
     *                                   recordlist.
     * @param string $fieldprefix The prefix of the attribute on HTML forms
     * @param DataGrid $grid The grid that this attribute lives on.
     *
     * @return string HTML
     */
    public function sortOrder($columnConfig, $fieldprefix = '', $grid = null)
    {
        $fieldname = $this->fieldName();
        $currentOrder = $columnConfig->getOrder($fieldname);

        $res = '';
        if ($currentOrder > 0) {
            $direction = ($columnConfig->getSortDirection($this->fieldName()) == 'desc' ? 'asc' : 'desc');
            if ($grid == null) {
                $res = Tools::href(Config::getGlobal('dispatcher').'?'.$columnConfig->getUrlCommand($fieldname, $direction),
                        Tools::atktext('column_'.$direction)).' ';
            } else {
                $call = $grid->getUpdateCall($columnConfig->getUrlCommandParams($fieldname, $direction));
                $res = '<a href="javascript:void(0)" onclick="'.htmlentities($call).'">'.$this->text('column_'.$direction).'</a>';
            }
        }

        $res .= '<select class="form-control select-standard" name="atkcolcmd[][setorder]['.$fieldprefix.$fieldname.']">';
        $res .= '<option value="">';
        for ($i = 1; $i < 6; ++$i) {
            $selected = ($currentOrder == $i ? 'selected' : '');
            $res .= '<option value="'.$i.'" '.$selected.'>'.$i;
        }
        $res .= '</select>';

        return $res;
    }

    /**
     * Retrieve the sortorder for the listheader based on the
     * ColumnConfig.
     *
     * @param ColumnConfig $columnConfig The config that contains options for
     *                                   extended sorting and grouping to a
     *                                   recordlist.
     *
     * @return string Returns sort order ASC or DESC
     */
    public function listHeaderSortOrder(ColumnConfig $columnConfig)
    {
        $order = $this->fieldName();

        if (isset($columnConfig->m_colcfg[$order])) {
            $direction = $columnConfig->getDirection($order);
            if ($direction == 'asc') {
                $order .= ' desc';
            }
        }

        return $order;
    }

    /**
     * Retrieves the ORDER BY statement for this attribute's node.
     * Derived attributes may override this functionality to implement other
     * ordering statements using the given parameters.
     *
     * @param array $extra A list of attribute names to add to the order by
     *                          statement
     * @param string $table The table name (if not given uses the owner node's table name)
     * @param string $direction Sorting direction (ASC or DESC)
     *
     * @return string The ORDER BY statement for this attribute
     */
    public function getOrderByStatement($extra = [], $table = '', $direction = 'ASC')
    {
        if (empty($table)) {
            $table = $this->m_ownerInstance->m_table;
        }
        $direction = ($direction ? " {$direction}" : '');

        // check for a schema name in $table
        if (strpos($table, '.') !== false) {
            $identifiers = explode('.', $table);

            $tableIdentifier = '';
            foreach ($identifiers as $identifier) {
                $tableIdentifier .= Db::quoteIdentifier($identifier).'.';
            }

            if ($this->dbFieldType() == Db::FT_STRING && $this->getDb()->getForceCaseInsensitive()) {
                return 'LOWER('.$tableIdentifier.Db::quoteIdentifier($this->fieldName()).')'.$direction;
            }

            return $tableIdentifier.Db::quoteIdentifier($this->fieldName()).$direction;

        } else {
            if ($this->dbFieldType() == Db::FT_STRING && $this->getDb()->getForceCaseInsensitive()) {
                return 'LOWER('.Db::quoteIdentifier($table, $this->fieldName()).')'.$direction;
            }

            return Db::quoteIdentifier($table, $this->fieldName()).$direction;
        }
    }

    /**
     * Translate using the owner instance's module and type.
     *
     * @param string $string The string to be translated
     * @param bool $fallback
     *
     * @return string The translated string.
     */
    public function text($string, $fallback = true)
    {
        if (is_object($this->getOwnerInstance())) {
            return $this->getOwnerInstance()->text($string, null, '', '', !$fallback);
        } else {
            return Tools::atktext($string, $this->getModule(), '', '', '', !$fallback);
        }
    }

    /**
     * Get database instance for this attribute. Will return the owner
     * instance database instance unless the owner instance is not set
     * in which case the default instance will be returned.
     *
     * @return Db database instance
     */
    public function getDb()
    {
        if (is_object($this->getOwnerInstance())) {
            return $this->getOwnerInstance()->getDb();
        }

        return Db::getInstance();
    }

    /**
     * Handle a partial request for this attribute, different attributes
     * support different partials.
     *
     * @param string $partial The name of the partial (i.e. refresh)
     * @param string $mode The current add/edit mode
     *
     * @return string HTML Returns the result of the call to the partial
     *                handling method
     */
    public function partial($partial, $mode)
    {
        $method = "partial_{$partial}";

        if (!method_exists($this, $method)) {
            return '<span style="color: red; font-weight: bold">Invalid partial!</span>';
        }

        return $this->$method($mode);
    }

    /**
     * Partial method to refresh  the add/edit field for this attribute.
     *
     * @param string $mode add/edit mode
     *
     * @return string HTML the output needed to refresh the attribute.
     */
    public function partial_refresh($mode)
    {
        $record = $this->m_ownerInstance->updateRecord();
        $fieldprefix = $this->m_ownerInstance->m_postvars['atkfp'];

        $arr = array('fields' => array());
        $defaults = &$record;
        $error = [];

        $this->addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);

        $script = '';
        foreach ($arr['fields'] as $field) {
            $element = str_replace('.', '_', $this->m_ownerInstance->atkNodeUri().'_'.$field['id']);
            $value = str_replace("'", "\\'", $field['html']);
            $script .= "jQuery('$element').html('$value');";
        }

        return '<script>'.$script.'</script>';
    }

    /**
     * Special case of an on-change handler which gets executed server-side and
     * can manipulate the DOM using PHP wrapper methods available in the
     * atkFormModifier class or by outputting JavaScript code directly.
     *
     * @param mixed callback closure or something else which is_callable
     *
     * @return Attribute attribute instance
     */
    public function addDependency($callback)
    {
        $this->m_dependencies[] = $callback;

        return $this;
    }

    /**
     * Retrieve the dependees for this attribute.
     *
     * @return array Returns the list of dependees (callbacks) for this attribute.
     */
    public function getDependencies()
    {
        return $this->m_dependencies;
    }

    /**
     * Initialize and calls the dependencies.
     *
     * @param array $record record
     * @param string $fieldPrefix the prefix for this attribute in an HTML form
     * @param string $mode add/edit mode
     * @param bool $noCall only initialize dependencies, without calling them
     */
    public function initDependencies(&$record, $fieldPrefix, $mode, $noCall = false)
    {
        if (Tools::count($this->getDependencies()) == 0) {
            return;
        }

        if (!$noCall) {
            $this->_callDependencies($record, $fieldPrefix, $mode, true);
        }

        $action = $this->getOwnerInstance()->m_action;
        if ($action == null) {
            $action = $mode == 'add' ? 'add' : 'edit';
        }

        $url = Tools::partial_url($this->getOwnerInstance()->atkNodeUri(), $action, 'attribute.'.$this->fieldName().'.dependencies',
            array('atkdata' => array('fieldPrefix' => $fieldPrefix, 'mode' => $mode)));
        $url = Json::encode($url);

        $this->getOwnerInstance()->getPage()->register_script(Config::getGlobal('assets_url').'javascript/attribute.js');
        $code = "ATK.Attribute.callDependencies({$url}, el);";
        $this->addOnChangeHandler($code);
    }

    /**
     * Calls the dependency callbacks for this attribute.
     *
     * @param array $record record
     * @param string $fieldPrefix the prefix for this attribute in an HTML form
     * @param string $mode add/edit mode
     * @param bool $initial initial call (e.g. non-javascript manipulation)
     */
    protected function _callDependencies(&$record, $fieldPrefix, $mode, $initial)
    {
        $modifier = new EditFormModifier($this->getOwnerInstance(), $record, $fieldPrefix, $mode, $initial);

        foreach ($this->getDependencies() as $callable) {
            call_user_func($callable, $modifier);
        }
    }

    /**
     * Call dependencies for this attribute and output JavaScript.
     */
    public function partial_dependencies()
    {
        // set attribute sizes
        $this->getOwnerInstance()->setAttribSizes();

        $record = $this->getOwnerInstance()->updateRecord();
        $fieldPrefix = $this->getOwnerInstance()->m_postvars['atkdata']['fieldPrefix'];
        $mode = $this->getOwnerInstance()->m_postvars['atkdata']['mode'];

        // initialize dependencies
        foreach ($this->getOwnerInstance()->getAttributes() as $attr) {
            $attr->initDependencies($record, $fieldPrefix, $mode, true); // without calling
        }

        $this->_callDependencies($record, $fieldPrefix, $mode, false);
    }

    /**
     * Retrieve the CSS classes that were registered for this attribute.
     *
     * @return array A list of css classes
     */
    public function getCSSClasses()
    {
        return $this->m_cssclasses;
    }

    /**
     * Add a CSS class for this attribute on an HTML form.
     *
     * @param string $classname The name of a class.
     *
     * @return Attribute The instance of this Attribute
     */
    public function addCSSClass($classname)
    {
        if (!in_array($classname, $this->m_cssclasses)) {
            $this->m_cssclasses[] = $classname;
        }

        return $this;
    }

    /**
     * Remove a CSS class for this attribute on an HTML form.
     *
     * @param string $classname The name of a class.
     *
     * @return Attribute The instance of this Attribute
     */
    public function removeCSSClass($classname)
    {
        $this->m_cssclasses = array_diff($this->m_cssclasses, array($classname));

        return $this;
    }

    /**
     * Add a CSS class for the container of this attribute on an HTML form.
     *
     * @param string $classname The name of a class.
     *
     * @return Attribute The instance of this Attribute
     */
    public function addRowCSSClass($classname)
    {
        if (!in_array($classname, $this->m_rowCssClasses)) {
            $this->m_rowCssClasses[] = $classname;
        }

        return $this;
    }

    /**
     * Retrieve the attribute for the HTML-tag for this Attribute.
     *
     * @param mixed $additionalclasses A string or an array with classnames.
     *
     * @return string HTML The attributes classname(s)
     */
    public function getCSSClassAttribute($additionalclasses = array())
    {
        $classes = array_merge($this->getCSSClasses(), is_array($additionalclasses) ? $additionalclasses : array($additionalclasses));

        return 'class="'.implode(' ', $classes).'"';
    }

    /**
     * Set whether initially hidden or not.  A field is "hidden" by adding the class AttrRowHidden.
     *
     * @param bool $bool Initially hidden?
     *
     * @return Attribute The instance of this Attribute
     */
    public function setInitialHidden($bool)
    {
        $this->m_initial_hidden = $bool;

        return $this;
    }

    /**
     * check whether initially hidden or not.
     *
     * @return bool initially hidden
     */
    public function isInitialHidden()
    {
        return $this->m_initial_hidden;
    }


    /**
     * String representation for this attribute (PHP5 only).
     *
     * @return string attribute name prefixed with node type
     */
    public function __toString()
    {
        return $this->m_ownerInstance->atkNodeUri().'::'.$this->fieldName();
    }

    /**
     * @param $options
     * @param null|string|array $types null for all types, or string with type or array of types ('edit', 'search')
     * @return $this
     */
    public function setSelect2Options($options, $types = null)
    {
        if ($types == null) {
            $types = array_keys($this->m_select2Options);
        }

        if (!is_array($types)) {
            $types = [$types];
        }

        foreach ($types as $type) {
            $this->m_select2Options[$type] = $options;
        }

        return $this;
    }

    /**
     * @param string|array $type (edit, search, extended_search)
     * @param string $style
     * @param mixed $value
     */
    public function setCssStyle($type, $style, $value) {
        if(is_array($type)){
            foreach($type as $t){
                $this->cssStyles[$t][$style] = $value;
            }
        }else{
            $this->cssStyles[$type][$style] = $value;
        }
    }

    public function getCssStyle($type, $style)
    {
        if(isset($this->cssStyles[$type][$style])){
            return $this->cssStyles[$type][$style];
        }

        return null;
    }

    public function getCssStyles($type = null)
    {
        if($type != null) {
            if (isset($this->cssStyles[$type])) {
                return $this->cssStyles[$type];
            } else {
                return [];
            }
        } else {
            return $this->cssStyles;
        }
    }
}
