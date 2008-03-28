<?php

    /**
     * This file is part of the Achievo ATK distribution.
     * Detailed copyright and licensing information can be found
     * in the doc/COPYRIGHT and doc/LICENSE files which should be
     * included in the distribution.
     *
     * @package atk
     * @subpackage attributes
     * @access private
     *
     * @copyright (c)2005 Ivo Jansch
     * @license http://www.achievo.org/atk/licensing ATK Open Source License
     *
     * @version $Revision$
     * $Id$
     */

    /**
     * Include the class we are about to test.
     */
    atkimport("atk.attributes.atkattribute");

    /**
     * Testcases for class atkattribute.
     *
     * @author Ivo Jansch <ivo@achievo.org>
     * @package atk
     * @subpackage attributes
     * @access private
     */
    class test_atkattribute extends atkAttributeTestCase
    {
      /**
       * Setup the testcase.
       */
      function setup()
      {
      }

      /**
       * Test method.
       */
      function test_attribute()
      {
        $this->apiTest(new atkAttribute("name"));
      }

      function test_encoding()
      {
        // check encoding
        $db = &atkGetDb();
        $attribute = new atkAttribute("name");
        $dbvalue = "contains'quote";
        $internalvalue = $attribute->db2value(array("name"=>$dbvalue));
        $roundtrip = $attribute->value2db(array("name"=>$internalvalue));
        $this->assertEqual($db->escapeSQL($dbvalue), $roundtrip, "value2db encoding check %s");
      }

      /**
       *
       */
      function test_search()
      {
        $attribute = new atkAttribute("username");
        $attribute->setOwnerInstance(new atkNode());

        $record = array();

        $result = $attribute->search($record, false, "");
        $this->assertWantedPattern('/name="atksearch_AE_username"/', $result);

        $result = $attribute->search($record, false, "person_AE_");
        $this->assertWantedPattern('/name="atksearch_AE_person_AE_username"/', $result);

        $result = $attribute->search($record, false, "group_AE_person_AE_");
        $this->assertWantedPattern('/name="atksearch_AE_group_AE_person_AE_username"/', $result);

        $result = $attribute->search($record, true, "");
        $this->assertWantedPattern('/name="atksearch_AE_username"/', $result);

        $result = $attribute->search($record, true, "person_AE_");
        $this->assertWantedPattern('/name="atksearch_AE_person_AE_username"/', $result);

        $result = $attribute->search($record, true, "group_AE_person_AE_");
        $this->assertWantedPattern('/name="atksearch_AE_group_AE_person_AE_username"/', $result);
      }

      function test_searchmode()
      {
        $attribute = new atkAttribute("username");
        $attribute->setOwnerInstance(new atkNode());

        $result = $attribute->searchMode(false, "");
        $this->assertWantedPattern('/type="hidden"/', $result);
        $this->assertWantedPattern('/name="atksearchmode_AE_username"/', $result);

        $result = $attribute->searchMode(false, "person_AE_");
        $this->assertWantedPattern('/type="hidden"/', $result);
        $this->assertWantedPattern('/name="atksearchmode_AE_person_AE_username"/', $result);

        $result = $attribute->searchMode(false, "group_AE_person_AE_");
        $this->assertWantedPattern('/type="hidden"/', $result);
        $this->assertWantedPattern('/name="atksearchmode_AE_group_AE_person_AE_username"/', $result);

        $result = $attribute->searchMode(true, "");
        $this->assertWantedPattern('/<select/', $result);
        $this->assertWantedPattern('/name="atksearchmode_AE_username"/', $result);

        $result = $attribute->searchMode(true, "person_AE_");
        $this->assertWantedPattern('/<select/', $result);
        $this->assertWantedPattern('/name="atksearchmode_AE_person_AE_username"/', $result);

        $result = $attribute->searchMode(true, "group_AE_person_AE_");
        $this->assertWantedPattern('/<select/', $result);
        $this->assertWantedPattern('/name="atksearchmode_AE_group_AE_person_AE_username"/', $result);
      }
    }

?>