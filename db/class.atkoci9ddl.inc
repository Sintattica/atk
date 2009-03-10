<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage db
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /** @internal Include base class */
  atkimport("atk.db.atkoci8ddl");
   
  /**
   * Oracle 9i ddl driver. 
   *
   * Based on 8i ddl class. Should work with Oracle 10i databases too.
   *
   * @author Martin Roest <martin@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */  
  class atkOci9DDL extends atkOci8DDL
  {
    /**
     * Constructor
     *
     * @return atkOci9DDL
     */
    function atkOci9DDL()
    {
    }
  }
?>