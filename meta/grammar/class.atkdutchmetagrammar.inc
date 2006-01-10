<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage meta
 *
 * @copyright (c) 2004-2005 Peter C. Verhage
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */
 
/**
 * The basic (dutch) "grammar" rules.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @package atk
 * @subpackage meta
 */
class atkDutchMetaGrammar extends atkMetaGrammar 
{
  /**
   * Returns the list of singular rules.
   *
   * @return list of singular rules
   */
  function getSingularRules()
  {
    return array(
      '/ven$/i' => 'f',    
      '/ia$/i' => 'ium',
      '/onen$/i' => 'oon',
      '/([aoeiu])s$/i' => '\1',
      '/([^aoeiu])en$/i' => '\1'
    );
  }
  
  /**
   * Returns the list of plural rules.
   *
   * @return list of plural rules
   */
  function getPluralRules()
  {
    return array(
      '/f$/i' => 'ven',
      '/ium$/i' => 'ia',
      '/oon$/i' => 'onen',
      '/([aoeiu])$/i' => '\1s',
      '/([^aoeiu])$/i' => '\1en'
    );
  }  
}