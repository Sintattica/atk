<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

/**
 * Abstract class for implementing an atkSearchHandler.
 *
 * To allow for registering sets of criteria, you need to create
 * a table in your database with columns like :
 *
 * CREATE TABLE atk_searchcriteria (
 *  name varchar(100) NOT NULL,
 *  nodetype varchar(100) NOT NULL,
 *  handlertype varchar(100) NOT NULL,
 *  criteria varchar(4096),
 *  PRIMARY KEY (name)
 * );
 *
 */
abstract class AbstractSearchHandler extends ActionHandler
{
    /**
     * Holds the table name of the searchcriteria
     * table. Due some BC issues of the atkSmartSearchHandler
     * this value can be overwritten by the checkTable function.
     *
     * @var string
     */
    protected $m_table = 'atk_searchcriteria';

    /**
     * Indicates if the table
     * atk_searchcriteria exists
     * use the function tableExists.
     *
     * @var bool
     */
    protected $m_table_exists = null;

    /**
     * Return the criteria based on the postvarse
     * used for storing.
     *
     * @return array
     */
    abstract public function fetchCriteria();

    /**
     * Return the type of the atkSmartSearchHandler.
     *
     * @return string
     */
    public function getSearchHandlerType()
    {
        return strtolower(get_class($this));
    }

    /**
     * check if database table exists.
     *
     * @return bool
     */
    protected function tableExist()
    {
        if ($this->m_table_exists !== null) {
            return $this->m_table_exists;
        }

        $db = $this->m_node->getDb();
        $this->m_table_exists = $db->tableExists($this->m_table);

        Tools::atkdebug('tableExists checking table: '.$this->m_table.' exists : '.print_r($this->m_table_exists, true));

        return $this->m_table_exists;
    }

    /**
     * List criteria.
     *
     * @return array criteria list
     */
    public function listCriteria()
    {
        if (!$this->tableExist()) {
            return [];
        }

        $query = $this->m_node->getDb()->createQuery($this->m_table);
        $query->addField('name');
        $query->addCondition('nodetype = :nodetype', [':nodetype' => $this->m_node->atkNodeUri()]);
        $query->addCondition('handlertype = :handlertype', [':handlertype' => $this->getSearchHandlerType()]);
        $query->addOrderBy('name');
        $rows = $query->executeSelect();

        $result = [];
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
    public function forgetCriteria($name)
    {
        if (!$this->tableExist()) {
            return false;
        }

        $db = $this->m_node->getDb();
        $query = $db->createQuery($this->m_table);
        $query->addCondition('nodetype = :nodetype', [':nodetype' => $this->m_node->atkNodeUri()]);
        $query->addCondition('name = :name', [':name' => $name]);
        $query->addCondition('handlertype = :handlertype', [':handlertype' => $this->getSearchHandlerType()]);

        $query->executeDelete();
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
    public function saveCriteria($name, $criteria)
    {
        if (!$this->tableExist()) {
            return false;
        }

        $this->forgetCriteria($name);
        $db = $this->m_node->getDb();
        $query = $db->createQuery($this->m_table);
        $query->addFields([
            'nodetype' => $this->m_node->atkNodeUri(),
            'name' => $name,
            'criteria' => serialize($criteria),
            'handlertype' => $this->getSearchHandlerType()
        ]);
        $query->executeInsert();
        $db->commit();
    }

    /**
     * Load search criteria.
     *
     * @param string $name name of the search criteria
     *
     * @return array search criteria
     */
    public function loadCriteria($name)
    {
        if (!$this->tableExist()) {
            return [];
        }

        $query = $this->m_node->getDb()->createQuery($this->m_table);
        $query->addField('criteria');
        $query->addCondition('nodetype = :nodetype', [':nodetype' => $this->m_node->atkNodeUri()]);
        $query->addCondition('name = :name', [':name' => $name]);
        $query->addCondition('handlertype = :handlertype', [':handlertype' => $this->getSearchHandlerType()]);

        $rows = $query->executeSelect();
        if (empty($rows)) {
            return null;
        }
        return unserialize($rows[0]['criteria']);
    }

    /**
     * Load base criteria.
     *
     * @return array search criteria
     */
    public function loadBaseCriteria()
    {
        return array(array('attrs' => array()));
    }

    /**
     * Returns a select list of loadable criteria which will on-selection
     * refresh the smart search page with the loaded criteria.
     *
     * @param string $current The current load criteria
     *
     * @return string criteria load HTML
     */
    public function getLoadCriteria($current)
    {
        $criteria = $this->listCriteria();
        if (Tools::count($criteria) == 0) {
            return;
        }

        $result = '
      <select name="load_criteria" onchange="this.form.submit();" class="form-control select-standard">
        <option value=""></option>';

        foreach ($criteria as $name) {
            $result .= '<option value="'.htmlentities($name).'"'.($name == $current ? ' selected' : '').'>'.htmlentities($name).'</option>';
        }

        $result .= '</select>';

        return $result;
    }

    /**
     * Take the necessary 'saved criteria' actions based on the
     * posted variables.
     * Returns the name of the saved criteria.
     *
     * @param array $criteria array with the current criteria
     *
     * @return string name of the saved criteria
     */
    public function handleSavedCriteria($criteria)
    {
        $name = array_key_exists('load_criteria', $this->m_postvars) ? $this->m_postvars['load_criteria'] : '';
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
     * to the different.
     *
     * @param string $current
     *
     * @return array
     */
    public function getSavedCriteria($current)
    {
        // check if table is present
        if (!$this->tableExist()) {
            return [];
        }

        return array(
            'load_criteria' => $this->getLoadCriteria($current),
            'forget_criteria' => $this->getForgetCriteria($current),
            'toggle_save_criteria' => $this->getToggleSaveCriteria(),
            'save_criteria' => $this->getSaveCriteria($current),
            'label_load_criteria' => htmlentities(Tools::atktext('load_criteria', 'atk')),
            'label_forget_criteria' => htmlentities(Tools::atktext('forget_criteria', 'atk')),
            'label_save_criteria' => '<label for="toggle_save_criteria">'.htmlentities(Tools::atktext('save_criteria', 'atk')).'</label>',
            'text_save_criteria' => htmlentities(Tools::atktext('save_criteria', 'atk')),
        );
    }

    /**
     * Returns a link for removing the currently selected criteria. If
     * nothing (valid) is selected nothing is returned.
     *
     * @param string $current currently loaded criteria
     *
     * @return string forget url
     */
    public function getForgetCriteria($current)
    {
        if (empty($current) || $this->loadCriteria($current) == null) {
            return;
        } else {
            $sm = SessionManager::getInstance();

            return $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), $this->m_action, array('forget_criteria' => $current)),
                SessionManager::SESSION_REPLACE);
        }
    }

    /**
     * Returns a checkbox for enabling/disabling the saving of criteria.
     *
     * @return string HTML
     */
    public function getToggleSaveCriteria()
    {
        return '<input id="toggle_save_criteria" type="checkbox" class="atkcheckbox" onclick="$(save_criteria)[0].disabled = !$(save_criteria)[0].disabled">';
    }

    /**
     * Returns a textfield for entering a name to save the search criteria as.
     *
     * @param string $current currently loaded criteria
     *
     * @@return string HTML
     */
    public function getSaveCriteria($current)
    {
        return '<input id="save_criteria" class="form-control" type="text" size="30" name="save_criteria" value="'.htmlentities($current).'" disabled="disabled">';
    }
}
