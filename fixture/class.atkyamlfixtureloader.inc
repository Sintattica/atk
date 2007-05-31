<?php
/**
 * YAML fixture loader. Loads YAML fixtures. In the YAML
 * fixture you can optionally use PHP code to output the fixtures
 * PHP code should be enclosed in PHP tags. Script has full access
 * to all of ATK. Scripts output will be interpreted as YAML.
 * 
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 */
class atkYAMLFixtureLoader extends atkAbstractFixtureLoader 
{
  /**
   * Loads and returns the fixture data from the given file.
   *
   * @param string $path fixture file path
   * @return array fixture data
   */
  function load($path)
  {
    $contents = file_get_contents($path);
    $contents = $this->parse($contents);
  	atkimport('atk.utils.atkyaml');
    $data = atkYAML::load($contents);
    return $data;  
  }
}