<?php

  require_once(atkconfig("atkroot")."atk/db/class.atkoci8db.inc");

  /**
   * Handles database connectivity and database interaction
   * with the Oracle database server version 8.0.5. 
   * (This class might also work with 8.0.x versions prior
   * to 8.0.5)
   *
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$   
   *
   */
  class atkoci805db extends atkoci8db
  {    
    /**
     * Base constructor
     */
    function atkoci805db()
    {
       return $this->atkoci8db();
    }    

  }

?>
