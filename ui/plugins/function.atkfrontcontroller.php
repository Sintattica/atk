<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage ui
   *
   * @copyright (c)2007 Ibuildings.nl
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5798 $
   * $Id$
   */

  /**
   * atkFrontController plug-in.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @param array $params
   * @param Smarty $smarty
   * @return string result
   */
  function smarty_function_atkfrontcontroller($params, $smarty)
  {
    atkimport('atk.front.atkfrontcontroller');
    return atkFrontController::dispatchRequest($params);
  }