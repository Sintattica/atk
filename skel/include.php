<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * This file is the skeleton main include wrapper, which you can copy
   * to your application dir and modify if necessary. It is used to 
   * include popups in a safe manner. Any popup loaded with this wrapper
   * has session support and login support. 
   * Only files defined in the $config_allowed_includes array are allowed
   * to be included.
   *
   * @package atk
   * @subpackage skel
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal includes
   */
  $config_atkroot = "./";
  include_once("atk.inc");

  atksession();
  atksecure();

  $file = $ATK_VARS["file"];
  $allowed = atkconfig("allowed_includes");
  if (atk_in_array($file, $allowed))
    include_once(atkconfig("atkroot").$file);
?>