<?php
/**
 * PHP fixture loader. Loads fixtures that are plain PHP
 * files. PHP files have access to a "global" variable named
 * $data that should be filled with the fixture data.
 * 
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 */
class atkPHPFixtureLoader extends atkAbstractFixtureLoader 
{
  /**
   * Loads and returns the fixture data from the given file.
   *
   * @param string $path fixture file path
   * @return array fixture data
   */
  function load($path)
  {
    $data = array();
    include_once($path);
    return $data;
  }
}
?>