<?php

/**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2006 Kevin Lwinmoe
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /** @internal includes **/
  atkimport("atk.atklanguage");
  useattrib("atklistattribute");

  /**
   * The atkStateAttribute class represents an attribute to handle United States in a listbox.
   * It extends atkListAttribute.
   *
   * @author Kevin Lwinmoe <kevin.lwinmoe@gmail.com>
   * @package atk
   * @subpackage attributes
   */
  class atkStateAttribute extends atkListAttribute
  {
    var $m_states       = array();


   var $m_usa_states   = array('AL', 'AK', 'AS', 'AZ', 'AR', 'CA', 'CO', 'CT', 'DE', 'DC', 'FM', 'FL',
                                'GA', 'GU', 'HI', 'ID', 'IL', 'IN', 'IA', 'KS', 'KY', 'LA', 'ME', 'MH',
                                'MD', 'MA', 'MI', 'MN', 'MS', 'MO', 'MT', 'NE', 'NV', 'NH', 'NJ', 'NM',
                                'NY', 'NC', 'ND', 'MP', 'OH', 'OK', 'OR', 'PW', 'PA', 'PR', 'RI', 'SC',
                                'SD', 'TN', 'TX', 'UT', 'VT', 'VI', 'VA', 'WA', 'WV', 'WI', 'WY');

   var $m_defaulttocurrent=true;


   /**
    * Constructor
    * <b>Example:</b>
    * $this->add(new atkStateAttribute("state_abbrev", AF_OBLIGATORY | AF_SEARCHABLE));
    * state_abbrev is the database attribute that holds state abbrevation data as AK,CA,NY
    * It will display the full state name.
    *
    * @param $name Name of the attribute
    * @param $switch Choose usa for USA states
    * @param $optionArray Array with options
    * @param $valueArray Array with values. If you don't use this parameter,
    *                    values are assumed to be the same as the options.
    * @param $flags Flags for the attribute
    * @param bool $defaulttocurrent Set the default selected state to the 
    *                               current state based on the atk language
    */
    function atkStateAttribute($name,$switch="usa",$optionArray="", $valueArray="", $flags = 0,$defaulttocurrent=true)
    {

      if (is_numeric($switch))
      {
        $flags = $switch;
        $switch = "usa";
      }

      // When switch is not user get country options
      atkdebug("StateAttribute - $name - $switch");

      $this->fillStateArray();
      $valueArray   = $this->getStateValueArray($switch);
      $optionsArray = $this->getStateOptionArray($switch);

      $this->m_defaulttocurrent = $defaulttocurrent;
      $this->atkListAttribute($name, $optionsArray,$valueArray, $flags|AF_NO_TRANSLATION, 0);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record="",$fieldprefix="",$mode="")
    {
      if ($this->m_defaulttocurrent && !$record[$this->fieldName()]) $record[$this->fieldName()] = strtoupper(atkLanguage::getLanguage());
      return parent::edit($record,$fieldprefix);
    }

    /**
     * Fill the state array
     *
     */
    function fillStateArray()
    {
      $this->m_state['AL']['en'] = "ALABAMA";
      $this->m_state['AK']['en'] = "ALASKA";
      $this->m_state['AS']['en'] = "AMERICAN SAMOA";
      $this->m_state['AZ']['en'] = "ARIZONA";
      $this->m_state['AR']['en'] = "ARKANSAS";
      $this->m_state['CA']['en'] = "CALIFORNIA";
      $this->m_state['CO']['en'] = "COLORADO";
      $this->m_state['CT']['en'] = "CONNECTICUT";
      $this->m_state['DE']['en'] = "DELAWARE";
      $this->m_state['DC']['en'] = "DISTRICT OF COLUMBIA";
      $this->m_state['FM']['en'] = "FEDERATED STATES OF MICRONESIA";
      $this->m_state['FL']['en'] = "FLORIDA";
      $this->m_state['GA']['en'] = "GEORGIA";
      $this->m_state['GU']['en'] = "GUAM";
      $this->m_state['HI']['en'] = "HAWAII";
      $this->m_state['ID']['en'] = "IDAHO";
      $this->m_state['IL']['en'] = "ILLINOIS";
      $this->m_state['IN']['en'] = "INDIANA";
      $this->m_state['IA']['en'] = "IOWA";
      $this->m_state['KS']['en'] = "KANSAS";
      $this->m_state['KY']['en'] = "KENTUCKY";
      $this->m_state['LA']['en'] = "LOUISIANA";
      $this->m_state['ME']['en'] = "MAINE";
      $this->m_state['MH']['en'] = "MARSHALL ISLANDS";
      $this->m_state['MD']['en'] = "MARYLAND";
      $this->m_state['MA']['en'] = "MASSACHUSETTS";
      $this->m_state['MI']['en'] = "MICHIGAN";
      $this->m_state['MN']['en'] = "MINNESOTA";
      $this->m_state['MS']['en'] = "MISSISSIPPI";
      $this->m_state['MO']['en'] = "MISSOURI";
      $this->m_state['MT']['en'] = "MONTANA";
      $this->m_state['NE']['en'] = "NEBRASKA";
      $this->m_state['NV']['en'] = "NEVADA";
      $this->m_state['NH']['en'] = "NEW HAMPSHIRE";
      $this->m_state['NJ']['en'] = "NEW JERSEY";
      $this->m_state['NM']['en'] = "NEW MEXICO";
      $this->m_state['NY']['en'] = "NEW YORK";
      $this->m_state['NC']['en'] = "NORTH CAROLINA";
      $this->m_state['ND']['en'] = "NORTH DAKOTA";
      $this->m_state['MP']['en'] = "NORTHERN MARIANA ISLANDS";
      $this->m_state['OH']['en'] = "OHIO";
      $this->m_state['OK']['en'] = "OKLAHOMA";
      $this->m_state['OR']['en'] = "OREGON";
      $this->m_state['PW']['en'] = "PALAU";
      $this->m_state['PA']['en'] = "PENNSYLVANIA";
      $this->m_state['PR']['en'] = "PUERTO RICO";
      $this->m_state['RI']['en'] = "RHODE ISLAND";
      $this->m_state['SC']['en'] = "SOUTH CAROLINA";
      $this->m_state['SD']['en'] = "SOUTH DAKOTA";
      $this->m_state['TN']['en'] = "TENNESSEE";
      $this->m_state['TX']['en'] = "TEXAS";
      $this->m_state['UT']['en'] = "UTAH";
      $this->m_state['VT']['en'] = "VERMONT";
      $this->m_state['VI']['en'] = "VIRGIN ISLANDS";
      $this->m_state['VA']['en'] = "VIRGINIA";
      $this->m_state['WA']['en'] = "WASHINGTON";
      $this->m_state['WV']['en'] = "WEST VIRGINIA";
      $this->m_state['WI']['en'] = "WISCONSIN";
      $this->m_state['WY']['en'] = "WYOMING";
    }

    /**
     * Get the state value array
     *
     * @param string $switch
     * @return array with state values
     */
    function getStateValueArray($switch)
    {
      if($switch=="usa")
      {
         return $this->m_usa_states;
      }
      else
      {
         $tmp_array=array();
         foreach($this->m_state as $iso => $value)
           $tmp_array[]=$iso;
         return $tmp_array;
      }
    }

    /**
     * Get the state option array
     *
     * @param string $switch
     * @return array with state options
     */
    function getStateOptionArray($switch)
    {
      $tmp_array = array();
      if($switch=="usa")
      {
         foreach($this->m_usa_states as $iso)
          $tmp_array[]=$this->getStateOption($iso);
      }
      else
      {
         foreach($this->m_state as $iso => $value)
           $tmp_array[]=$value;
      }
      return $tmp_array;
    }

    /**
     * Get the state option based on the language
     *
     * @param string $iso_code
     * @return String The state option
     */
    function getStateOption($iso_code)
    {
       return $this->m_state[$iso_code][atkLanguage::getLanguage()];
    }
  }
?>