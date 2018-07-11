<?php

namespace Sintattica\Atk\Relations;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\Page;

/**
 * Many-to-many relation.
 *
 * The relation shows a list of available records, and a set of checkboxes
 * to link the records with the current record on the source side.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ManyBoolRelation extends ManyToManyRelation
{
    /**
     * Attribute flag. When used the atkManyBoolRelation shows add links to add records for the related table.
     */
    const AF_MANYBOOL_AUTOLINK = 33554432;

    /**
     * Hides the select all, select none and inverse links.
     */
    const AF_MANYBOOL_NO_TOOLBAR = 67108864;

    /**
     * The flag indicating wether or not we should show the 'details' link.
     *
     * @var bool
     */
    private $m_showDetailsLink = true;

    /**
     * Return a piece of html code to edit the attribute.
     *
     * @param array $record Current record
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string piece of html code
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $this->createDestination();
        $this->createLink();
        $result = '';

        $page = Page::getInstance();
        $page->register_script(Config::getGlobal('assets_url').'javascript/profileattribute.js');

        $selectedPk = $this->getSelectedRecords($record);

        $recordset = $this->_getSelectableRecords($record, $mode);
        $total_records = Tools::count($recordset);
        if ($total_records > 0) {
            $page = Page::getInstance();
            $page->register_script(Config::getGlobal('assets_url').'javascript/profileattribute.js');

            if (!$this->hasFlag(self::AF_MANYBOOL_NO_TOOLBAR)) {
                $result .= '<div align="left">[<a href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkAll(\''.$this->fieldName().'\'); return false;">'.Tools::atktext('check_all',
                        'atk').'</a> | <a href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkNone(\''.$this->fieldName().'\'); return false;">'.Tools::atktext('check_none',
                        'atk').'</a> | <a href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkInvert(\''.$this->fieldName().'\'); return false;">'.Tools::atktext('invert_selection',
                        'atk').'</a>]</div>';
            }

            $result .= '<div>';
            for ($i = 0; $i < $total_records; ++$i) {
                $detailLink = '';
                $sel = '';
                $onchange = '';
                $inputId = $this->getHtmlId($fieldprefix).'_'.$i;

                if (in_array($this->m_destInstance->primaryKey($recordset[$i]), $selectedPk)) {
                    $sel = 'checked';
                    if ($this->getShowDetailsLink() && !$this->m_linkInstance->hasFlag(Node::NF_NO_EDIT) && $this->m_linkInstance->allowed('edit')) {
                        $localPkAttr = $this->getOwnerInstance()->getAttribute($this->getOwnerInstance()->primaryKeyField());
                        $localValue = $localPkAttr->value2db($record);
                        $remotePkAttr = $this->getDestination()->getAttribute($this->getDestination()->primaryKeyField());
                        $remoteValue = $remotePkAttr->value2db($recordset[$i]);
                        $selector = $this->m_linkInstance->m_table.'.'.$this->getLocalKey().'='.$localValue.''.' AND '.$this->m_linkInstance->m_table.'.'.$this->getRemoteKey()."='".$remoteValue."'";
                        $detailLink = Tools::href(Tools::dispatch_url($this->m_link, 'edit', array('atkselector' => $selector)),
                            '['.Tools::atktext('edit', 'atk').']', SessionManager::SESSION_NESTED, true);
                    }
                }

                if (Tools::count($this->m_onchangecode)) {
                    $onchange = ' onChange="'.$inputId.'_onChange(this);"';
                    $this->_renderChangeHandler($fieldprefix, '_'.$i);
                }

                $value = $recordset[$i][$this->m_destInstance->primaryKeyField()];
                $css = $this->getCSSClassAttribute('atkcheckbox');
                $label = $this->m_destInstance->descriptor($recordset[$i]);
                $result .= '<div>';
                $result .= '  <input type="checkbox" id="'.$inputId.'" name="'.$this->getHtmlName($fieldprefix).'[]['.$this->getRemoteKey().']" value="'.$value.'" '.$css.' '.$sel.$onchange.' />';
                $result .= '  <label for="'.$inputId.'">'.$label.'</label>';
                if ($detailLink != '') {
                    $result .= ' '.$detailLink;
                }
                $result .= '</div>';
            }
            $result .= '</div>';
        } else {
            $nodename = $this->m_destInstance->m_type;
            $modulename = $this->m_destInstance->m_module;
            $result .= Tools::atktext('select_none', $modulename, $nodename).' ';
        }

        if (($this->hasFlag(self::AF_MANYBOOL_AUTOLINK)) && ($this->m_destInstance->allowed('add'))) {
            $result .= Tools::href(Tools::dispatch_url($this->m_destination, 'add'), $this->getAddLabel(), SessionManager::SESSION_NESTED)."\n";
        }

        return $result;
    }

    /**
     * Returns true if the details link should be rendered.
     *
     * @return bool
     */
    public function getShowDetailsLink()
    {
        return $this->m_showDetailsLink;
    }

    /**
     * Set wether or not we should show the details link.
     *
     * @param bool $status
     *
     * @return ManyToManyRelation
     */
    public function setShowDetailsLink($status)
    {
        $this->m_showDetailsLink = ($status == true);

        return $this;
    }
}
