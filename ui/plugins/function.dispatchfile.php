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
   * @copyright (c)2007 Sandy Pleyte <sandy@achievo.org>
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Returns the dispatch file.
   *
   * @author Sandy Pleyte <sandy@achievo.org>
   *
   */
  function smarty_function_dispatchfile($params, &$smarty)
  {
    return getDispatchFile();
  }
?>