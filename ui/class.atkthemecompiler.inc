<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage ui
   *
   * @copyright (c)2004-2006 Ibuildings.nl BV
   * @copyright (c)2004-2006 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
  atkimport('atk.ui.atktheme');

  /**
   * Compiles cache for current theme.
   *
   * The compiler scans the theme directory and file structure and builds a
   * compiled file that contains the exact location of every themeable
   * element.
   *
   * If a theme is derived from another theme, the compiled theme contains the
   * sum of the parts, so a single compiled theme file contains every
   * information that ATK needs about the theme.
   *
   * @author Boy Baukema <boy@ibuildings.nl>
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage ui
   *
   */
  class atkThemeCompiler
  {
    /**
     * Compile a theme file for a certain theme.
     *
     * @param String $name The name of the theme to compile.
     */
    function compile($name)
    {
      // Process theme directory structure into data array.
      $data = $this->readStructure($name);

      // Write it to the compiled theme file
      if (count($data))
      {
        if (!file_exists(atkconfig("atktempdir")."themes/"))
        {
          mkdir(atkconfig("atktempdir")."themes/");
        }

        atkimport("atk.utils.atktmpfile");
        $tmpfile = new atkTmpFile("themes/$name.inc");
        $tmpfile->writeAsPhp("theme", $data);
        return true;
      }
      return false;
    }

    /**
     * Parse theme structure.
     *
     * This method parses the themes directory and file structure and
     * converts it to a dataset containing all theme attributes and the
     * exact location of all themable files.
     *
     * This method also takes inheritance into account. If the theme derives
     * from another theme, the info for said theme is included too. This is
     * done recursively so themes can derive from any number of base themes.
     *
     * All themes are implicitly derived from the 'default' theme unless they
     * specify otherwise in their themedef.inc file.
     *
     * @param String $name The name of the theme
     * @param String $location The location of the theme ("atk", "app" or "auto")
     * @return array Theme dData structure
     */
    function readStructure($name, $location="auto")
    {
      $data = array();

      $path = $this->findTheme($name, $location);

      // First parse the themedef file for attributes
      if ($path!="" && file_exists(atkconfig("atkroot").$path."themedef.inc"))
      {
        include(atkconfig("atkroot").$path."themedef.inc");

        if (isset($theme["basetheme"])) // If theme is derived from another theme, use that other theme as basis
        {
          $data = $this->readStructure($theme["basetheme"]);
        }
        else if ($name!="default" || $location!="atk") // no basetheme set, assume 'default' theme as basetheme (but not for default itself)
        {
          $data = $this->readStructure("default", "atk");
        }

        if (isset($theme))
        {
          foreach($theme as $key=>$value) $data["attributes"][$key] = $value;
        }

        // Second scan all files in the theme path
        $this->scanThemePath($path, $data);

        $data["attributes"]["basepath"] = $path;
      }
      return $data;
    }

    /**
     * Find the location on disk of a theme with a certain name.
     *
     * @param String $name Name of the theme
     * @param String $location The location of the theme ("atk", "app" or "auto")
     * @return String The path, relative to $config_atkroot, where the theme is located
     */
    function findTheme($name, $location="auto")
    {
      if ($location!="atk"&&file_exists(atkconfig("atkroot")."themes/$name/themedef.inc")) return "themes/$name/";
      else if ($location!="app"&&file_exists(atkconfig("atkroot")."atk/themes/$name")) return "atk/themes/$name/";
      atkerror("Thene $name not found");
      return "";
    }

    /**
     * Traverse theme path.
     *
     * Traverses the theme path and remembers the physical location of all theme files.
     *
     * @param String $path The path of the theme, relative to atkroot.
     * @param String $data Reference to the data array in which to report the file locations
     */
    function scanThemePath($path, &$data)
    {
      $traverser = &atknew("atk.utils.atkdirectorytraverser");
      $abspath = atkconfig("atkroot").$path;
      $subitems = $traverser->getDirContents($abspath);
      foreach($subitems as $name)
      {
        if (in_array($name, array("images", "styles", "templates"))) // images, styles and templates are compiled the same
        {
          $files = $traverser->getDirContents($abspath.$name);
          foreach($files as $file)
          {
            if (!in_array($file, array(".", "..", "CVS")))
            {
              $data["files"][$name][$file] = $path.$name."/".$file;
            }
          }
        }
        else if ($name=="icons") // New ATK5 style icon theme dirs
        {
          $subs = $traverser->getDirContents($abspath.$name);
          foreach($subs as $type)
          {
            if (!in_array($type, array(".", "..", "CVS")))
            {
              $files = $traverser->getDirContents($abspath.$name."/".$type);
              foreach($files as $file)
              {
                if (!in_array($file, array(".", "..", "CVS")))
                {
                  $data["files"]["icons"][$type][$file] = $path.$name."/".$type."/".$file;
                }
              }
            }
          }
        }
        else if (in_array($name, array("tree_icons", "recordlist_icons", "toolbar_icons"))) // Old ATK5 style icon theme dirs
        {
          $type = substr($name, 0, -6);
          $files = $traverser->getDirContents($abspath.$name);
          foreach($files as $file)
          {
            if (!in_array($file, array(".", "..", "CVS")))
            {
              $data["files"]["icons"][$type][$file] = $path.$name."/".$file;
            }
          }
        }
      }
    }

  }

?>
