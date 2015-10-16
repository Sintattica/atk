<?php namespace Sintattica\Atk\Wizard;


use Sintattica\Atk\Core\Controller;

/**
 * atkWizard class which is capable of using atknodes
 *
 * This class makes the distinction between update/save and
 * navigation actions and respondis correspondingly.
 *
 * @author maurice <maurice@ibuildings.nl>
 * @package atk
 *
 */
class WizardBase extends Controller
{

    /**
     * Constructor of atkWizardBase
     *
     * @return Controller object
     */
    function __construct()
    {
        parent::__construct();
    }

    /**
     * Returns the form buttons for a certain page.
     *
     * Can be overridden by derived classes to define custom buttons.
     * @param String $action The action for which the buttons are retrieved.
     * @param array $record The record currently displayed/edited in the form.
     *                      This param can be used to define record specific
     *                      buttons.
     * @return Array with html code elements with buttons
     *
     * ToDo/Fixme this function has been refactored in Controller.
     * It should be refactored in the same way.
     */
    function getFormButtons($action, $record)
    {
        $result = array();

        if ($this->m_mode == WIZARD_MODE_ADD) {
            $currentPanel = $this->getCurrentPanel();
            // We post the action as key value in de atkwizardaction var. Therefor
            // we have to convert the atkwizardaction value in Wizard::start().
            $node = $this->getNode();
            if ($node->m_action != 'admin') {
                //if we explicitly don't want the finish button we set a hidden var to post the atkwizardaction          
                if (($currentPanel->showFinishButton() == FINISH_BUTTON_DONT_SHOW) && $currentPanel->isFinishPanel()) {
                    $atkwizardaction = "finish";
                } else {
                    $atkwizardaction = "next";
                }

                if ($this->showFinishButton()) {
                    $result[] = '<input type="submit" class="btn_next" name="atkwizardaction[finish]" value="' . Tools::atktext("finish",
                            "atk") . '">';
                } else {
                    $result[] = '<input type="submit" class="btn_next" name="atkwizardaction[' . $atkwizardaction . ']" value="' . Tools::atktext("next",
                            "atk") . '">';
                }
            } else {
                //if we explicitly don't want the finish button we set a hidden var to post the atkwizardaction
                if ($currentPanel->showFinishButton() == FINISH_BUTTON_DONT_SHOW && $currentPanel->isFinishPanel()) {
                    $atkwizardaction = "finish";
                } else {
                    $atkwizardaction = "saveandnext";
                }

                $result[] = '<input type="submit" class="btn_next" name="atkwizardaction[saveandaddnew]" value="' . Tools::atktext("saveandaddnew",
                        "atk") . '">';
                if ($this->showFinishButton()) {
                    $result[] = '<input type="submit" class="btn_next" name="atkwizardaction[finish]" value="' . Tools::atktext("finish",
                            "atk") . '">';
                } else {
                    $result[] = '<input type="submit" class="btn_next" name="atkwizardaction[' . $atkwizardaction . ']" value="' . Tools::atktext("saveandnext",
                            "atk") . '">';
                }
            }

            $result[] = '<input type="submit" class="btn_cancel" name="atkwizardcancel" value="' . Tools::atktext("cancel",
                    "atk") . '">';
        } elseif ($this->m_mode == WIZARD_MODE_EDIT) {
            // We post the action as key value in de atkwizardaction var. Therefor
            // we have to convert the atkwizardaction value in Wizard::start().
            $result[] = '<input type="submit" class="btn_save" name="atknoclose" value="' . Tools::atktext("save",
                    "atk") . '">';
            $result[] = '<input type="submit" class="btn_next" name="atkwizardaction[finish]" value="' . Tools::atktext("finish",
                    "atk") . '">';
            $result[] = '<input type="submit" class="btn_cancel" name="atkcancel" value="' . Tools::atktext("cancel",
                    "atk") . '">';
        } else {
            $result = parent::getFormButtons($action, $record);
        }

        return $result;
    }

    /**
     * Determine if this panel should show a finish button
     *
     * @return Boolean
     */
    function showFinishButton()
    {
        $currentPanel = $this->getCurrentPanel();
        return (($currentPanel->showFinishButton() == FINISH_BUTTON_SHOW) ||
            ($currentPanel->showFinishButton() == FINISH_BUTTON_DEFAULT && $currentPanel->isFinishPanel()));
    }

}

