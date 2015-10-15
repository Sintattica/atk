<?php namespace Sintattica\Atk\Wizard;

/**
 * Converts a atkwizardaction key array to string value.
 *
 * @author maurice <maurice@ibuildings.nl>
 * @package atk
 * @subpackage wizard
 *
 */
class AtkWizardActionLoader
{

    /**
     * Get the wizard action
     *
     * @param array|string $wizardAction The wizard action
     * @return String the wizard action
     */
    function getWizardAction($wizardAction)
    {
        if (is_array($wizardAction)) {
            return key($wizardAction);
        } else {
            return $wizardAction;
        }
    }

}

