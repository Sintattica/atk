<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage relations
 *
 * @copyright (c) 2000-2007 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/** @internal includes and defines **/
userelation("atkmanytomanyrelation");
userelation('atkmanytoonerelation');

define('AF_MANYTOMANYSELECT_DETAILEDIT',      AF_SPECIFIC_1);
define('AF_MANYTOMANYSELECT_DETAILADD',       AF_SPECIFIC_2);
define('AF_MANYTOMANYSELECT_NO_AUTOCOMPLETE', AF_SPECIFIC_3);

/**
 * Many-to-many select relation.
 *
 * The relation shows allows you to add one record at a time to a
 * many-to-many relation using auto-completion or a select page.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage relations
 */
class atkManyToManySelectRelation extends atkManyToManyRelation
{
  const SEARCH_MODE_EXACT      = atkManyToOneRelation::SEARCH_MODE_EXACT;
  const SEARCH_MODE_STARTSWITH = atkManyToOneRelation::SEARCH_MODE_STARTSWITH;
  const SEARCH_MODE_CONTAINS   = atkManyToOneRelation::SEARCH_MODE_CONTAINS;

  /**
   * The many-to-one relation.
   *
   * @var atkManyToOneRelation
   */
  private $m_manyToOneRelation = null;
  
  /**
   * Constructs a new many-to-many select relation.
   *
   * @param string $name        The name of the relation
   * @param string $link         The full name of the node that is used as
   *                            intermediairy node. The intermediairy node is
   *                            assumed to have 2 attributes that are named
   *                            after the nodes at both ends of the relation.
   *                            For example, if node 'project' has a M2M relation
   *                            with 'activity', then the intermediairy node
   *                            'project_activity' is assumed to have an attribute
   *                            named 'project' and one that is named 'activity'.
   *                            You can set your own keys by calling setLocalKey()
   *                            and setRemoteKey()
   * @param string $destination The full name of the node that is the other
   *                            end of the relation.
   * @param int $flags          Flags for the relation.
   */
  public function __construct($name, $link, $destination, $flags=0)
  {
    parent::__construct($name, $link, $destination, $flags);

    $relation = new atkManyToOneRelation($this->fieldName().'_m2msr_add', $this->m_destination, AF_MANYTOONE_AUTOCOMPLETE|AF_HIDE);
    $relation->setDisabledModes(DISABLED_VIEW|DISABLED_EDIT);
    $relation->setLoadType(NOLOAD);
    $relation->setStorageType(NOSTORE);
    $relation->setNoneLabel($this->text('list_null_value_obligatory'));
    $this->m_manyToOneRelation = $relation;
  }

  /**
   * Initialize.
   */
  public function init()
  {
    $this->getOwnerInstance()->add($this->getManyToOneRelation());
  }

  /**
   * Return the many-to-one relation we will use for the selection
   * of new records etc.
   *
   * @return atkManyToOneRelation
   */
  protected function getManyToOneRelation()
  {
    return $this->m_manyToOneRelation;
  }

  /**
   * Return a piece of html code to edit the attribute.
   *
   * @param $record current record
   *
   * @return string piece of html code
   */
  public function edit($record, $fieldprefix="", $mode="")
  {
    if ($this->hasFlag(AF_MANYTOMANYSELECT_NO_AUTOCOMPLETE))
    {
      $this->getManyToOneRelation()->removeFlag(AF_MANYTOONE_AUTOCOMPLETE);
    }

    $this->createDestination();
    $this->createLink();

    $this->getOwnerInstance()->getPage()->register_script(atkconfig('atkroot').'atk/javascript/class.'.strtolower(__CLASS__).'.js');
    $this->getOwnerInstance()->addStyle('atkmanytomanyselectrelation.css');

    $selectedKeys = $this->getSelectedRecords($record);

    if (isset($record[$this->getManyToOneRelation()->fieldName()]) && is_array($record[$this->getManyToOneRelation()->fieldName()]))
    {
      $selectedKeys[] = $this->getDestination()->primaryKey($record[$this->getManyToOneRelation()->fieldName()]);
    }

    $selectedRecords = array();
    if (count($selectedKeys) > 0)
    {
      $selector = '('.implode(') OR (', $selectedKeys).')';
      $selectedRecords = $this->getDestination()->selectDb($selector);
    }

    $id = $this->getHtmlId($fieldprefix);

    $result =
      '<div class="atkmanytomanyselectrelation">
         <ul id="'.$id.'_selection" class="atkmanytomanyselectrelation-selection">';

    foreach ($selectedRecords as $selectedRecord)
    {
      $result .= $this->renderSelectedRecord($selectedRecord, $fieldprefix);
    }

    $result .= $this->renderAdditionField($fieldprefix, $mode);

    $result .= '
        </ul>
      </div>';

    if (($this->hasFlag(AF_MANYTOMANYSELECT_DETAILADD)) && ($this->m_destInstance->allowed("add")))
      $result.= href(dispatch_url($this->m_destination, "add"), $this->getAddLabel(), SESSION_NESTED) . "\n";

    return $result;
  }

  /**
   * Render selected record.
   *
   * @param array  $record      selected record
   * @param string $fieldprefix field prefix
   */
  protected function renderSelectedRecord($record, $fieldprefix)
  {
    $name = $this->getHtmlId($fieldprefix).'[]['.$this->getRemoteKey().']';
    $key = $record[$this->getDestination()->primaryKeyField()];

    // Define what actions need to be shown.
    $actions = array();
    if ($this->hasFlag(AF_MANYTOMANYSELECT_DETAILEDIT) && $this->getDestination()->allowed('edit', $record))
    {
      $actions[] = 'edit';
    }
    
    $actions[] = 'delete';
    $this->recordActions($record, $actions);

    // Call the renderButton action for those actions
    $actionLinks = array();
    foreach ($actions as $action)
    {
      $actionLink = $this->getActionLink($action, $record);
      if ($actionLink != null)
        $actionLinks[] = $actionLink; 
    }
    
    // Build the record
    $result = '
      <li class="atkmanytomanyselectrelation-selected">
        <input type="hidden" name="'.$name.'" value="'.htmlentities($key).'"/>
        <span>'.htmlentities($this->getDestination()->descriptor($record)).'</span>
        ('.implode(' / ', $actionLinks).')
      </li>
    ';

    return $result;
  }
  
  /**
   * This method returns the HTML for the link of a certain action
   *
   * @param string $action
   * @param array $record
   * @return string
   */
  protected function getActionLink($action, $record)
  {
      $actionMethod = "get{$action}ActionLink";
      
      if (method_exists($this,$actionMethod))
      {
        return $this->$actionMethod($record);
      }
      else
      {
        atkwarning('Missing '.$actionMethod.' method on manytomanyselectrelation. ');
      }
      
  }
  
  /**
   * The default edit link
   *
   * @param array $record
   * @return string
   */
  protected function getEditActionLink($record)
  {
    return href(dispatch_url($this->getDestination()->atkNodeType(), 'edit', array('atkselector' => $this->getDestination()->primaryKey($record))), $this->text('edit'), SESSION_NESTED, true);
  }
  
  /**
   * The default delete link
   *
   * @param array $record
   * @return string
   */
  protected function getDeleteActionLink($record)
  {
    return '<a href="javascript:void(0)" onclick="ATK.ManyToManySelectRelation.deleteItem(this); return false;">'.$this->text('delete').'</a>';
  }
  
  /**
   * Function that is called for each record in a recordlist, to determine
   * what actions may be performed on the record.
   *
   * @param array $record The record for which the actions need to be
   *                      determined.
   * @param &$actions Reference to an array with the already defined
   *                  actions.
   */
  function recordActions($record, &$actions)
  {
  }

  /**
   * Render addition field.
   *
   * @param string $fieldprefix field prefix
   */
  protected function renderAdditionField($fieldprefix, $mode)
  {
    $url = partial_url($this->getOwnerInstance()->atkNodeType(), $mode, 'attribute.'.$this->fieldName().'.selectedrecord', array('fieldprefix' => $fieldprefix));

    $relation = $this->getManyToOneRelation();
    $relation->addOnChangeHandler("ATK.ManyToManySelectRelation.add(el, '{$url}');");

    return '<li class="atkmanytomanyselectrelation-addition">'.$relation->edit(array(), $fieldprefix, $mode).'</li>';
  }

  /**
   * Partial selected record.
   */
  public function partial_selectedrecord()
  {
    $this->createDestination();
    $this->createLink();

    $fieldprefix = $this->getOwnerInstance()->m_postvars['fieldprefix'];
    $selector = $this->getOwnerInstance()->m_postvars['selector'];

    if (empty($selector)) return '';

    $record = $this->getDestination()->select($selector)->includes($this->getDestination()->descriptorFields())->firstRow();

    return $this->renderSelectedRecord($record, $fieldprefix);
  }

  /**
   * Set the searchfields for the autocompletion. By default the
   * descriptor fields are used.
   *
   * @param array $searchFields
   */
  public function setAutoCompleteSearchFields($searchFields)
  {
    $this->getManyToOneRelation()->setAutoCompleteSearchFields($searchFields);
  }

  /**
   * Set the searchmode for the autocompletion:
   * exact, startswith(default) or contains.
   *
   * @param array $searchmode
   */
  public function setAutoCompleteSearchMode($mode)
  {
    $this->getManyToOneRelation()->setAutoCompleteSearchMode($mode);
  }

  /**
   * Set the case-sensitivity for the autocompletion search (true or false).
   *
   * @param array $caseSensitive
   */
  public function setAutoCompleteCaseSensitive($caseSensitive)
  {
    $this->getManyToOneRelation()->setAutoCaseSensitive($caseSensitive);
  }

  /**
   * Sets the minimum number of characters before auto-completion kicks in.
   *
   * @param int $chars
   */
  public function setAutoCompleteMinChars($chars)
  {
    $this->getManyToOneRelation()->setAutoCompleteMinChars($chars);
  }

  /**
   * Adds a filter value to the destination filter.
   *
   * @param string $filter
   */
  public function addDestinationFilter($filter)
  {
    return $this->getManyToOneRelation()->addDestinationFilter($filter);
  }

  /**
   * Sets the destination filter.
   *
   * @param string $filter
   */
  public function setDestinationFilter($filter)
  {
    return $this->getManyToOneRelation()->setDestinationFilter($filter);
  }
}
