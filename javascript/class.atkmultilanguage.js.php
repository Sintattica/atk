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
 * <form action="./" method="get" onSubmit="submitSave(this)">
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
 *
 * $Id$
 * $Log$
 * Revision 1.1  2001/04/23 10:17:12  ivo
 * Initial revision
 *
 * Revision 1.1  2001/02/22 22:42:19  peter
 * initial release of the multilanguage script
 *
 */

  /**
   * Makes sure all the data gets saved before
   * the form gets submitted.
   * @param frm the form object
   */
  function submitSave(frm)
  {
    for (var i = 0; i < frm.elements.length; i++)
    {
      var current = frm.elements["multilanguage_current"].value;
      var element = frm.elements[i];

      if (element.type != "hidden" && element.name.lastIndexOf("[multilanguage]") == element.name.length - "[multilanguage]".length)
      {
        if (frm.elements[element.name.substr(0, element.name.lastIndexOf("[multilanguage]")) + "[" + current + "]"] != null &&
            frm.elements[element.name.substr(0, element.name.lastIndexOf("[multilanguage]")) + "[" + current + "]"].type == "hidden")
        {
          frm.elements[element.name.substr(0, element.name.lastIndexOf("[multilanguage]")) + "[" + current + "]"].value = element.value;
        }
      }
    }
  }

  /**
   * Saves the current data if another language is chosen,
   * and loads the data of the new language into the form fields.
   * @field the change language select box object
   */
  function changeLanguage(field)
  {
    var frm = field.form;
    var current = frm.elements["multilanguage_current"].value;

    for (var i = 0; i < frm.elements.length; i++)
    {
      var element = frm.elements[i];
      if (element.type != "hidden" && element.name.lastIndexOf("[multilanguage]") == element.name.length - "[multilanguage]".length)
      {
        if (frm.elements[element.name.substr(0, element.name.lastIndexOf("[multilanguage]")) + "[" + current + "]"] != null &&
            frm.elements[element.name.substr(0, element.name.lastIndexOf("[multilanguage]")) + "[" + current + "]"].type == "hidden")
        {
          frm.elements[element.name.substr(0, element.name.lastIndexOf("[multilanguage]")) + "[" + current + "]"].value = element.value;
        }
      }
    }

    current = field.options[field.selectedIndex].value;
    frm.elements["multilanguage_current"].value = current;

    for (var i = 0; i < frm.elements.length; i++)
    {
      var element = frm.elements[i];
      if (element.type != "hidden" && element.name.lastIndexOf("[multilanguage]") > 0)
      {
        if (frm.elements[element.name.substr(0, element.name.lastIndexOf("[multilanguage]")) + "[" + current + "]"] != null &&
            frm.elements[element.name.substr(0, element.name.lastIndexOf("[multilanguage]")) + "[" + current + "]"].type == "hidden")
        {
          element.value = frm.elements[element.name.substr(0, element.name.lastIndexOf("[multilanguage]")) + "[" + current + "]"].value;
        }
      }
    }
  }