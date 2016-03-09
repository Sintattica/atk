<?php namespace Sintattica\Atk\Relations;

use Sintattica\Atk\Core\Tools;

/**
 *
 * @author Tjeerd Bijlsma <tjeerd@ibuildings.nl>
 * @package atk
 * @subpackage relations
 */
abstract class ShuttleFilter extends ShuttleControl
{

    /**
     * Returns the change handler name.
     *
     * @param string $prefix
     *
     * @return string
     */
    protected function getChangeHandlerName($prefix)
    {
        return $prefix . $this->getName() . "_onChange";
    }

    /**
     * Register change handler
     *
     * @param string $mode
     * @param string $prefix
     */
    protected function registerChangeHandler($mode, $prefix)
    {
        $mode = ($mode == "add") ? "add" : "edit";
        $url = addslashes(Tools::partial_url($this->m_shuttle->m_ownerInstance->atkNodeUri(), $mode,
            "attribute." . $this->m_shuttle->getHtmlId($prefix) . ".filter", array("atkfieldprefix" => $prefix)));

        $page = $this->m_shuttle->m_ownerInstance->getPage();
        $page->register_scriptcode("function " . $this->getChangeHandlerName($prefix) . "(el)
                                  {
                                    shuttle_refresh('" . $url . "', '" . $this->m_shuttle->getHtmlId($prefix) . '[cselected][][' . $this->m_shuttle->getRemoteKey() . ']' . "', '" . $prefix . $this->m_shuttle->fieldName() . "[section]', '" . $this->m_section . "')
                                  }\n");
    }

    /**
     * Renders the shuttle filter control.
     *
     * @param array $record
     * @param string $mode
     * @param string $prefix
     *
     * @return string control
     */
    public function render($record, $mode, $prefix)
    {
        $this->registerChangeHandler($mode, $prefix);
        return '';
    }

    /**
     * This method gets called to set a hard limit to the amount of records that can
     * get returned.
     *
     * @return int
     */
    public function getLimit()
    {
        return null;
    }

    /**
     * Applies a filter clause to the destination node for this filter's current value.
     *
     * The current value can be retrieved from the record using $this->getValue(...).
     *
     * @param array $record full record
     *
     * @return string filter
     */
    public function getFilter(&$record)
    {

    }

}
