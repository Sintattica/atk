<?php
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
 * @version $Revision: 6168 $
 * $Id$
 */

header('Content-Type: text/javascript');
?>

function profile_getForm()
{
    if (document.dialogform)
        return document.dialogform;
    else
        return document.entryform;
}

function profile_checkAll(fieldname)
{
    with (profile_getForm())
    {
        for (i = 0; i < elements.length; i++)
        {
            if (elements[i].name.substr(0, fieldname.length) == fieldname)
            {
                elements[i].checked = true;
            }
        }
    }
}

function profile_checkNone(fieldname)
{
    with (profile_getForm())
    {
        for (i = 0; i < elements.length; i++)
        {
            if (elements[i].name.substr(0, fieldname.length) == fieldname)
            {
                elements[i].checked = false;
            }
        }
    }
}

function profile_checkInvert(fieldname)
{
    with (profile_getForm())
    {
        for (i = 0; i < elements.length; i++)
        {
            if (elements[i].name.substr(0, fieldname.length) == fieldname)
            {
                elements[i].checked = !elements[i].checked;
            }
        }
    }
}


function profile_checkAllByValue(fieldname, fieldvalue)
{
    with (profile_getForm())
    {
        for (i = 0; i < elements.length; i++)
        {
            if (elements[i].name.substr(0, fieldname.length) == fieldname && elements[i].value.substr(0, fieldvalue.length) == fieldvalue)
            {
                elements[i].checked = true;
            }
        }
    }
}

function profile_checkNoneByValue(fieldname, fieldvalue)
{
    with (profile_getForm())
    {
        for (i = 0; i < elements.length; i++)
        {
            if (elements[i].name.substr(0, fieldname.length) == fieldname && elements[i].value.substr(0, fieldvalue.length) == fieldvalue)
            {
                elements[i].checked = false;
            }
        }
    }
}

function profile_checkInvertByValue(fieldname, fieldvalue)
{
    with (profile_getForm())
    {
        for (i = 0; i < elements.length; i++)
        {
            if (elements[i].name.substr(0, fieldname.length) == fieldname && elements[i].value.substr(0, fieldvalue.length) == fieldvalue)
            {
                elements[i].checked = !elements[i].checked;
            }
        }
    }
}

function profile_fixExpandImage(divName, atkRoot)
{
    var image = get_object("img_" + divName);
    if (get_object(divName).style.display == 'none')
        image.src = atkRoot + 'atk/images/plus.gif';
    else
        image.src = atkRoot + 'atk/images/minus.gif';
}

function profile_fixDivState(divName)
{
    var divElement = get_object(divName);
    var inputElement = get_object("divstate['" + divName + "']");

    if (divElement.style.display == 'none')
        inputElement.value = 'closed';
    else
        inputElement.value = 'opened';
}

function profile_swapProfileDiv(divName, atkRoot)
{
    toggleDisplay(divName, get_object(divName));
    profile_fixExpandImage(divName, atkRoot);
    profile_fixDivState(divName);
}