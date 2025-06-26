<?php

namespace Sintattica\Atk\Utils;

use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;

/**
 * Allows to make some modifications to the add/edit. Depending on which
 * time these methods are called the modifications are made in PHP or outputted
 * as JavaScript.
 *
 * This class is used by the attribute dependency mechanism and should *not* be
 * used stand-alone.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class EditFormModifier
{
    /**
     * Node.
     *
     * @var Node
     */
    private $m_node;

    /**
     * Record.
     *
     * @var array
     */
    private $m_record;

    /**
     * Add/edit mode.
     *
     * @var string
     */
    private $m_mode;

    /**
     * Field prefix.
     *
     * @var string
     */
    private $m_fieldPrefix;

    /**
     * Initial setup/modification of the edit form, e.g. when the form is
     * rendered for the first time.
     *
     * @var bool
     */
    private $m_initial;

    /**
     * Constructor.
     *
     * @param Node $node node instance
     * @param array $record record
     * @param string $fieldPrefix field prefix
     * @param string $mode add/edit mode
     * @param string $initial initial form setup?
     */
    public function __construct(Node $node, &$record, $fieldPrefix, $mode, $initial)
    {
        $this->m_node = $node;
        $this->m_record = &$record;
        $this->m_fieldPrefix = $fieldPrefix;
        $this->m_mode = $mode;
        $this->m_initial = $initial;
    }

    /**
     * Returns the node instance.
     *
     * @return Node node instance
     */
    public function getNode()
    {
        return $this->m_node;
    }

    /**
     * Returns a reference to the record. This means the record can be modified
     * which can be used to modify the record before refreshAttribute calls.
     *
     * @return array record reference
     */
    public function &getRecord()
    {
        return $this->m_record;
    }

    /**
     * Returns the form's field prefix.
     *
     * @return string field prefix
     */
    public function getFieldPrefix()
    {
        return $this->m_fieldPrefix;
    }

    /**
     * Returns the mode (add or edit).
     *
     * @return string mode (add or edit)
     */
    public function getMode()
    {
        return $this->m_mode;
    }

    /**
     * Is this the initial setup of the form (or are we updating the form from
     * an Ajax request)?
     *
     * @return bool initial form setup?
     */
    public function isInitial()
    {
        return $this->m_initial;
    }

    /**
     * Show the attribute row for the attribute with the given name.
     *
     * @param string $name attribute name
     */
    public function showAttribute($name)
    {
        $attr = $this->getNode()->getAttribute($name);
        if ($this->isInitial()) {
            $attr->setInitialHidden(false);
        } else {
            $rowId = 'ar_'.$attr->getHtmlId($this->getFieldPrefix());
            $this->scriptCode("ATK.Tools.showAttribute('$rowId');");
        }
    }

    /**
     * Show the attributes rows for the attributes with the given names.
     *
     * @param array $names attributes names
     * @param bool $check Check the presence of the attributes
     */
    public function showAttributes($names, $check = false)
    {
        foreach ($names as $name) {
            if (!$check || $this->getNode()->getAttribute($name)) {
                $this->showAttribute($name);
            }
        }
    }

    /**
     * Hide the attribute row for the attribute with the given name.
     *
     * @param string $name attribute name
     */
    public function hideAttribute($name)
    {
        $attr = $this->getNode()->getAttribute($name);
        if ($this->isInitial()) {
            $attr->setInitialHidden(true);
        } else {
            $rowId = 'ar_'.$attr->getHtmlId($this->getFieldPrefix());
            $this->scriptCode("ATK.Tools.hideAttribute('$rowId');");
        }
    }

    /**
     * Hide the attributes rows for the attributes with the given names.
     *
     * @param array $names attributes names
     * @param bool $check Check the presence of the attributes
     */
    public function hideAttributes($names, $check = false)
    {
        foreach ($names as $name) {
            if (!$check || $this->getNode()->getAttribute($name)) {
                $this->hideAttribute($name);
            }
        }
    }

    /**
     * Hide the tab with the given name.
     *
     * @param string $name tab name
     */
    public function hideTab($name)
    {
        $this->scriptCode("ATK.Tools.hideTab('tab_$name');");
    }

    /**
     * Show the tab with the given name.
     *
     * @param string $name tab name
     */
    public function showTab($name)
    {
        $this->scriptCode("ATK.Tools.showTab('tab_$name');");
    }

    /**
     * Re-render / refresh the attribute with the given name.
     *
     * @param string $name attribute name
     */
    public function refreshAttribute($name)
    {
        if ($this->isInitial()) {
            return;
        }

        $offset = Tools::count($this->getNode()->getPage()->getLoadScripts());

        $error = [];
        $editArray = array('fields' => array());
        $this->m_node->getAttribute($name)->addToEditArray($this->getMode(), $editArray, $this->getRecord(), $error, $this->getFieldPrefix());

        $scriptCode = '';
        $re = '/<script.*>(.*)<\/script>/iU';

        foreach ($editArray['fields'] as $field) {
            $element = '#'.str_replace('.', '_', $this->getNode()->atkNodeUri().'_'.$field['id']);

            $value = preg_replace($re, '', $field['html']);
            $value = str_replace("'", "\\'", $value);
            $value = str_replace("\n", '', $value);
            $scriptCode .= "jQuery('$element').html('$value');";

            preg_match_all($re, $field['html'], $matches, PREG_SET_ORDER, 0);
            foreach ($matches as $match) {
                $scriptCode .= "\n".$match[1];
            }
        }

        $this->getNode()->getPage()->register_loadscript($scriptCode, $offset);
    }

    /**
     * Re-render / refresh the attributes with the given names.
     *
     * @param array $names attributes names
     * @param bool $check Check the presence of the attributes
     */
    public function refreshAttributes($names, $check = false)
    {
        foreach ($names as $name) {
            if (!$check || $this->getNode()->getAttribute($name)) {
                $this->refreshAttribute($name);
            }
        }
    }

    /**
     * Output JavaScript code.
     *
     * Script is executed in the on-load.
     *
     * @param string $code JavaScript code
     */
    public function scriptCode($code)
    {
        $this->getNode()->getPage()->register_loadscript($code);
    }

    /**
     * Register JavaScript file.
     *
     * @param string $file JavaScript file
     */
    public function scriptFile($file)
    {
        $this->getNode()->getPage()->register_script($file);
    }
}
