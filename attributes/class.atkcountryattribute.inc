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
   * The atkCountry class represents an attribute to handle ISO Countries in a listbox.
   *
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   */
  class atkCountryAttribute extends atkListAttribute
  {
    var $m_countries           = array();
    var $m_europe_countries    = array( 'AL','AT','BE','BA','BG','HR','CY','CZ','DK','DE','FI','FR','GB','GR','HU','IE','IT','LI',
                                        'LU','MC','NL','NO','PL','PT','RO','SK','SI','ES','SE','CH','TR');
    var $m_benelux_countries   = array( 'NL','BE','LU');


    var $m_world_countries_shortlist = array( 'AT', 'BE', 'CA', 'DK', 'FI', 'FR', 'DE', 'GR', 'IE', 'IT', 'LU', 'NL', 'PT', 'ES', 'SE', 'GB', 'US' );

    var $m_world_countries     = array( 'AF', 'AL', 'DZ', 'AS', 'AD', 'AO', 'AI', 'AQ', 'AG', 'AR', 'AM', 'AW', 'AU', 'AT', 'AZ',
                                        'BS', 'BH', 'BD', 'BB', 'BY', 'BE', 'BZ', 'BJ', 'BM', 'BO', 'BA', 'BW', 'BV', 'BR', 'IO', 'BN',
                                        'BG', 'BF', 'BI', 'BT', 'KH', 'CM', 'CA', 'CV', 'KY', 'CF', 'TD', 'CL', 'CN', 'CX', 'CC', 'CO',
                                        'KM', 'CG', 'CK', 'CR', 'HR', 'CU', 'CY', 'CZ', 'DK', 'DE', 'DJ', 'DM', 'DO', 'TP', 'EC',
                                        'EG', 'SV', 'GQ', 'EE', 'ET', 'FK', 'FO', 'FJ', 'FI', 'FR', 'FX', 'TF', 'GA', 'GM', 'GE', 'GH',
                                        'GI', 'GB', 'GR', 'GL', 'GD', 'GP', 'GU', 'GT', 'GN', 'GW', 'GY', 'GF', 'HT', 'HM', 'HN', 'HK',
                                        'HU', 'IS', 'IN', 'ID', 'IR', 'IQ', 'IE', 'IL', 'IT', 'CI', 'JM', 'JP', 'JO', 'KZ', 'KE', 'KG',
                                        'KI', 'KP', 'KR', 'KW', 'LA', 'LV', 'LB', 'LS', 'LR', 'LY', 'LI', 'LT', 'LU', 'MO', 'MG', 'MW',
                                        'MY', 'MV', 'ML', 'MT', 'MH', 'MQ', 'MR', 'MU', 'MX', 'FM', 'MD', 'MC', 'MN', 'MS', 'MA', 'MZ',
                                        'MM', 'NA', 'NR', 'NL', 'AN', 'NP', 'NC', 'NZ', 'NI', 'NE', 'NG', 'NU', 'NF', 'MP', 'NO', 'OM',
                                        'PK', 'PW', 'PA', 'PG', 'PY', 'PE', 'PH', 'PN', 'PL', 'PF', 'PT', 'PR', 'QA', 'RE', 'RO', 'RU',
                                        'RW', 'LC', 'WS', 'SM', 'SA', 'SN', 'SC', 'SL', 'SG', 'SK', 'SI', 'SB', 'SO', 'ZA', 'ES',
                                        'LK', 'SH', 'PM', 'ST', 'KN', 'VC', 'SD', 'SR', 'SJ', 'SZ', 'SE', 'CH', 'SY', 'TJ', 'TW', 'TZ',
                                        'TH', 'TG', 'TK', 'TO', 'TT', 'TN', 'TR', 'TM', 'TC', 'TV', 'UM', 'UG', 'UA', 'AE', 'GB', 'US',
                                        'UY', 'UZ', 'VU', 'VA', 'VE', 'VN', 'VG', 'VI', 'WF', 'EH', 'YE', 'ZR', 'ZM', 'ZW');

   var $m_custom_countries = array();

   var $m_defaulttocurrent=true;


   /**
    * Constructor
    *
    * <b>Example:</b>
    *        $this->add(new atkCountryAttribute("zipcode","country",AF_OBLIGATORY));
    * @param $name Name of the attribute
    * @param $switch Can be benelux, europe, world, user
                     If user, it will use the option and value Array.
    * @param $optionArray Array with options
    * @param $valueArray Array with values. If you don't use this parameter,
    *                    values are assumed to be the same as the options.
    * @param $flags Flags for the attribute
    */
    function atkCountryAttribute($name,$switch="world",$optionArray="", $valueArray="", $flags = 0,$defaulttocurrent=true)
    {
      if (is_numeric($switch))
      {
        $flags = $switch;
        $switch = "world";
      }

      // When switch is not user get country options
      atkdebug("CountryAttribute - $name - $switch");
      if($switch!="user")
      {
        //we assume that the 3 param is flags
         if(is_numeric($optionArray)) $flags = $optionArray;

         if ($switch != "world")
         {
           $this->fillCountriesArray();
         }
         else
         {
           $this->fillWorldCountriesArray();
         }

         $optionsArray = $this->getCountryOptionArray($switch);
         $valueArray   = $this->getCountryValueArray($switch);
      }
      $this->m_defaulttocurrent = $defaulttocurrent;
      $this->atkListAttribute($name,$optionsArray,$valueArray, $flags|AF_NO_TRANSLATION, 0);
    }

    /**
     * With this function you can set the "custom" list with a array of country ISO codes
     * that are KNOWN to this class. (they should be in the world list atleast)
     *
     * @param array $countries  CountryIso codes, like NL BE LU etc...
     * @return bool             It will return false when the resulting custom list is empty.
     */
    function setList($countries)
    {
      $custom_list = array();
      foreach ($countries as $countryIso)
      {
        $countryIso = strtoupper($countryIso);
        if (in_array($countryIso,$this->m_world_countries))
        {
          array_push($custom_list,$countryIso);
        }
        else
        {
          atkwarning('atkCountryAttribute: setList: '.$countryIso.' is a unknown country and will be ignored');
        }
      }

      if (count($custom_list)==0)
      {
        atkerror('atkCountryAttribute: setList: empty custom country list');
        return false;
      }

      $this->m_custom_countries = $custom_list;
      return true;
    }

    function edit($record="",$fieldprefix="")
    {
      if ($this->m_defaulttocurrent && !$record[$this->fieldName()]) $record[$this->fieldName()] = strtoupper(atkLanguage::getLanguage());
      return parent::edit($record,$fieldprefix);
    }

    function getCountryOptionArray($switch)
    {
      $tmp_array = array();
      if($switch=="benelux")
      {
        foreach($this->m_benelux_countries as $iso)
          $tmp_array[]=$this->getCountryOption($iso);
      }
      elseif($switch=="europe")
      {
        foreach($this->m_europe_countries as $iso)
          $tmp_array[]=$this->getCountryOption($iso);
      }
      elseif($switch=="world")
      {
        foreach($this->m_world_countries as $iso)
          $tmp_array[]=$this->getCountryOption($iso);
      }
      elseif($switch=="world_shortlist")
      {
        foreach($this->m_world_countries_shortlist as $iso)
          $tmp_array[]=$this->getCountryOption($iso);
      }
      elseif($switch=="custom")
      {
        foreach($this->m_custom_countries as $iso)
          $tmp_array[]=$this->getCountryOption($iso);
      }
      else
      {
        foreach($this->m_country as $iso => $value)
           $tmp_array[]=$value;
      }
      return $tmp_array;
    }

    function getCountryValueArray($switch)
    {
      if($switch=="benelux")
      {
        return $this->m_benelux_countries;
      }
      elseif($switch=="europe")
      {
        return $this->m_europe_countries;
      }
      elseif($switch=="world")
      {
        return $this->m_world_countries;
      }
      elseif($switch=="world_shortlist")
      {
        return $this->m_world_countries_shortlist;
      }
      elseif($switch=="custom")
      {
        return $this->m_custom_countries;
      }
      else
      {
        $tmp_array=array();
        foreach($this->m_country as $iso => $value)
          $tmp_array[]=$iso;
        return $tmp_array;
      }
    }

    /**
     * Get Country option, when current language doesn't exists
     * it will return the english language.
     *
     * @param string $iso_code 2 Letter iso code of the country
     * @return string Country name
     */
    function getCountryOption($iso_code)
    {
      $lng = atkLanguage::getLanguage();
      if(!array_key_exists($iso_code,$this->m_country)) atkdebug('UNKNOWN ISO CODE: '.$iso_code);
      if(array_key_exists($lng,$this->m_country[$iso_code]))
      {
        return $this->m_country[$iso_code][$lng];
      }
      else
      {
        return $this->m_country[$iso_code]['en'];
      }
    }

    function fillCountriesArray()
    {
      $this->m_country["AL"]["nl"] = html_entity_decode("Albani&euml;", null, atktext('charset'));
      $this->m_country["AL"]["de"] = "Albania";
      $this->m_country["AL"]["en"] = "Albania";

      $this->m_country["AT"]["nl"] = "Oostenrijk";
      $this->m_country["AT"]["de"] = "Austria";
      $this->m_country["AT"]["en"] = "Austria";

      $this->m_country['BE']['nl'] = html_entity_decode("Belgi&euml;", null, atktext('charset'));
      $this->m_country['BE']['de'] = "Belgien";
      $this->m_country['BE']['en'] = "Belgium";

      $this->m_country["BA"]["nl"] = html_entity_decode("Bosni&euml;-Herzegovina", null, atktext('charset'));
      $this->m_country["BA"]["de"] = "Bosnia-Herzegovina";
      $this->m_country["BA"]["en"] = "Bosnia-Herzegovina";

      $this->m_country["BG"]["nl"] = "Bulgarije";
      $this->m_country["BG"]["de"] = "Bulgaria";
      $this->m_country["BG"]["en"] = "Bulgaria";

      $this->m_country['CH']['nl'] = "Zwitserland";
      $this->m_country['CH']['de'] = "Die Schweiz";
      $this->m_country['CH']['en'] = "Switzerland";

      $this->m_country["CY"]["nl"] = "Cyprus";
      $this->m_country["CY"]["de"] = "Cyprus";
      $this->m_country["CY"]["en"] = "Cyprus";

      $this->m_country["CZ"]["nl"] = html_entity_decode("Tsjechi&euml;", null, atktext('charset'));
      $this->m_country["CZ"]["de"] = "Czech Republic";
      $this->m_country["CZ"]["en"] = "Czech Republic";

      $this->m_country["CS"]["nl"] = "Tsjechoslowakije";
      $this->m_country["CS"]["de"] = "Czechoslovakia";
      $this->m_country["CS"]["en"] = "Czechoslovakia";

      $this->m_country["DK"]["nl"] = "Denemarken";
      $this->m_country["DK"]["de"] = "Denmark";
      $this->m_country["DK"]["en"] = "Denmark";

      $this->m_country['DE']['nl'] = "Duitsland";
      $this->m_country['DE']['de'] = "Deutschland";
      $this->m_country['DE']['en'] = "Germany";

      $this->m_country["ES"]["nl"] = "Spanje";
      $this->m_country["ES"]["de"] = "Spain";
      $this->m_country["ES"]["en"] = "Spain";

      $this->m_country['FI']['nl'] = "Finland";
      $this->m_country['FI']['de'] = "Finnland";
      $this->m_country['FI']['en'] = "Finland";

      $this->m_country['FR']['nl'] = "Frankrijk";
      $this->m_country['FR']['de'] = "Frankreich";
      $this->m_country['FR']['en'] = "France";

      $this->m_country['GB']['nl'] = html_entity_decode("Groot-Brittanni&euml;", null, atktext('charset'));
      $this->m_country['GB']['de'] = "England";
      $this->m_country['GB']['en'] = "Great Britain";

      $this->m_country["GR"]["nl"] = "Griekenland";
      $this->m_country["GR"]["de"] = "Greece";
      $this->m_country["GR"]["en"] = "Greece";

      $this->m_country["HR"]["nl"] = html_entity_decode("Kroati&euml;", null, atktext('charset'));
      $this->m_country["HR"]["de"] = "Croatia";
      $this->m_country["HR"]["en"] = "Croatia";

      $this->m_country["HU"]["nl"] = "Hongarije";
      $this->m_country["HU"]["de"] = "Hungary";
      $this->m_country["HU"]["en"] = "Hungary";

      $this->m_country['IS']['nl'] = "IJsland";
      $this->m_country['IS']['de'] = "Island";
      $this->m_country['IS']['en'] = "Iceland";

      $this->m_country['IE']['nl'] = "Ierland";
      $this->m_country['IE']['de'] = "Irland";
      $this->m_country['IE']['en'] = "Ireland";

      $this->m_country['IT']['nl'] = html_entity_decode("Itali&euml;", null, atktext('charset'));
      $this->m_country['IT']['de'] = "Italien";
      $this->m_country['IT']['en'] = "Italy";

      $this->m_country["LI"]["nl"] = "Liechtenstein";
      $this->m_country["LI"]["de"] = "Liechtenstein";
      $this->m_country["LI"]["en"] = "Liechtenstein";

      $this->m_country['LU']['nl'] = "Luxemburg";
      $this->m_country['LU']['de'] = "Luxemburg";
      $this->m_country['LU']['en'] = "Luxemburg";

      $this->m_country["MC"]["nl"] = "Monaco";
      $this->m_country["MC"]["de"] = "Monaco";
      $this->m_country["MC"]["en"] = "Monaco";

      $this->m_country['NL']['nl'] = "Nederland";
      $this->m_country['NL']['de'] = "Die Niederlande";
      $this->m_country['NL']['en'] = "The Netherlands";

      $this->m_country['NO']['nl'] = "Noorwegen";
      $this->m_country['NO']['de'] = "Norwegen";
      $this->m_country['NO']['en'] = "Norway";

      $this->m_country['PL']['nl'] = "Polen";
      $this->m_country['PL']['de'] = "Polen";
      $this->m_country['PL']['en'] = "Poland";

      $this->m_country["PT"]["nl"] = "Portugal";
      $this->m_country["PT"]["de"] = "Portugal";
      $this->m_country["PT"]["en"] = "Portugal";

      $this->m_country["RO"]["nl"] = html_entity_decode("Roemeni&euml;", null, atktext('charset'));
      $this->m_country["RO"]["de"] = "Romania";
      $this->m_country["RO"]["en"] = "Romania";

      $this->m_country['SE']['nl'] = "Zweden";
      $this->m_country['SE']['de'] = "Schweden";
      $this->m_country['SE']['en'] = "Sweden";

      $this->m_country["SK"]["nl"] = html_entity_decode("Slowakij&euml;", null, atktext('charset'));
      $this->m_country["SK"]["de"] = "Slovak Republic";
      $this->m_country["SK"]["en"] = "Slovak Republic";

      $this->m_country["SI"]["nl"] = html_entity_decode("Sloveni&euml;", null, atktext('charset'));
      $this->m_country["SI"]["de"] = "Slovenia";
      $this->m_country["SI"]["en"] = "Slovenia";

      $this->m_country["TR"]["nl"] = "Turkije";
      $this->m_country["TR"]["de"] = "Turkey";
      $this->m_country["TR"]["en"] = "Turkey";

      $this->m_country['YU']['nl'] = html_entity_decode("Joegoslavi&euml;", null, atktext('charset'));
      $this->m_country['YU']['de'] = "Jugoslawien";
      $this->m_country['YU']['en'] = "Yugoslavia";
    }

    function fillWorldCountriesArray()
    {

      $this->m_country["AF"]["nl"] ="Afghanistan";
      $this->m_country["AF"]["de"] ="Afghanistan";
      $this->m_country["AF"]["en"] ="Afghanistan";

      $this->m_country["AL"]["nl"] = html_entity_decode("Albani&euml;", null, atktext('charset'));
      $this->m_country["AL"]["de"] ="Albania";
      $this->m_country["AL"]["en"] ="Albania";

      $this->m_country["DZ"]["nl"] ="Algerije";
      $this->m_country["DZ"]["de"] ="Algeria";
      $this->m_country["DZ"]["en"] ="Algeria";

      $this->m_country["AS"]["nl"] ="Amerikaans-Samoa";
      $this->m_country["AS"]["de"] ="American Samoa";
      $this->m_country["AS"]["en"] ="American Samoa";

      $this->m_country["AD"]["nl"] ="Andorra";
      $this->m_country["AD"]["de"] ="Andorra";
      $this->m_country["AD"]["en"] ="Andorra";

      $this->m_country["AO"]["nl"] ="Angola";
      $this->m_country["AO"]["de"] ="Angola";
      $this->m_country["AO"]["en"] ="Angola";

      $this->m_country["AI"]["nl"] ="Anguilla";
      $this->m_country["AI"]["de"] ="Anguilla";
      $this->m_country["AI"]["en"] ="Anguilla";

      $this->m_country["AQ"]["nl"] ="Antarctica";
      $this->m_country["AQ"]["de"] ="Antarctica";
      $this->m_country["AQ"]["en"] ="Antarctica";

      $this->m_country["AG"]["nl"] ="Antigua en Barbuda";
      $this->m_country["AG"]["de"] ="Antigua and Barbuda";
      $this->m_country["AG"]["en"] ="Antigua and Barbuda";

      $this->m_country["AR"]["nl"] = html_entity_decode("Argentini&euml;", null, atktext('charset'));
      $this->m_country["AR"]["de"] ="Argentina";
      $this->m_country["AR"]["en"] ="Argentina";

      $this->m_country["AM"]["nl"] = html_entity_decode("Armeni&euml;", null, atktext('charset'));
      $this->m_country["AM"]["de"] ="Armenia";
      $this->m_country["AM"]["en"] ="Armenia";

      $this->m_country["AW"]["nl"] ="Aruba";
      $this->m_country["AW"]["de"] ="Aruba";
      $this->m_country["AW"]["en"] ="Aruba";

      $this->m_country["AU"]["nl"] = html_entity_decode("Australi&euml;", null, atktext('charset'));
      $this->m_country["AU"]["de"] ="Australia";
      $this->m_country["AU"]["en"] ="Australia";

      $this->m_country["AT"]["nl"] ="Oosterijk";
      $this->m_country["AT"]["de"] ="Austria";
      $this->m_country["AT"]["en"] ="Austria";

      $this->m_country["AZ"]["nl"] ="Azerbeidzjan";
      $this->m_country["AZ"]["de"] ="Azerbaidjan";
      $this->m_country["AZ"]["en"] ="Azerbaidjan";

      $this->m_country["BS"]["nl"] ="Bahamas";
      $this->m_country["BS"]["de"] ="Bahamas";
      $this->m_country["BS"]["en"] ="Bahamas";

      $this->m_country["BH"]["nl"] ="Bahrein";
      $this->m_country["BH"]["de"] ="Bahrain";
      $this->m_country["BH"]["en"] ="Bahrain";

      $this->m_country["BD"]["nl"] ="Bangladesh";
      $this->m_country["BD"]["de"] ="Bangladesh";
      $this->m_country["BD"]["en"] ="Bangladesh";

      $this->m_country["BB"]["nl"] ="Barbados";
      $this->m_country["BB"]["de"] ="Barbados";
      $this->m_country["BB"]["en"] ="Barbados";

      $this->m_country["BY"]["nl"] ="Belarus";
      $this->m_country["BY"]["de"] ="Belarus";
      $this->m_country["BY"]["en"] ="Belarus";

      $this->m_country['BE']['nl'] = html_entity_decode("Belgi&euml;", null, atktext('charset'));
      $this->m_country['BE']['de'] = "Belgien";
      $this->m_country['BE']['en'] = "Belgium";

      $this->m_country["BZ"]["nl"] ="Belize";
      $this->m_country["BZ"]["de"] ="Belize";
      $this->m_country["BZ"]["en"] ="Belize";

      $this->m_country["BJ"]["nl"] ="Benin";
      $this->m_country["BJ"]["de"] ="Benin";
      $this->m_country["BJ"]["en"] ="Benin";

      $this->m_country["BM"]["nl"] ="Bermuda";
      $this->m_country["BM"]["de"] ="Bermuda";
      $this->m_country["BM"]["en"] ="Bermuda";

      $this->m_country["BO"]["nl"] ="Bolivia";
      $this->m_country["BO"]["de"] ="Bolivia";
      $this->m_country["BO"]["en"] ="Bolivia";

      $this->m_country["BA"]["nl"] = html_entity_decode("Bosni&euml;-Herzegovina", null, atktext('charset'));
      $this->m_country["BA"]["de"] ="Bosnia-Herzegovina";
      $this->m_country["BA"]["en"] ="Bosnia-Herzegovina";

      $this->m_country["BW"]["nl"] ="Botswana";
      $this->m_country["BW"]["de"] ="Botswana";
      $this->m_country["BW"]["en"] ="Botswana";

      $this->m_country["BV"]["nl"] ="Bouvet";
      $this->m_country["BV"]["de"] ="Bouvet Island";
      $this->m_country["BV"]["en"] ="Bouvet Island";

      $this->m_country["BR"]["nl"] = html_entity_decode("Brazili&euml;", null, atktext('charset'));
      $this->m_country["BR"]["de"] ="Brazil";
      $this->m_country["BR"]["en"] ="Brazil";

      $this->m_country["IO"]["nl"] ="Brits Territorium in de Indische Oceaan";
      $this->m_country["IO"]["de"] ="British Indian O. Terr.";
      $this->m_country["IO"]["en"] ="British Indian O. Terr.";

      $this->m_country["BN"]["nl"] ="Brunei";
      $this->m_country["BN"]["de"] ="Brunei Darussalam";
      $this->m_country["BN"]["en"] ="Brunei Darussalam";

      $this->m_country["BG"]["nl"] ="Bulgarije";
      $this->m_country["BG"]["de"] ="Bulgaria";
      $this->m_country["BG"]["en"] ="Bulgaria";

      $this->m_country["BF"]["nl"] ="Burkina Faso";
      $this->m_country["BF"]["de"] ="Burkina Faso";
      $this->m_country["BF"]["en"] ="Burkina Faso";

      $this->m_country["BI"]["nl"] ="Burundi";
      $this->m_country["BI"]["de"] ="Burundi";
      $this->m_country["BI"]["en"] ="Burundi";

      $this->m_country["BT"]["nl"] ="Buthan";
      $this->m_country["BT"]["de"] ="Buthan";
      $this->m_country["BT"]["en"] ="Buthan";

      $this->m_country["KH"]["nl"] ="Cambodja";
      $this->m_country["KH"]["de"] ="Cambodia";
      $this->m_country["KH"]["en"] ="Cambodia";

      $this->m_country["CM"]["nl"] ="Kameroen";
      $this->m_country["CM"]["de"] ="Cameroon";
      $this->m_country["CM"]["en"] ="Cameroon";

      $this->m_country["CA"]["nl"] ="Canada";
      $this->m_country["CA"]["de"] ="Canada";
      $this->m_country["CA"]["en"] ="Canada";

      $this->m_country["CV"]["nl"] = html_entity_decode("Kaapverdi&euml;", null, atktext('charset'));
      $this->m_country["CV"]["de"] ="Cape Verde";
      $this->m_country["CV"]["en"] ="Cape Verde";

      $this->m_country["KY"]["nl"] ="Kaaimaneilanden";
      $this->m_country["KY"]["de"] ="Cayman Islands";
      $this->m_country["KY"]["en"] ="Cayman Islands";

      $this->m_country["CF"]["nl"] ="Centraal-Afrikaanse Republiek";
      $this->m_country["CF"]["de"] ="Central African Rep.";
      $this->m_country["CF"]["en"] ="Central African Rep.";

      $this->m_country["TD"]["nl"] ="Tsjaad";
      $this->m_country["TD"]["de"] ="Chad";
      $this->m_country["TD"]["en"] ="Chad";

      $this->m_country["CL"]["nl"] ="Chili";
      $this->m_country["CL"]["de"] ="Chile";
      $this->m_country["CL"]["en"] ="Chile";

      $this->m_country["CN"]["nl"] ="China";
      $this->m_country["CN"]["de"] ="China";
      $this->m_country["CN"]["en"] ="China";

      $this->m_country["CX"]["nl"] ="Christmaseiland";
      $this->m_country["CX"]["de"] ="Christmas Island";
      $this->m_country["CX"]["en"] ="Christmas Island";

      $this->m_country["CC"]["nl"] ="Cocoseilanden";
      $this->m_country["CC"]["de"] ="Cocos (Keeling) Isl.";
      $this->m_country["CC"]["en"] ="Cocos (Keeling) Isl.";

      $this->m_country["CO"]["nl"] ="Colombia";
      $this->m_country["CO"]["de"] ="Colombia";
      $this->m_country["CO"]["en"] ="Colombia";

      $this->m_country["KM"]["nl"] ="Comoren";
      $this->m_country["KM"]["de"] ="Comoros";
      $this->m_country["KM"]["en"] ="Comoros";

      $this->m_country["CG"]["nl"] ="Congo-Brazzaville";
      $this->m_country["CG"]["de"] ="Congo";
      $this->m_country["CG"]["en"] ="Congo";

      $this->m_country["CK"]["nl"] ="Cookeilanden";
      $this->m_country["CK"]["de"] ="Cook Islands";
      $this->m_country["CK"]["en"] ="Cook Islands";

      $this->m_country["CR"]["nl"] ="Costa Rica";
      $this->m_country["CR"]["de"] ="Costa Rica";
      $this->m_country["CR"]["en"] ="Costa Rica";

      $this->m_country["HR"]["nl"] = html_entity_decode("Kroati&euml;", null, atktext('charset'));
      $this->m_country["HR"]["de"] ="Croatia";
      $this->m_country["HR"]["en"] ="Croatia";

      $this->m_country["CU"]["nl"] ="Cuba";
      $this->m_country["CU"]["de"] ="Cuba";
      $this->m_country["CU"]["en"] ="Cuba";

      $this->m_country["CY"]["nl"] ="Cyprus";
      $this->m_country["CY"]["de"] ="Cyprus";
      $this->m_country["CY"]["en"] ="Cyprus";

      $this->m_country["CZ"]["nl"] =html_entity_decode("Tsjechi&euml;", null, atktext('charset'));
      $this->m_country["CZ"]["de"] ="Czech Republic";
      $this->m_country["CZ"]["en"] ="Czech Republic";

      $this->m_country["CS"]["nl"] ="Tsjechoslowakije";
      $this->m_country["CS"]["de"] ="Czechoslovakia";
      $this->m_country["CS"]["en"] ="Czechoslovakia";

      $this->m_country["DK"]["nl"] ="Denmarken";
      $this->m_country["DK"]["de"] ="Denmark";
      $this->m_country["DK"]["en"] ="Denmark";

      $this->m_country["DE"]["nl"] ="Duitsland";
      $this->m_country["DE"]["de"] ="Deutschland";
      $this->m_country["DE"]["en"] ="Germany";

      $this->m_country["DJ"]["nl"] ="Djibouti";
      $this->m_country["DJ"]["de"] ="Djibouti";
      $this->m_country["DJ"]["en"] ="Djibouti";

      $this->m_country["DM"]["nl"] ="Dominica";
      $this->m_country["DM"]["de"] ="Dominica";
      $this->m_country["DM"]["en"] ="Dominica";

      $this->m_country["DO"]["nl"] ="Dominicaanse Republiek";
      $this->m_country["DO"]["de"] ="Dominican Republic";
      $this->m_country["DO"]["en"] ="Dominican Republic";

      $this->m_country["TP"]["nl"] ="Oost Timor";
      $this->m_country["TP"]["de"] ="East Timor";
      $this->m_country["TP"]["en"] ="East Timor";

      $this->m_country["EC"]["nl"] ="Ecuador";
      $this->m_country["EC"]["de"] ="Ecuador";
      $this->m_country["EC"]["en"] ="Ecuador";

      $this->m_country["EG"]["nl"] ="Egypte";
      $this->m_country["EG"]["de"] ="Egypt";
      $this->m_country["EG"]["en"] ="Egypt";

      $this->m_country["SV"]["nl"] ="El Salvador";
      $this->m_country["SV"]["de"] ="El Salvador";
      $this->m_country["SV"]["en"] ="El Salvador";

      $this->m_country["GQ"]["nl"] ="Equatoriaal-Guinea";
      $this->m_country["GQ"]["de"] ="Equatorial Guinea";
      $this->m_country["GQ"]["en"] ="Equatorial Guinea";

      $this->m_country["EE"]["nl"] ="Estland";
      $this->m_country["EE"]["de"] ="Estonia";
      $this->m_country["EE"]["en"] ="Estonia";

      $this->m_country["ET"]["nl"] =html_entity_decode("Ethiopi&euml;", null, atktext('charset'));
      $this->m_country["ET"]["de"] ="Ethiopia";
      $this->m_country["ET"]["en"] ="Ethiopia";

      $this->m_country["FK"]["nl"] ="Falklandeilanden";
      $this->m_country["FK"]["de"] ="Falkland Isl.(Malvinas)";
      $this->m_country["FK"]["en"] ="Falkland Isl.(Malvinas)";

      $this->m_country["FO"]["nl"] = html_entity_decode("Faer&ouml;er", null, atktext('charset'));
      $this->m_country["FO"]["de"] ="Faroe Islands";
      $this->m_country["FO"]["en"] ="Faroe Islands";

      $this->m_country["FJ"]["nl"] ="Fiji";
      $this->m_country["FJ"]["de"] ="Fiji";
      $this->m_country["FJ"]["en"] ="Fiji";

      $this->m_country["FI"]["nl"] ="Finland";
      $this->m_country["FI"]["de"] ="Finland";
      $this->m_country["FI"]["en"] ="Finland";

      $this->m_country["FR"]["nl"] ="Frankrijk";
      $this->m_country["FR"]["de"] ="France";
      $this->m_country["FR"]["en"] ="France";

      $this->m_country["FX"]["nl"] ="France (European Ter.)";
      $this->m_country["FX"]["de"] ="France (European Ter.)";
      $this->m_country["FX"]["en"] ="France (European Ter.)";

      $this->m_country["TF"]["nl"] ="Franse Zuidelijke en Antarctische Gebieden";
      $this->m_country["TF"]["de"] ="French Southern Terr.";
      $this->m_country["TF"]["en"] ="French Southern Terr.";

      $this->m_country["GA"]["nl"] ="Gabon";
      $this->m_country["GA"]["de"] ="Gabon";
      $this->m_country["GA"]["en"] ="Gabon";

      $this->m_country["GM"]["nl"] ="Gambia";
      $this->m_country["GM"]["de"] ="Gambia";
      $this->m_country["GM"]["en"] ="Gambia";

      $this->m_country["GE"]["nl"] =html_entity_decode("Georgi&euml;", null, atktext('charset'));
      $this->m_country["GE"]["de"] ="Georgia";
      $this->m_country["GE"]["en"] ="Georgia";

      $this->m_country["GH"]["nl"] ="Ghana";
      $this->m_country["GH"]["de"] ="Ghana";
      $this->m_country["GH"]["en"] ="Ghana";

      $this->m_country["GI"]["nl"] ="Gibraltar";
      $this->m_country["GI"]["de"] ="Gibraltar";
      $this->m_country["GI"]["en"] ="Gibraltar";

      $this->m_country["GB"]["nl"] ="Verenigd Koninkrijk";
      $this->m_country["GB"]["de"] ="Great Britain (UK)";
      $this->m_country["GB"]["en"] ="Great Britain (UK)";

      $this->m_country["GR"]["nl"] ="Griekenland";
      $this->m_country["GR"]["de"] ="Greece";
      $this->m_country["GR"]["en"] ="Greece";

      $this->m_country["GL"]["nl"] ="Groenland";
      $this->m_country["GL"]["de"] ="Greenland";
      $this->m_country["GL"]["en"] ="Greenland";

      $this->m_country["GD"]["nl"] ="Grenada";
      $this->m_country["GD"]["de"] ="Grenada";
      $this->m_country["GD"]["en"] ="Grenada";

      $this->m_country["GP"]["nl"] ="Guadeloupe (Fr.)";
      $this->m_country["GP"]["de"] ="Guadeloupe (Fr.)";
      $this->m_country["GP"]["en"] ="Guadeloupe (Fr.)";

      $this->m_country["GU"]["nl"] ="Guam (US)";
      $this->m_country["GU"]["de"] ="Guam (US)";
      $this->m_country["GU"]["en"] ="Guam (US)";

      $this->m_country["GT"]["nl"] ="Guatemala";
      $this->m_country["GT"]["de"] ="Guatemala";
      $this->m_country["GT"]["en"] ="Guatemala";

      $this->m_country["GN"]["nl"] ="Guinee";
      $this->m_country["GN"]["de"] ="Guinea";
      $this->m_country["GN"]["en"] ="Guinea";

      $this->m_country["GW"]["nl"] ="Guinee-Bissau";
      $this->m_country["GW"]["de"] ="Guinea Bissau";
      $this->m_country["GW"]["en"] ="Guinea Bissau";

      $this->m_country["GY"]["nl"] ="Guyana";
      $this->m_country["GY"]["de"] ="Guyana";
      $this->m_country["GY"]["en"] ="Guyana";

      $this->m_country["GF"]["nl"] ="Guyana (Fr.)";
      $this->m_country["GF"]["de"] ="Guyana (Fr.)";
      $this->m_country["GF"]["en"] ="Guyana (Fr.)";

      $this->m_country["HT"]["nl"] = html_entity_decode("Ha&iuml;ti", null, atktext('charset'));
      $this->m_country["HT"]["de"] ="Haiti";
      $this->m_country["HT"]["en"] ="Haiti";

      $this->m_country["HM"]["nl"] ="Heard en McDonaldeilanden";
      $this->m_country["HM"]["de"] ="Heard & McDonald Isl.";
      $this->m_country["HM"]["en"] ="Heard & McDonald Isl.";

      $this->m_country["HN"]["nl"] ="Honduras";
      $this->m_country["HN"]["de"] ="Honduras";
      $this->m_country["HN"]["en"] ="Honduras";

      $this->m_country["HK"]["nl"] ="Hongkong";
      $this->m_country["HK"]["de"] ="Hong Kong";
      $this->m_country["HK"]["en"] ="Hong Kong";

      $this->m_country["HU"]["nl"] ="Hongarije";
      $this->m_country["HU"]["de"] ="Hungary";
      $this->m_country["HU"]["en"] ="Hungary";

      $this->m_country["IS"]["nl"] ="IJsland";
      $this->m_country["IS"]["de"] ="Iceland";
      $this->m_country["IS"]["en"] ="Iceland";

      $this->m_country["IN"]["nl"] ="India";
      $this->m_country["IN"]["de"] ="India";
      $this->m_country["IN"]["en"] ="India";

      $this->m_country["ID"]["nl"] =html_entity_decode("Indonesi&euml;", null, atktext('charset'));
      $this->m_country["ID"]["de"] ="Indonesia";
      $this->m_country["ID"]["en"] ="Indonesia";

      $this->m_country["IR"]["nl"] ="Iran";
      $this->m_country["IR"]["de"] ="Iran";
      $this->m_country["IR"]["en"] ="Iran";

      $this->m_country["IQ"]["nl"] ="Iraq";
      $this->m_country["IQ"]["de"] ="Iraq";
      $this->m_country["IQ"]["en"] ="Iraq";

      $this->m_country["IE"]["nl"] ="Ierland";
      $this->m_country["IE"]["de"] ="Ireland";
      $this->m_country["IE"]["en"] ="Ireland";

      $this->m_country["IL"]["nl"] = html_entity_decode("Isra&euml;l", null, atktext('charset'));
      $this->m_country["IL"]["de"] ="Israel";
      $this->m_country["IL"]["en"] ="Israel";

      $this->m_country["IT"]["nl"] = html_entity_decode("Itali&euml;", null, atktext('charset'));
      $this->m_country["IT"]["de"] ="Italy";
      $this->m_country["IT"]["en"] ="Italy";

      $this->m_country["CI"]["nl"] ="Ivoorkust";
      $this->m_country["CI"]["de"] ="Ivory Coast";
      $this->m_country["CI"]["en"] ="Ivory Coast";

      $this->m_country["JM"]["nl"] ="Jamaica";
      $this->m_country["JM"]["de"] ="Jamaica";
      $this->m_country["JM"]["en"] ="Jamaica";

      $this->m_country["JP"]["nl"] ="Japan";
      $this->m_country["JP"]["de"] ="Japan";
      $this->m_country["JP"]["en"] ="Japan";

      $this->m_country["JO"]["nl"] = html_entity_decode("Jordani&euml;", null, atktext('charset'));
      $this->m_country["JO"]["de"] ="Jordan";
      $this->m_country["JO"]["en"] ="Jordan";

      $this->m_country["KZ"]["nl"] ="Kazachstan";
      $this->m_country["KZ"]["de"] ="Kazachstan";
      $this->m_country["KZ"]["en"] ="Kazachstan";

      $this->m_country["KE"]["nl"] ="Kenia";
      $this->m_country["KE"]["de"] ="Kenya";
      $this->m_country["KE"]["en"] ="Kenya";

      $this->m_country["KG"]["nl"] = html_entity_decode("Kirgizi&euml;", null, atktext('charset'));
      $this->m_country["KG"]["de"] ="Kirgistan";
      $this->m_country["KG"]["en"] ="Kirgistan";

      $this->m_country["KI"]["nl"] ="Kiribati";
      $this->m_country["KI"]["de"] ="Kiribati";
      $this->m_country["KI"]["en"] ="Kiribati";

      $this->m_country["KP"]["nl"] ="Korea (Noord)";
      $this->m_country["KP"]["de"] ="Korea (North)";
      $this->m_country["KP"]["en"] ="Korea (North)";

      $this->m_country["KR"]["nl"] ="Korea (Zuid)";
      $this->m_country["KR"]["de"] ="Korea (South)";
      $this->m_country["KR"]["en"] ="Korea (South)";

      $this->m_country["KW"]["nl"] ="Kuweit";
      $this->m_country["KW"]["de"] ="Kuwait";
      $this->m_country["KW"]["en"] ="Kuwait";

      $this->m_country["LA"]["nl"] ="Laos";
      $this->m_country["LA"]["de"] ="Laos";
      $this->m_country["LA"]["en"] ="Laos";

      $this->m_country["LV"]["nl"] ="Letland";
      $this->m_country["LV"]["de"] ="Latvia";
      $this->m_country["LV"]["en"] ="Latvia";

      $this->m_country["LB"]["nl"] ="Libanon";
      $this->m_country["LB"]["de"] ="Lebanon";
      $this->m_country["LB"]["en"] ="Lebanon";

      $this->m_country["LS"]["nl"] ="Lesotho";
      $this->m_country["LS"]["de"] ="Lesotho";
      $this->m_country["LS"]["en"] ="Lesotho";

      $this->m_country["LR"]["nl"] ="Liberia";
      $this->m_country["LR"]["de"] ="Liberia";
      $this->m_country["LR"]["en"] ="Liberia";

      $this->m_country["LY"]["nl"] = html_entity_decode("Libi&euml;", null, atktext('charset'));
      $this->m_country["LY"]["de"] ="Libya";
      $this->m_country["LY"]["en"] ="Libya";

      $this->m_country["LI"]["nl"] ="Liechtenstein";
      $this->m_country["LI"]["de"] ="Liechtenstein";
      $this->m_country["LI"]["en"] ="Liechtenstein";

      $this->m_country["LT"]["nl"] ="Litouwen";
      $this->m_country["LT"]["de"] ="Lithuania";
      $this->m_country["LT"]["en"] ="Lithuania";

      $this->m_country["LU"]["nl"] ="Luxemburg";
      $this->m_country["LU"]["de"] ="Luxembourg";
      $this->m_country["LU"]["en"] ="Luxembourg";

      $this->m_country["MO"]["nl"] ="Macau";
      $this->m_country["MO"]["de"] ="Macau";
      $this->m_country["MO"]["en"] ="Macau";

      $this->m_country["MG"]["nl"] ="Madagaskar";
      $this->m_country["MG"]["de"] ="Madagascar";
      $this->m_country["MG"]["en"] ="Madagascar";

      $this->m_country["MW"]["nl"] ="Malawi";
      $this->m_country["MW"]["de"] ="Malawi";
      $this->m_country["MW"]["en"] ="Malawi";

      $this->m_country["MY"]["nl"] =html_entity_decode("Maleisi&euml;", null, atktext('charset'));
      $this->m_country["MY"]["de"] ="Malaysia";
      $this->m_country["MY"]["en"] ="Malaysia";

      $this->m_country["MV"]["nl"] ="Maldiven";
      $this->m_country["MV"]["de"] ="Maldives";
      $this->m_country["MV"]["en"] ="Maldives";

      $this->m_country["ML"]["nl"] ="Mali";
      $this->m_country["ML"]["de"] ="Mali";
      $this->m_country["ML"]["en"] ="Mali";

      $this->m_country["MT"]["nl"] ="Malta";
      $this->m_country["MT"]["de"] ="Malta";
      $this->m_country["MT"]["en"] ="Malta";

      $this->m_country["MH"]["nl"] ="Marshalleilanden";
      $this->m_country["MH"]["de"] ="Marshall Islands";
      $this->m_country["MH"]["en"] ="Marshall Islands";

      $this->m_country["MQ"]["nl"] ="Martinique (Fr.)";
      $this->m_country["MQ"]["de"] ="Martinique (Fr.)";
      $this->m_country["MQ"]["en"] ="Martinique (Fr.)";

      $this->m_country["MR"]["nl"] =html_entity_decode("Mauritani&euml;", null, atktext('charset'));
      $this->m_country["MR"]["de"] ="Mauritania";
      $this->m_country["MR"]["en"] ="Mauritania";

      $this->m_country["MU"]["nl"] ="Mauritius";
      $this->m_country["MU"]["de"] ="Mauritius";
      $this->m_country["MU"]["en"] ="Mauritius";

      $this->m_country["MX"]["nl"] ="Mexico";
      $this->m_country["MX"]["de"] ="Mexico";
      $this->m_country["MX"]["en"] ="Mexico";

      $this->m_country["FM"]["nl"] ="Micronesia";
      $this->m_country["FM"]["de"] ="Micronesia";
      $this->m_country["FM"]["en"] ="Micronesia";

      $this->m_country["MD"]["nl"] =html_entity_decode("Moldavi&euml;", null, atktext('charset'));
      $this->m_country["MD"]["de"] ="Moldavia";
      $this->m_country["MD"]["en"] ="Moldavia";

      $this->m_country["MC"]["nl"] ="Monaco";
      $this->m_country["MC"]["de"] ="Monaco";
      $this->m_country["MC"]["en"] ="Monaco";

      $this->m_country["MN"]["nl"] =html_entity_decode("Mongoli&euml;", null, atktext('charset'));
      $this->m_country["MN"]["de"] ="Mongolia";
      $this->m_country["MN"]["en"] ="Mongolia";

      $this->m_country["MS"]["nl"] ="Montserrat";
      $this->m_country["MS"]["de"] ="Montserrat";
      $this->m_country["MS"]["en"] ="Montserrat";

      $this->m_country["MA"]["nl"] ="Morocco";
      $this->m_country["MA"]["de"] ="Morocco";
      $this->m_country["MA"]["en"] ="Morocco";

      $this->m_country["MZ"]["nl"] ="Mozambique";
      $this->m_country["MZ"]["de"] ="Mozambique";
      $this->m_country["MZ"]["en"] ="Mozambique";

      $this->m_country["MM"]["nl"] ="Myanmar";
      $this->m_country["MM"]["de"] ="Myanmar";
      $this->m_country["MM"]["en"] ="Myanmar";

      $this->m_country["NA"]["nl"] =html_entity_decode("Namibi&euml;", null, atktext('charset'));
      $this->m_country["NA"]["de"] ="Namibia";
      $this->m_country["NA"]["en"] ="Namibia";

      $this->m_country["NR"]["nl"] ="Nauru";
      $this->m_country["NR"]["de"] ="Nauru";
      $this->m_country["NR"]["en"] ="Nauru";

      $this->m_country['NL']['nl'] = "Nederland";
      $this->m_country['NL']['de'] = "Die Niederlande";
      $this->m_country['NL']['en'] = "The Netherlands";

      $this->m_country["AN"]["nl"] ="Nederlandse Antillen";
      $this->m_country["AN"]["de"] ="Nederlandse Antillen";
      $this->m_country["AN"]["en"] ="Nederlandse Antillen";

      $this->m_country["NP"]["nl"] ="Nepal";
      $this->m_country["NP"]["de"] ="Nepal";
      $this->m_country["NP"]["en"] ="Nepal";

      $this->m_country["NC"]["nl"] ="Nieuw-Caledoni&euml; (Fr.)";
      $this->m_country["NC"]["de"] ="New Caledonia (Fr.)";
      $this->m_country["NC"]["en"] ="New Caledonia (Fr.)";

      $this->m_country["NZ"]["nl"] ="Nieuw-Zeeland";
      $this->m_country["NZ"]["de"] ="New Zealand";
      $this->m_country["NZ"]["en"] ="New Zealand";

      $this->m_country["NI"]["nl"] ="Nicaragua";
      $this->m_country["NI"]["de"] ="Nicaragua";
      $this->m_country["NI"]["en"] ="Nicaragua";

      $this->m_country["NE"]["nl"] ="Niger";
      $this->m_country["NE"]["de"] ="Niger";
      $this->m_country["NE"]["en"] ="Niger";

      $this->m_country["NG"]["nl"] ="Nigeria";
      $this->m_country["NG"]["de"] ="Nigeria";
      $this->m_country["NG"]["en"] ="Nigeria";

      $this->m_country["NU"]["nl"] ="Niue";
      $this->m_country["NU"]["de"] ="Niue";
      $this->m_country["NU"]["en"] ="Niue";

      $this->m_country["NF"]["nl"] ="Norfolk";
      $this->m_country["NF"]["de"] ="Norfolk Island";
      $this->m_country["NF"]["en"] ="Norfolk Island";

      $this->m_country["MP"]["nl"] ="Noordelijke Marianen";
      $this->m_country["MP"]["de"] ="Northern Mariana Isl.";
      $this->m_country["MP"]["en"] ="Northern Mariana Isl.";

      $this->m_country["NO"]["nl"] ="Noorwegen";
      $this->m_country["NO"]["de"] ="Norway";
      $this->m_country["NO"]["en"] ="Norway";

      $this->m_country["OM"]["nl"] ="Oman";
      $this->m_country["OM"]["de"] ="Oman";
      $this->m_country["OM"]["en"] ="Oman";

      $this->m_country["PK"]["nl"] ="Pakistan";
      $this->m_country["PK"]["de"] ="Pakistan";
      $this->m_country["PK"]["en"] ="Pakistan";

      $this->m_country["PW"]["nl"] ="Palau";
      $this->m_country["PW"]["de"] ="Palau";
      $this->m_country["PW"]["en"] ="Palau";

      $this->m_country["PA"]["nl"] ="Panama";
      $this->m_country["PA"]["de"] ="Panama";
      $this->m_country["PA"]["en"] ="Panama";

      $this->m_country["PG"]["nl"] ="Papoea-Nieuw-Guinea";
      $this->m_country["PG"]["de"] ="Papua New Guinea";
      $this->m_country["PG"]["en"] ="Papua New Guinea";

      $this->m_country["PY"]["nl"] ="Paraguay";
      $this->m_country["PY"]["de"] ="Paraguay";
      $this->m_country["PY"]["en"] ="Paraguay";

      $this->m_country["PE"]["nl"] ="Peru";
      $this->m_country["PE"]["de"] ="Peru";
      $this->m_country["PE"]["en"] ="Peru";

      $this->m_country["PH"]["nl"] ="Filipijnen";
      $this->m_country["PH"]["de"] ="Philippines";
      $this->m_country["PH"]["en"] ="Philippines";

      $this->m_country["PN"]["nl"] ="Pitcairneilanden";
      $this->m_country["PN"]["de"] ="Pitcairn";
      $this->m_country["PN"]["en"] ="Pitcairn";

      $this->m_country["PL"]["nl"] ="Polen";
      $this->m_country["PL"]["de"] ="Poland";
      $this->m_country["PL"]["en"] ="Poland";

      $this->m_country["PF"]["nl"] =html_entity_decode("Frans-Polynesi&euml;", null, atktext('charset'));
      $this->m_country["PF"]["de"] ="Polynesia (Fr.)";
      $this->m_country["PF"]["en"] ="Polynesia (Fr.)";

      $this->m_country["PT"]["nl"] ="Portugal";
      $this->m_country["PT"]["de"] ="Portugal";
      $this->m_country["PT"]["en"] ="Portugal";

      $this->m_country["PR"]["nl"] ="Puerto Rico";
      $this->m_country["PR"]["de"] ="Puerto Rico (US)";
      $this->m_country["PR"]["en"] ="Puerto Rico (US)";

      $this->m_country["QA"]["nl"] ="Qatar";
      $this->m_country["QA"]["de"] ="Qatar";
      $this->m_country["QA"]["en"] ="Qatar";

      $this->m_country["RE"]["nl"] ="Reunion";
      $this->m_country["RE"]["de"] ="Reunion (Fr.)";
      $this->m_country["RE"]["en"] ="Reunion (Fr.)";

      $this->m_country["RO"]["nl"] = html_entity_decode("Roemeni&euml;", null, atktext('charset'));
      $this->m_country["RO"]["de"] ="Romania";
      $this->m_country["RO"]["en"] ="Romania";

      $this->m_country["RU"]["nl"] ="Rusland";
      $this->m_country["RU"]["de"] ="Russian Federation";
      $this->m_country["RU"]["en"] ="Russian Federation";

      $this->m_country["RW"]["nl"] ="Rwanda";
      $this->m_country["RW"]["de"] ="Rwanda";
      $this->m_country["RW"]["en"] ="Rwanda";

      $this->m_country["LC"]["nl"] ="Saint Lucia";
      $this->m_country["LC"]["de"] ="Saint Lucia";
      $this->m_country["LC"]["en"] ="Saint Lucia";

      $this->m_country["WS"]["nl"] ="Samoa";
      $this->m_country["WS"]["de"] ="Samoa";
      $this->m_country["WS"]["en"] ="Samoa";

      $this->m_country["SM"]["nl"] ="San Marino";
      $this->m_country["SM"]["de"] ="San Marino";
      $this->m_country["SM"]["en"] ="San Marino";

      $this->m_country["SA"]["nl"] = html_entity_decode("Saoedi-Arabi&euml;", null, atktext('charset'));
      $this->m_country["SA"]["de"] ="Saudi Arabia";
      $this->m_country["SA"]["en"] ="Saudi Arabia";

      $this->m_country["SN"]["nl"] ="Senegal";
      $this->m_country["SN"]["de"] ="Senegal";
      $this->m_country["SN"]["en"] ="Senegal";

      $this->m_country["SC"]["nl"] ="Seychellen";
      $this->m_country["SC"]["de"] ="Seychelles";
      $this->m_country["SC"]["en"] ="Seychelles";

      $this->m_country["SL"]["nl"] ="Sierra Leone";
      $this->m_country["SL"]["de"] ="Sierra Leone";
      $this->m_country["SL"]["en"] ="Sierra Leone";

      $this->m_country["SG"]["nl"] ="Singapore";
      $this->m_country["SG"]["de"] ="Singapore";
      $this->m_country["SG"]["en"] ="Singapore";

      $this->m_country["SK"]["nl"] =html_entity_decode("Slowakij&euml;", null, atktext('charset'));
      $this->m_country["SK"]["de"] ="Slovak Republic";
      $this->m_country["SK"]["en"] ="Slovak Republic";

      $this->m_country["SI"]["nl"] ="Sloveni&euml;";
      $this->m_country["SI"]["de"] ="Slovenia";
      $this->m_country["SI"]["en"] ="Slovenia";

      $this->m_country["SB"]["nl"] ="Solomon Islands";
      $this->m_country["SB"]["de"] ="Solomon Islands";
      $this->m_country["SB"]["en"] ="Solomon Islands";

      $this->m_country["SO"]["nl"] = html_entity_decode("Somali&euml;", null, atktext('charset'));
      $this->m_country["SO"]["de"] ="Somalia";
      $this->m_country["SO"]["en"] ="Somalia";

      $this->m_country["ZA"]["nl"] ="Zuid Afrika";
      $this->m_country["ZA"]["de"] ="South Africa";
      $this->m_country["ZA"]["en"] ="South Africa";

      $this->m_country["ES"]["nl"] ="Spanje";
      $this->m_country["ES"]["de"] ="Spain";
      $this->m_country["ES"]["en"] ="Spain";

      $this->m_country["LK"]["nl"] ="Sri Lanka";
      $this->m_country["LK"]["de"] ="Sri Lanka";
      $this->m_country["LK"]["en"] ="Sri Lanka";

      $this->m_country["SH"]["nl"] ="Sint-Helena";
      $this->m_country["SH"]["de"] ="St. Helena";
      $this->m_country["SH"]["en"] ="St. Helena";

      $this->m_country["PM"]["nl"] ="Saint-Pierre en Miquelon";
      $this->m_country["PM"]["de"] ="St. Pierre & Miquelon";
      $this->m_country["PM"]["en"] ="St. Pierre & Miquelon";

      $this->m_country["ST"]["nl"] ="Sao Tome en Principe";
      $this->m_country["ST"]["de"] ="St. Tome and Principe";
      $this->m_country["ST"]["en"] ="St. Tome and Principe";

      $this->m_country["KN"]["nl"] ="Saint Kitts en Nevis";
      $this->m_country["KN"]["de"] ="St.Kitts Nevis Anguilla";
      $this->m_country["KN"]["en"] ="St.Kitts Nevis Anguilla";

      $this->m_country["VC"]["nl"] ="Saint Vincent en de Grenadines";
      $this->m_country["VC"]["de"] ="St.Vincent & Grenadines";
      $this->m_country["VC"]["en"] ="St.Vincent & Grenadines";

      $this->m_country["SD"]["nl"] ="Soedan";
      $this->m_country["SD"]["de"] ="Sudan";
      $this->m_country["SD"]["en"] ="Sudan";

      $this->m_country["SR"]["nl"] ="Suriname";
      $this->m_country["SR"]["de"] ="Suriname";
      $this->m_country["SR"]["en"] ="Suriname";

      $this->m_country["SJ"]["nl"] ="Spitsbergen en Jan Mayen";
      $this->m_country["SJ"]["de"] ="Svalbard & Jan Mayen Is";
      $this->m_country["SJ"]["en"] ="Svalbard & Jan Mayen Is";

      $this->m_country["SZ"]["nl"] ="Swaziland";
      $this->m_country["SZ"]["de"] ="Swaziland";
      $this->m_country["SZ"]["en"] ="Swaziland";

      $this->m_country["SE"]["nl"] ="Zweden";
      $this->m_country["SE"]["de"] ="Sweden";
      $this->m_country["SE"]["en"] ="Sweden";

      $this->m_country["CH"]["nl"] ="Zwitserland";
      $this->m_country["CH"]["de"] ="Switzerland";
      $this->m_country["CH"]["en"] ="Switzerland";

      $this->m_country["SY"]["nl"] =html_entity_decode("Syri&euml;", null, atktext('charset'));
      $this->m_country["SY"]["de"] ="Syria";
      $this->m_country["SY"]["en"] ="Syria";

      $this->m_country["TJ"]["nl"] ="Tadzjikistan";
      $this->m_country["TJ"]["de"] ="Tadjikistan";
      $this->m_country["TJ"]["en"] ="Tadjikistan";

      $this->m_country["TW"]["nl"] ="Taiwan";
      $this->m_country["TW"]["de"] ="Taiwan";
      $this->m_country["TW"]["en"] ="Taiwan";

      $this->m_country["TZ"]["nl"] ="Tanzania";
      $this->m_country["TZ"]["de"] ="Tanzania";
      $this->m_country["TZ"]["en"] ="Tanzania";

      $this->m_country["TH"]["nl"] ="Thailand";
      $this->m_country["TH"]["de"] ="Thailand";
      $this->m_country["TH"]["en"] ="Thailand";

      $this->m_country["TG"]["nl"] ="Togo";
      $this->m_country["TG"]["de"] ="Togo";
      $this->m_country["TG"]["en"] ="Togo";

      $this->m_country["TK"]["nl"] ="Tokelau-eilanden";
      $this->m_country["TK"]["de"] ="Tokelau";
      $this->m_country["TK"]["en"] ="Tokelau";

      $this->m_country["TO"]["nl"] ="Tonga";
      $this->m_country["TO"]["de"] ="Tonga";
      $this->m_country["TO"]["en"] ="Tonga";

      $this->m_country["TT"]["nl"] ="Trinidad en Tobago";
      $this->m_country["TT"]["de"] ="Trinidad & Tobago";
      $this->m_country["TT"]["en"] ="Trinidad & Tobago";

      $this->m_country["TN"]["nl"] ="Tunesi&euml;";
      $this->m_country["TN"]["de"] ="Tunisia";
      $this->m_country["TN"]["en"] ="Tunisia";

      $this->m_country["TR"]["nl"] ="Turkije";
      $this->m_country["TR"]["de"] ="Turkey";
      $this->m_country["TR"]["en"] ="Turkey";

      $this->m_country["TM"]["nl"] ="Turkmenistan";
      $this->m_country["TM"]["de"] ="Turkmenistan";
      $this->m_country["TM"]["en"] ="Turkmenistan";

      $this->m_country["TC"]["nl"] ="Turks- en Caicoseilanden";
      $this->m_country["TC"]["de"] ="Turks & Caicos Islands";
      $this->m_country["TC"]["en"] ="Turks & Caicos Islands";

      $this->m_country["TV"]["nl"] ="Tuvalu";
      $this->m_country["TV"]["de"] ="Tuvalu";
      $this->m_country["TV"]["en"] ="Tuvalu";

      $this->m_country["UM"]["nl"] ="Kleine Pacifische eilanden van de Verenigde Staten";
      $this->m_country["UM"]["de"] ="US Minor outlying Isl.";
      $this->m_country["UM"]["en"] ="US Minor outlying Isl.";

      $this->m_country["UG"]["nl"] ="Oeganda";
      $this->m_country["UG"]["de"] ="Uganda";
      $this->m_country["UG"]["en"] ="Uganda";

      $this->m_country["UA"]["nl"] = html_entity_decode("Oekra&iuml;ne", null, atktext('charset'));
      $this->m_country["UA"]["de"] ="Ukraine";
      $this->m_country["UA"]["en"] ="Ukraine";

      $this->m_country["AE"]["nl"] ="Verenigde Arabische Emiraten";
      $this->m_country["AE"]["de"] ="United Arab Emirates";
      $this->m_country["AE"]["en"] ="United Arab Emirates";

      $this->m_country["GB"]["nl"] ="Verenigd Koninkrijk";
      $this->m_country["GB"]["de"] ="United Kingdom";
      $this->m_country["GB"]["en"] ="United Kingdom";

      $this->m_country["US"]["nl"] ="Verenigde Staten";
      $this->m_country["US"]["de"] ="United States";
      $this->m_country["US"]["en"] ="United States";

      $this->m_country["UY"]["nl"] ="Uruguay";
      $this->m_country["UY"]["de"] ="Uruguay";
      $this->m_country["UY"]["en"] ="Uruguay";

      $this->m_country["UZ"]["nl"] ="Oezbekistan";
      $this->m_country["UZ"]["de"] ="Uzbekistan";
      $this->m_country["UZ"]["en"] ="Uzbekistan";

      $this->m_country["VU"]["nl"] ="Vanuatu";
      $this->m_country["VU"]["de"] ="Vanuatu";
      $this->m_country["VU"]["en"] ="Vanuatu";

      $this->m_country["VA"]["nl"] ="Vaticaanstad";
      $this->m_country["VA"]["de"] ="Vatican City State";
      $this->m_country["VA"]["en"] ="Vatican City State";

      $this->m_country["VE"]["nl"] ="Venezuela";
      $this->m_country["VE"]["de"] ="Venezuela";
      $this->m_country["VE"]["en"] ="Venezuela";

      $this->m_country["VN"]["nl"] ="Vietnam";
      $this->m_country["VN"]["de"] ="Vietnam";
      $this->m_country["VN"]["en"] ="Vietnam";

      $this->m_country["VG"]["nl"] ="Britse Maagdeneilanden";
      $this->m_country["VG"]["de"] ="Virgin Islands (British)";
      $this->m_country["VG"]["en"] ="Virgin Islands (British)";

      $this->m_country["VI"]["nl"] ="Amerikaanse Maagdeneilanden";
      $this->m_country["VI"]["de"] ="Virgin Islands (US)";
      $this->m_country["VI"]["en"] ="Virgin Islands (US)";

      $this->m_country["WF"]["nl"] ="Wallis en Futuna";
      $this->m_country["WF"]["de"] ="Wallis & Futuna Islands";
      $this->m_country["WF"]["en"] ="Wallis & Futuna Islands";

      $this->m_country["EH"]["nl"] ="Westelijke Sahara";
      $this->m_country["EH"]["de"] ="Western Sahara";
      $this->m_country["EH"]["en"] ="Western Sahara";

      $this->m_country["YE"]["nl"] ="Yemen";
      $this->m_country["YE"]["de"] ="Yemen";
      $this->m_country["YE"]["en"] ="Yemen";

      $this->m_country["ZR"]["nl"] ="Zaire";
      $this->m_country["ZR"]["de"] ="Zaire";
      $this->m_country["ZR"]["en"] ="Zaire";

      $this->m_country["ZM"]["nl"] ="Zambia";
      $this->m_country["ZM"]["de"] ="Zambia";
      $this->m_country["ZM"]["en"] ="Zambia";

      $this->m_country["ZW"]["nl"] ="Zimbabwe";
      $this->m_country["ZW"]["de"] ="Zimbabwe";
      $this->m_country["ZW"]["en"] ="Zimbabwe";
    }
  }
?>