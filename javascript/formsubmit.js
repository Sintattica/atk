  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage javascript
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

function atkSubmit(target)
{
  if(target=='-1') return;
  
  // Set ALL <input name="atkescape"> to target--for some reason
  // there are multiple atkescape inputs on some pages, as it's
  // possible to set the wrong one, which means atksession() in
  // class.atksessionmanager.inc gets a blank atkescape.
  $$('input[name="atkescape"]').each(function (n) { n.value = target; }); 
  
  // call global submit function, which doesn't get called automatically
  // when we call entryform.submit manually.
  globalSubmit(document.entryform);
  document.entryform.submit();
}
