<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage wizard
   *
   * @author maurice <maurice@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license see doc/LICENSE
   *
   * @version $Revision$
   * $Id$
   */

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
      if(is_array($wizardAction))
        return key($wizardAction);
      else
        return $wizardAction;
    }
  }

?>