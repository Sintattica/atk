<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\DateAttribute;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

/**
 * Smart search handler class.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class SmartSearchHandler extends AbstractSearchHandler
{
    /**
     * Fetch posted criteria.
     *
     * @return array fetched criteria
     */
    public function fetchCriteria()
    {
        $criteriaFields = $this->m_postvars['criteria'];
        $criteriaValues = $this->m_postvars['atksearch']['criteria'];
        $criteriaModes = $this->m_postvars['atksearchmode']['criteria'];

        $criteria = [];
        if (is_array($criteriaFields)) {
            foreach ($criteriaFields as $id => $field) {
                if (isset($criteriaValues[$id])) {
                    $field = array_merge($field, array('value' => reset($criteriaValues[$id])));
                }

                if (isset($criteriaModes[$id])) {
                    $field = array_merge($field, array('mode' => reset($criteriaModes[$id])));
                }

                $criteria[$id] = $field;
            }
        }

        return $criteria;
    }

    /**
     * The action handler method.
     */
    public function action_smartsearch()
    {
        $page = $this->getPage();

        // set attribute sizes
        $this->m_node->setAttribSizes();

        // handle partials
        if (!empty($this->m_postvars['atkpartial'])) {
            $method = 'partial'.$this->m_postvars['atkpartial'];
            if (method_exists($this, $method)) {
                echo $this->$method();
                die;
            } else {
                echo 'Unknown partial';
                die;
            }
        }

        $criteria = $this->fetchCriteria();
        $name = $this->handleSavedCriteria($criteria);

        // redirect to search results and return
        $doSearch = isset($this->m_postvars['atkdosearch']);
        if ($doSearch) {
            $this->redirectToResults($criteria);

            return;
        }

        // load criteria
        if (!empty($name)) {
            $criteria = $this->loadCriteria($name);
        } else {
            $criteria = $this->loadBaseCriteria();
        }

        // render smart search page
        $smartSearchPage = $this->invoke('smartSearchPage', $name, $criteria);
        $actionPage = $this->m_node->renderActionPage('smartsearch', $smartSearchPage);
        $page->addContent($actionPage);
    }

    /**
     * Partial criterium.
     */
    public function partialCriterium()
    {
        $criterium = $this->getCriterium((int)$this->m_postvars['next_criterium_id']);

        return $this->renderCriterium($criterium);
    }

    /**
     * Partial criterium field.
     */
    public function partialCriteriumField()
    {
        $criterium_id = $this->m_postvars['criterium_id'];
        $field_nr = $this->m_postvars['field_nr'];

        $attrNames = $this->m_postvars['criteria'][$criterium_id]['attrs'];
        ksort($attrNames);

        $attrNames = array_slice($attrNames, 0, $field_nr + 1);
        $path = $this->getNodeAndAttrPath($attrNames);
        $path = array_slice($path, $field_nr + 1);

        if (Tools::count($path) > 0) {
            return $this->getCriteriumField($criterium_id, $path, $scriptCode).'<script language="javascript">'.implode("\n", $scriptCode).'</script>';
        } else {
            return '';
        }
    }

    /**
     * Partial criterium value / mode.
     *
     * @param string $type 'value' or 'mode' partial?
     */
    public function _partialCriteriumValueOrMode($type)
    {
        $criterium_id = $this->m_postvars['criterium_id'];
        $field_nr = $this->m_postvars['field_nr'];

        $attrNames = $this->m_postvars['criteria'][$criterium_id]['attrs'];
        ksort($attrNames);

        $attrNames = array_slice($attrNames, 0, $field_nr + 1);
        $path = $this->getNodeAndAttrPath($attrNames);

        if (Tools::count($path) == $field_nr + 1) {
            if ($type == 'mode') {
                return $this->getCriteriumMode($criterium_id, $path);
            } else {
                return $this->getCriteriumValue($criterium_id, $path);
            }
        } else {
            return $this->m_node->text('none');
        }
    }

    /**
     * Partial criterium value.
     */
    public function partialCriteriumValue()
    {
        return $this->_partialCriteriumValueOrMode('value');
    }

    /**
     * Partial criterium value.
     */
    public function partialCriteriumMode()
    {
        return $this->_partialCriteriumValueOrMode('mode');
    }

    /**
     * Redirect to search results based on the given criteria.
     *
     * @param array $criteria
     */
    public function redirectToResults($criteria)
    {
        for ($i = 0, $_i = Tools::count($criteria); $i < $_i; ++$i) {
            $attrs = &$criteria[$i]['attrs'];
            if ($attrs[Tools::count($attrs) - 1] == '.') {
                array_pop($attrs);
            }
        }

        $params = array('atksmartsearch' => $criteria);
        $url = Tools::dispatch_url($this->m_node->atkNodeUri(), 'admin', $params);
        $sm = SessionManager::getInstance();
        $this->m_node->redirect($sm->sessionUrl($url, $sm->atkLevel() == 0 ? SessionManager::SESSION_REPLACE : SessionManager::SESSION_BACK));
    }

    /**
     * Returns the base labels for use in the templates.
     * Contains labels for the following fields:
     * 'field', 'value', 'add', 'remove'.
     *
     * @return string label
     */
    public function getLabels()
    {
        $labels = array(
            'criterium_field',
            'criterium_value',
            'criterium_mode',
            'add_criterium',
            'remove_criterium',
            'load_criteria',
            'save_criteria',
            'forget_criteria',
            'reset_criteria',
        );
        $result = [];
        foreach ($labels as $label) {
            $result[$label] = htmlentities(Tools::atktext($label, 'atk'));
        }

        return $result;
    }

    /**
     * Returns the template path with the given name.
     * Name can be either 'form' or 'criterium'.
     *
     * @param string $name template name
     *
     * @return string full template path
     */
    public function getTemplate($name)
    {
        $ui = $this->getUi();
        if ($name == 'form') {
            return $this->m_node->getTemplate('smartsearch');
        } else {
            if ($name == 'criterium') {
                return $ui->templatePath('smartcriterium.tpl');
            }
        }
    }

    /**
     * Get searchable attributes for the given node.
     *
     * @param Node $node reference to the node
     * @param array $excludes attribute exclude list
     *
     * @return array list of reference to searchable attributes
     */
    public function getSearchableAttributes($node, $excludes)
    {
        $attrNames = array_keys($node->m_attribList);
        $attrNames = array_diff($attrNames, $excludes);
        sort($attrNames);

        $attrs = [];
        foreach ($attrNames as $attrName) {
            $attr = $node->getAttribute($attrName);
            if (!$attr->hasFlag(Attribute::AF_HIDE_SEARCH)) {
                $attrs[] = $attr;
            }
        }

        return $attrs;
    }

    /**
     * Returns a select element with searchable attributes for
     * a certain node.
     *
     * @param array $entry
     */
    public function getAttributeList($entry)
    {
        if (Tools::count($entry['attrs']) == 1 && !$entry['includeSelf']) {
            $attr = $entry['attrs'][0];
            $label = is_a($attr, 'ManyToOneRelation') ? '' : htmlentities(strip_tags($attr->label()));

            return $label.'<input type="hidden" name="'.$entry['name'].'" value="'.$attr->fieldName().'">';
        }

        $result = '<select id="'.$entry['name'].'" name="'.$entry['name'].'" class="form-control select-standard">'.'<option value=""></option>';

        if ($entry['includeSelf']) {
            $result .= '<option value="."'.($entry['selectSelf'] ? ' selected="selected"' : '').'>'.$this->m_node->text('self').'</option>'.'<option value=""></option>';
        }

        $current = $entry['attr'];
        for ($i = 0, $_i = Tools::count($entry['attrs']); $i < $_i; ++$i) {
            $attr = $entry['attrs'][$i];
            $selected = $current != null && $attr->fieldName() == $current->fieldName() ? ' selected="selected"' : '';
            $label = htmlentities(strip_tags($attr->label()));
            $result .= '<option value="'.$attr->fieldName().'"'.$selected.'>'.$label.'</option>';
        }

        $result .= '</select>';

        return $result;
    }

    /**
     * Adds a node/attribute entry to the node/attribute path.
     *
     * The entry consists of the the following fields:
     * - nr,          number in the node/attribute path (>= 0)
     * - node,        reference to the node for this path entry
     * - attr,        reference to the currently selected attribute for this path entry
     * - attrs,       all searchable attributes for this node
     * - includeSelf, whatever the attribute list should contain a reference to ourselves or not
     * - selectSelf,  should the self option be selected? (only valid if includeSelf is true)
     *
     * This method will modify the $path, $includeSelf and $excludes parameters to prepare
     * them for the next call to this method.
     *
     * @param array $path reference to the current path
     * @param Node $node reference to the current node
     * @param string $attrName currently selected attribute
     * @param bool $includeSelf should we include ourselves?
     * @param array $excludes attributes to exclude
     *
     * @return Node next node
     */
    public function addNodeAndAttrEntry(&$path, $node, $attrName, &$includeSelf, &$excludes)
    {
        $attr = $node->getAttribute($attrName);

        $nr = Tools::count($path);
        $attrs = $this->getSearchableAttributes($node, $excludes);

        if (Tools::count($attrs) == 1 && !$includeSelf) {
            $attr = $attrs[0];
        }

        $selectSelf = $includeSelf && $attrName == '.';

        $entry = array(
            'nr' => $nr,
            'node' => $node,
            'attrs' => $attrs,
            'attr' => $attr,
            'includeSelf' => $includeSelf,
            'selectSelf' => $selectSelf,
        );
        $path[] = &$entry;

        $includeSelf = is_a($attr, 'ManyToOneRelation');
        $excludes = is_a($attr, 'OneToManyRelation') ? $attr->m_refKey : [];

        if (is_a($attr, 'Relation')) {
            $attr->createDestination();

            return $attr->m_destInstance;
        }

        return;
    }

    /**
     * Returns the node/attribute path for the given
     * attribute name path.
     *
     * @param array $attrPath attribute name path
     *
     * @return array node/attribute path
     */
    public function getNodeAndAttrPath($attrPath)
    {
        $path = [];

        $node = $this->m_node;
        $includeSelf = false;
        $excludes = [];

        $entry = null;

        foreach ($attrPath as $attrName) {
            $node = $this->addNodeAndAttrEntry($path, $node, $attrName, $includeSelf, $excludes);
            if ($node == null) {
                break;
            }
        }

        while ($node != null) {
            $node = $this->addNodeAndAttrEntry($path, $node, null, $includeSelf, $excludes);
        }

        return $path;
    }

    /**
     * Returns the criterium field for the given path.
     *
     * @param int $id criterium id
     * @param array $path criterium path
     * @param array $scriptCode lines of JavaScript
     *
     * @return string criterium field HTML
     */
    public function getCriteriumField($id, $path, &$scriptCode)
    {
        $prefix = "criteria[{$id}][attrs]";
        $sm = SessionManager::getInstance();

        for ($i = 0, $_i = Tools::count($path); $i < $_i; ++$i) {
            $entry = &$path[$i];
            $entry['name'] = "{$prefix}[{$entry[nr]}]";
            $entry['field'] = $this->getAttributeList($entry);
        }

        $result = '';
        for ($i = Tools::count($path) - 1, $_i = 0; $i >= $_i; --$i) {
            $entry = &$path[$i];
            $fieldName = "criterium_{$id}_{$entry[nr]}_other";
            $readOnly = Tools::count($entry['attrs']) == 1 && !$entry['includeSelf'];
            $hasLabel = !$readOnly || !is_a($entry['attr'], 'ManyToOneRelation');

            if (!$readOnly) {
                $valueName = "criterium_{$id}_value";
                $modeName = "criterium_{$id}_mode";

                $fieldUrl = $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), 'smartsearch',
                    array('criterium_id' => $id, 'field_nr' => $entry['nr'], 'atkpartial' => 'criteriumfield')), SessionManager::SESSION_NEW);
                $valueUrl = $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), 'smartsearch',
                    array('criterium_id' => $id, 'field_nr' => $entry['nr'], 'atkpartial' => 'criteriumvalue')), SessionManager::SESSION_NEW);
                $modeUrl = $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), 'smartsearch',
                    array('criterium_id' => $id, 'field_nr' => $entry['nr'], 'atkpartial' => 'criteriummode')), SessionManager::SESSION_NEW);

                $scriptCode[] = "ATK.SmartSearchHandler.registerCriteriumFieldListener('{$entry[name]}', '{$prefix}', '{$fieldName}', '{$fieldUrl}', '{$valueName}', '{$valueUrl}', '{$modeName}', '{$modeUrl}')";
            }

            $result = $entry['field'].($hasLabel ? '&nbsp;' : '').'<span id="'.$fieldName.'">'.$result.'</span>';
        }

        return $result;
    }

    /**
     * Returns the criterium value or mode field for the given path.
     *
     * @param int $id criterium id
     * @param array $path criterium path
     * @param array $value current search value
     * @param array $mode current search mode
     * @param string $type return either 'value' or 'mode' field
     * @param string string criterium value or mode field HTML
     */
    public function _getCriteriumValueOrMode($id, $path, $value, $mode, $type)
    {
        $entry = array_pop($path);
        if ($entry['selectSelf']) {
            $entry = array_pop($path);
        }

        /** @var Attribute $attr */
        $attr = $entry['attr'];

        if ($attr == null) {
            $node = $entry['node'];

            return $node->text('none');
        } else {
            /*
             * Yury's comment:
              See $this->fetchCriteria() method:
              $criteriaValues = $this->m_postvars['atksearch']['criteria'];
              $criteriaModes  = $this->m_postvars['atksearchmode']['criteria'];
              ....
              $field = array_merge($field, array('value' => reset($criteriaValues[$id])));

              See Attribute->getSearchFieldName()
              return 'atksearch_AE_'.$prefix.$this->fieldName();

              In fetchCriteria we expect the following:
              $criteriaValues - array with values for search for attribute(array index - attribute index
              $criteriaModes -  array with search mode for each attribute
              example: $atksearch['criteria'][0]['blabla'];

              For obtain this result, field index must be:
              atksearch_AE_criteria_AE_0_AE_
              and field prefix must be:
              criteria_AE_0_AE_
             */

            $prefix = "criteria_AE_{$id}_AE_";

            $valueArray = $value == null ? null : array($attr->fieldName() => $value);
            $attr->addToSearchformFields($fields, $entry['node'], $valueArray, $prefix, true);
            $field = array_shift($fields); // we only support the first field returned

            return $type == 'mode' ? $field['searchmode'] : $field['widget'];
        }
    }

    /**
     * Returns the criterium value field for the given path.
     *
     * @param int $id criterium id
     * @param array $path criterium path
     * @param array $value current search value
     * @param array $mode current search mode
     * @return string criterium value field HTML
     */
    public function getCriteriumValue($id, $path, $value = [], $mode = array())
    {
        return $this->_getCriteriumValueOrMode($id, $path, $value, $mode, 'value');
    }

    /**
     * Returns the criterium mode field for the given path.
     *
     * @param int $id criterium id
     * @param array $path criterium path
     * @param array $value current search value
     * @param array $mode current search mode
     * @return string criterium mode field HTML
     */
    public function getCriteriumMode($id, $path, $value = [], $mode = array())
    {
        return $this->_getCriteriumValueOrMode($id, $path, $value, $mode, 'mode');
    }

    /**
     * Returns the criterium parameters needed to render a criterium. The data
     * structure contains the already known information about the currently
     * selected field and values (if any).
     *
     * @param string $id criterium identifier
     * @param array $data criterium data
     */
    public function getCriterium($id, $data = array())
    {
        $prefix = "criterium_{$id}";
        $path = $this->getNodeAndAttrPath($data['attrs']);
        $scriptCode[] = "ATK.SmartSearchHandler.registerCriterium($id);";

        $result = [];

        $result['id'] = $id;
        $result['element'] = array(
            'box' => "{$prefix}_box",
            'field' => "{$prefix}_field",
            'value' => "{$prefix}_value",
            'mode' => "{$prefix}_mode",
        );
        $result['field'] = $this->getCriteriumField($id, $path, $scriptCode);
        $result['value'] = $this->getCriteriumValue($id, $path, $data['value'], $data['mode']);
        $result['mode'] = $this->getCriteriumMode($id, $path, $data['value'], $data['mode']);
        $result['template'] = $this->getTemplate('criterium');
        $result['script'] = '<script language="javascript">'.implode("\n", $scriptCode).'</script>';
        $result['remove_action'] = "ATK.SmartSearchHandler.removeCriterium($id)";

        return $result;
    }

    /**
     * Renders a single criterium (field and value).
     *
     * @param array $criterium criterium structure (from getCriterium)
     *
     * @return string rendered criterium
     */
    public function renderCriterium($criterium)
    {
        $ui = $this->getUi();
        $params = [];
        $params['label'] = $this->getLabels();
        $params['criterium'] = $criterium;

        return $ui->render('smartcriterium.tpl', $params);
    }

    /**
     * Returns a link for resetting the currently selected criteria.
     *
     * @return string reset url
     */
    public function getResetCriteria()
    {
        $sm = SessionManager::getInstance();

        return $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), $this->m_action), SessionManager::SESSION_REPLACE);
    }

    /**
     * This method returns a form that the user can use to search records.
     *
     * @param string $name
     * @param array $criteria
     *
     * @return string The searchform in html form.
     */
    public function smartSearchForm($name = '', $criteria = array())
    {
        $ui = $this->getUi();
        $sm = SessionManager::getInstance();

        $params = [];

        $params['label'] = $this->getLabels();
        $params['reset_criteria'] = $this->getResetCriteria();
        // $params['load_criteria']        = $this->getLoadCriteria($name);
        // $params['forget_criteria']      = $this->getForgetCriteria($name);
        // $params['toggle_save_criteria'] = $this->getToggleSaveCriteria();
        // $params['save_criteria']        = $this->getSaveCriteria($name);
        $params['saved_criteria'] = $this->getSavedCriteria($name);

        $params['criteria'] = [];
        Tools::atkdebug('criteria smartSearchForm: '.print_r($criteria, true));
        foreach ($criteria as $i => $criterium) {
            $params['criteria'][] = $this->getCriterium($i, $criterium);
        }

        $url = $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), 'smartsearch', array('atkpartial' => 'criterium')),
            SessionManager::SESSION_NEW);
        $params['action_add'] = "ATK.SmartSearchHandler.addCriterium('".addslashes($url)."')";

        return $ui->render($this->getTemplate('form'), $params);
    }

    /**
     * This method returns an html page that can be used as a search form.
     *
     * @param string $name
     * @param array $criteria
     *
     * @return string The html search page.
     */
    public function smartSearchPage($name = '', $criteria = array())
    {
        $node = $this->m_node;
        $page = $this->getPage();
        $ui = $this->getUi();
        $sm = SessionManager::getInstance();
        $page->register_script(Config::getGlobal('assets_url').'javascript/smartsearchhandler.js');
        DateAttribute::registerScriptsAndStyles();

        $params = [];
        $params['formstart'] = '<form id="entryform" name="entryform" action="'.Config::getGlobal('dispatcher').'" method="post" class="form">'.$sm->formState(SessionManager::SESSION_REPLACE).'<input type="hidden" name="atkaction" value="smartsearch">'.'<input type="hidden" name="atknodeuri" value="'.$node->atkNodeUri().'">';
        $params['content'] = $this->invoke('smartSearchForm', $name, $criteria);
        $params['buttons'][] = '<input type="submit" class="btn btn-sm btn-default btn_search" name="atkdosearch" value="'.Tools::atktext('search', 'atk').'">';
        $params['formend'] = '</form>';

        $action = $ui->renderAction('smartsearch', $params);
        $box = $ui->renderBox(array('title' => $node->actionTitle('smartsearch'), 'content' => $action));

        return $box;
    }
}
