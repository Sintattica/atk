<?php
  /**
   * Multilanguage Form Script
   * -------------------------
   *
   * Description:
   * This file contains two javascript functions which can handle form input in multiple languages.
   * To make use of these functions you have to create hidden form fields for every field in every language,
   * with a name ending with "[shortname for the language]", e.g. "fieldname[nl]". Ofcourse you
   * have to create a normal form field to "get" the user input, you have to name this field "fieldname[multilanguage]".
   * Put also a select box for the languages with option values which will be the shortname for the language(s),
   * e.g."<option value="nl">...</option>. And last but not least you have to add a hidden field named
   * multilanguage_current with the value of the first selected language.
   *
   * Note:
   * You can still use other fields in the form which aren't multilanguage. As long as the names don't
   * end with "[multilanguage]"! Also note that
   *
   * Special thanks to Sandy Pleyte (sandy@ibuildings.nl) and Wim Costan (wim@ibuildings.nl) whom created the original
   * multilanguage form script on which this script is based. In fact this script is a generalization of their work.
   *
   * Example:
   * <form action="./" method="get" onSubmit="mlPreSubmit(this)">
   *   <input type="hidden" name="multilanguage_current" value="en">
   *   <input type="hidden" name="name[en]" value="">
   *   <input type="hidden" name="name[de] value="">
   *   <input type="hidden" name="name[fr] value="">
   *   <input type="hidden" name="name[nl]" value="">
   *   <select onChange="changeLanguage(this)">
   *     <option value="en">English</option>
   *     <option value="de">German</option>
   *     <option value="fr">French</option>
   *     <option value="nl">Dutch</option>
   *   </select>
   *   <input name="name[multilanguage]" type="text">
   *   <input type="submit">
   * </form>
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$
   *
  */
?>

  /**
   * Makes sure all the data gets saved before
   * the form gets submitted.
   * @param frm the form object
   */
  function mlPreSubmit(prefix, frm)
  {    
    var curhid = document.getElementById(prefix+'_current');
    var oldlang = curhid.value;
    
    // Search for all non-hidden formelements that end with 'multilanguage'.
    // The value from these must be transfered to their corresponding hidden element
    // before submit.
    for (var i=0; i<frm.elements.length; i++)
    {
      var element = frm.elements[i];
      
      if (element.name.substr(0,prefix.length)==prefix)
      {
        // This element belongs to us..
        var endpos = element.name.lastIndexOf("[multilanguage]");
        
        if (element.name.substr(endpos)=="[multilanguage]")
        {
          // And this element is a multilanguage hidden dingske.
          var basename = element.name.substr(0, endpos);
          var hiddenCurrentEl = frm.elements[basename+'['+oldlang+']'];
          hiddenCurrentEl.value = element.value;       
        }
      }
    }
    
    return true;
  }

  /**
   * Saves the current data if another language is chosen,
   * and loads the data of the new language into the form fields.
   * @param switchfield the change language select box object
   * @param prefix      
   */
  function changeLanguage(switchfield, prefix, all)
  {
    var frm = switchfield.form;
    
    var curhid = document.getElementById(prefix+'_current');
    var oldlang = curhid.value;      
    var newlang = switchfield.options[switchfield.selectedIndex].value;
    
    if (oldlang!=newlang)
    {

      for (var i = 0; i < frm.elements.length; i++)
      {
        var element = frm.elements[i];
        
        if (element.name.substr(0,prefix.length)==prefix||all)
        {
          // This element belongs to us..
          var endpos = element.name.lastIndexOf("[multilanguage]");
          if (element.name.substr(endpos)=="[multilanguage]")
          {
            //alert(element.name);
            // And this element is a multilanguage hidden dingske.
            // So we must put it's current value in the hidden field that belongs to it
            // And set it's value to the hidden field of the new language.
            var basename = element.name.substr(0, endpos);
            var hiddenCurrentEl = frm.elements[basename+'['+oldlang+']'];
            var hiddenNewEl = frm.elements[basename+'['+newlang+']'];
            hiddenCurrentEl.value = element.value;
            element.value = hiddenNewEl.value;
            //alert('taal: '+element.value);
            
            var label = document.getElementById(basename+'_label');            
            label.innerHTML=str_languages[newlang];
          }     
          else 
          {
            // This might be a switchfield.
            // We need to switch all switchfields if prefix=="*".
            if (all)
            {          
              var endpos = element.name.lastIndexOf("_lgswitch");
         //        alert(element.name);              
              if (element.name.substr(endpos)=="_lgswitch" && element.name!=switchfield.name) // not the current one
              {              
                element.selectedIndex = switchfield.selectedIndex;
                
                // We also need to set the hidden _current element of all switches that we change, to the new language.
                var elcurhid = document.getElementById(element.name.substr(0,endpos)+'_current');
                elcurhid.value = newlang;
              }
            }
          }
        }
        
      }  
      
      
      
      curhid.value = newlang; // remember which language is currently active
   }
                   
    return true;
  }
