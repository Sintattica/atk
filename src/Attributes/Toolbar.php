<?php namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Ui\Theme;
use Sintattica\Atk\Ui\Page;

/**
 * The atkToolbar displays a set of buttons that can be used
 * to manipulate text in textboxes (bold, italic, underline).
 *
 * This attribute only works in Internet Explorer 4 and up.
 *
 * The attribute has no database interaction and does not correspond to a
 * database field.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage attributes
 *
 */
class Toolbar extends DummyAttribute
{

    /**
     * Default constructor.
     *
     * @param String $name Name of the attribute (unique within a node)
     * @param int $flags Flags for the attribute.
     */
    function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags | self::AF_HIDE_LIST | self::AF_BLANKLABEL);
    }

    /**
     * Returns a piece of html code that can be used to represent this
     * attribute in an HTML form.
     *
     * @param array $record The record that is currently being edited.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record = array(), $fieldprefix = "")
    {
        $theme = Theme::getInstance();

        $page = Page::getInstance();
        $page->register_script(Config::getGlobal('assets_url') . "javascript/newwindow.js");
        $page->register_script(Config::getGlobal('assets_url') . "javascript/class.atktoolbar.js");
        $res = '<a href="javascript:modifySelection(\'<b>\',\'</b>\');"><img src="' . $theme->iconPath("bold",
                "toolbar") . '" border="0" alt="Vet"></a> ';
        $res .= '<a href="javascript:modifySelection(\'<i>\',\'</i>\');"><img src="' . $theme->iconPath("italic",
                "toolbar") . '" border="0" alt="Schuin"></a> ';
        $res .= '<a href="javascript:modifySelection(\'<u>\',\'</u>\');"><img src="' . $theme->iconPath("underline",
                "toolbar") . '" border="0" alt="Onderstreept"></a>';

        // TODO/FIXME:This is platform specific code and should not be here
        // I think is still needed for older platform version (M1, M2) 
        $res .= '&nbsp;<img src="' . $theme->iconPath("delimiter", "toolbar") . '" border="0">&nbsp;';
        $res .= '<a href="javascript:popupSelection(\'pagesel.php\',\'pagesel\');" onmouseover="selectie=document.selection.createRange();"><img src="' . $theme->iconPath("link",
                "toolbar") . '" border="0" alt="Link"></a>';

        return $res;
    }

}

