<?php namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

/**
 * Abstract class for implementing an atkSearchHandler
 *
 * @package atk
 * @subpackage testcases
 */
abstract class AbstractSearchHandler extends ActionHandler
{
    /**
     * Holds the table name of the searchcriteria
     * table. Due some BC issues of the atkSmartSearchHandler
     * this value can be overwritten by the checkTable function
     *
     * @var string
     */
    protected $m_table = 'atk_searchcriteria';

    /**
     * Indicates if the table
     * atk_searchcriteria exists
     * use the function tableExists
     *
     * @var boolean
     */
    protected $m_table_exists = null;

    /**
     * Return the criteria based on the postvarse
     * used for storing
     *
     * @return array
     */
    abstract function fetchCriteria();

    /**
     * Return the type of the atkSmartSearchHandler
     *
     * @return string
     */
    function getSearchHandlerType()
    {
        return strtolower(get_class($this));
    }

    /**
     * check if database table exists
     *
     * @return boolean
     */
    protected function tableExist()
    {
        if ($this->m_table_exists !== null) {
            return $this->m_table_exists;
        }

        $db = $this->m_node->getDb();
        $this->m_table_exists = $db->tableExists($this->m_table);

        Tools::atkdebug('tableExists checking table: ' . $this->m_table . ' exists : ' . print_r($this->m_table_exists,
                true));

        return $this->m_table_exists;
    }

    /**
     * List criteria.
     *
     * @return array criteria list
     */
    function listCriteria()
    {
        if (!$this->tableExist()) {
            return array();
        }

        $db = $this->m_node->getDb();
        $query = "SELECT c.name FROM {$this->m_table} c WHERE c.nodetype = '%s' ORDER BY UPPER(c.name) AND handlertype = '%s'";
        $rows = $db->getRows(sprintf($query, $this->m_node->atkNodeUri(), $this->getSearchHandlerType()));

        $result = array();
        foreach ($rows as $row) {
            $result[] = $row['name'];
        }

        return $result;
    }

    /**
     * Remove search criteria.
     *
     * @param string $name name of the search criteria
     */
    function forgetCriteria($name)
    {
        if (!$this->tableExist()) {
            return false;
        }

        $db = $this->m_node->getDb();
        $query = "DELETE FROM {$this->m_table} WHERE nodetype = '%s' AND UPPER(name) = UPPER('%s') AND handlertype = '%s'";

        $db->query(sprintf($query, $this->m_node->atkNodeUri(), Tools::escapeSQL($name),
            $this->getSearchHandlerType()));
        $db->commit();
    }

    /**
     * Save search criteria.
     *
     * NOTE:
     * This method will overwrite existing criteria with the same name.
     *
     * @param string $name name for the search criteria
     * @param array $criteria search criteria data
     */
    function saveCriteria($name, $criteria)
    {
        if (!$this->tableExist()) {
            return false;
        }

        $this->forgetCriteria($name);
        $db = $this->m_node->getDb();
        $query = "INSERT INTO {$this->m_table} (nodetype, name, criteria, handlertype) VALUES('%s', '%s', '%s', '%s')";
        $db->query(sprintf($query, $this->m_node->atkNodeUri(), Tools::escapeSQL($name),
            Tools::escapeSQL(serialize($criteria)), $this->getSearchHandlerType()));
        $db->commit();
    }

    /**
     * Load search criteria.
     *
     * @param string $name name of the search criteria
     * @return array search criteria
     */
    function loadCriteria($name)
    {
        if (!$this->tableExist()) {
            return array();
        }

        $db = $this->m_node->getDb();
        $query = "SELECT c.criteria FROM {$this->m_table} c WHERE c.nodetype = '%s' AND UPPER(c.name) = UPPER('%s') AND handlertype = '%s'";

        Tools::atk_var_dump(sprintf($query, $this->m_node->atkNodeUri(), Tools::escapeSQL($name),
            $this->getSearchHandlerType()), 'loadCriteria query');

        list($row) = $db->getRows(sprintf($query, $this->m_node->atkNodeUri(), Tools::escapeSQL($name),
            $this->getSearchHandlerType()));
        $criteria = $row == null ? null : unserialize($row['criteria']);

        Tools::atk_var_dump($criteria, 'loadCriteria criteria');
        return $criteria;
    }

    /**
     * Load base criteria.
     *
     * @return array search criteria
     */
    function loadBaseCriteria()
    {
        return array(array('attrs' => array()));
    }

    /**
     * Returns a select list of loadable criteria which will on-selection
     * refresh the smart search page with the loaded criteria.
     *
     * @param string $current The current load criteria
     * @return String criteria load HTML
     */
    function getLoadCriteria($current)
    {
        $criteria = $this->listCriteria();
        if (count($criteria) == 0) {
            return null;
        }

        $result = '
      <select name="load_criteria" onchange="this.form.submit();" class="form-control">
        <option value=""></option>';

        foreach ($criteria as $name) {
            $result .= '<option value="' . htmlentities($name) . '"' . ($name == $current
                    ? ' selected' : '') . '>' . htmlentities($name) . '</option>';
        }

        $result .= '</select>';
        return $result;
    }

    /**
     * Take the necessary 'saved criteria' actions based on the
     * posted variables.
     * Returns the name of the saved criteria
     *
     * @param array $criteria array with the current criteria
     * @return string name of the saved criteria
     */
    function handleSavedCriteria($criteria)
    {
        $name = array_key_exists('load_criteria', $this->m_postvars) ? $this->m_postvars['load_criteria']
            : '';
        if (!empty($this->m_postvars['forget_criteria'])) {
            $forget = $this->m_postvars['forget_criteria'];
            $this->forgetCriteria($forget);
            $name = null;
        } else {
            if (!empty($this->m_postvars['save_criteria'])) {
                $save = $this->m_postvars['save_criteria'];
                $this->saveCriteria($save, $criteria);
                $name = $save;
            }
        }
        return $name;
    }

    /**
     * Returns an array with all the saved criteria
     * information. This information will be parsed
     * to the different
     *
     * @param string $current
     * @return array
     */
    function getSavedCriteria($current)
    {
        // check if table is present
        if (!$this->tableExist()) {
            return array();
        }

        return array(
            'load_criteria' => $this->getLoadCriteria($current),
            'forget_criteria' => $this->getForgetCriteria($current),
            'toggle_save_criteria' => $this->getToggleSaveCriteria(),
            'save_criteria' => $this->getSaveCriteria($current),
            'label_load_criteria' => htmlentities(Tools::atktext('load_criteria', 'atk')),
            'label_forget_criteria' => htmlentities(Tools::atktext('forget_criteria', 'atk')),
            'label_save_criteria' => '<label for="toggle_save_criteria">' . htmlentities(Tools::atktext('save_criteria',
                    'atk')) . '</label>',
            'text_save_criteria' => htmlentities(Tools::atktext('save_criteria', 'atk'))
        );
    }

    /**
     * Returns a link for removing the currently selected criteria. If
     * nothing (valid) is selected nothing is returned.
     *
     * @param string $current currently loaded criteria
     * @return String forget url
     */
    function getForgetCriteria($current)
    {
        if (empty($current) || $this->loadCriteria($current) == null) {
            return null;
        } else {
            $sm = SessionManager::getInstance();
            return $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), $this->m_action,
                array('forget_criteria' => $current)), SessionManager::SESSION_REPLACE);
        }
    }

    /**
     * Returns a checkbox for enabling/disabling the saving of criteria.
     *
     * @return checkbox HTML
     */
    function getToggleSaveCriteria()
    {
        return '<input id="toggle_save_criteria" type="checkbox" class="atkcheckbox" onclick="$(save_criteria).disabled = !$(save_criteria).disabled">';
    }

    /**
     * Returns a textfield for entering a name to save the search criteria as.
     *
     * @param string $current currently loaded criteria
     * @param string textfield HTML
     */
    function getSaveCriteria($current)
    {
        return '<input id="save_criteria" class="form-control" type="text" size="30" name="save_criteria" value="' . htmlentities($current) . '" disabled="disabled">';
    }

}
