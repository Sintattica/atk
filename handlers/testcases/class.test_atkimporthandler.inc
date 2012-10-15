<?php

    /**
     * This file is part of the Achievo ATK distribution.
     * Detailed copyright and licensing information can be found
     * in the doc/COPYRIGHT and doc/LICENSE files which should be 
     * included in the distribution.
     *
     * @package atk
     * @subpackage handlers
     * @access private
     *
     * @copyright (c)2009 Ibuildings
     * @license http://www.atkframework.com/licensing ATK Open Source License
     */
  
    /**
     * Include the class we are about to test.
     */
    atkimport("atk.handlers.atkimporthandler");
  
    /**
     * Testcases for class atkImportHandler
     *
     * @author Yury Golovnya <yury@achievo.org>
     * @package atk
     * @subpackage handlers
     * @access private
     */
    class test_atkimporthandler extends atkTestCase
    {    
      /**
       * Test method. 
       */
     
      function test_fgetcsvfromline()
      {
        $result = array("R902352050","PORTECLEF CAOUTCHOUC LOGAN","50","24,209489304");

        $line = '"R902352050","PORTECLEF CAOUTCHOUC LOGAN",50,"24,209489304"';
        $output = atkImportHandler::fgetcsvfromline($line, 4, $delimiterChar = ',', $enclosureChar = '"');
        $this->assertEqual($result, $output);      

        $line = '"R902352050","PORTECLEF CAOUTCHOUC LOGAN","50","24,209489304"';
        $output = atkImportHandler::fgetcsvfromline($line, 4, $delimiterChar = ',', $enclosureChar = '"');
        $this->assertEqual($result, $output);      

        $line = 'R902352050;PORTECLEF CAOUTCHOUC LOGAN;50;24,209489304';
        $output = atkImportHandler::fgetcsvfromline($line, 4, $delimiterChar = ';', $enclosureChar = '');
        $this->assertEqual($result, $output);      

        $result = array("","PORTECLEF CAOUTCHOUC LOGAN","50","");

        $line = '"","PORTECLEF CAOUTCHOUC LOGAN","50",""';
        $output = atkImportHandler::fgetcsvfromline($line, 4, $delimiterChar = ',', $enclosureChar = '"');
        $this->assertEqual($result, $output);      

        $line = ',"PORTECLEF CAOUTCHOUC LOGAN","50",';
        $output = atkImportHandler::fgetcsvfromline($line, 4, $delimiterChar = ',', $enclosureChar = '"');
        $this->assertEqual($result, $output);      

        $line = ',PORTECLEF CAOUTCHOUC LOGAN,50,';
        $output = atkImportHandler::fgetcsvfromline($line, 4, $delimiterChar = ',', $enclosureChar = '');
        $this->assertEqual($result, $output);      

        $result = array("R902352050","PORTECLEF CAOUTCHOUC LOGAN","","24,209489304");

        $line = '"R902352050","PORTECLEF CAOUTCHOUC LOGAN","","24,209489304"';
        $output = atkImportHandler::fgetcsvfromline($line, 4, $delimiterChar = ',', $enclosureChar = '"');
        $this->assertEqual($result, $output);      

        $line = '"R902352050","PORTECLEF CAOUTCHOUC LOGAN",,"24,209489304"';
        $output = atkImportHandler::fgetcsvfromline($line, 4, $delimiterChar = ',', $enclosureChar = '"');
        $this->assertEqual($result, $output);      

        $line = 'R902352050;PORTECLEF CAOUTCHOUC LOGAN;;24,209489304';
        $output = atkImportHandler::fgetcsvfromline($line, 4, $delimiterChar = ';', $enclosureChar = '');
        $this->assertEqual($result, $output);      
      }      
    }

?>
