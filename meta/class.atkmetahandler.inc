<?php
/**
 * The default ATK meta handler.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @version $Revision$
 */
class atkMetaHandler
{
  var $m_object = NULL;
  var $m_method = NULL;

  /**
   * Constructor.
   */
  function atkMetaHandler($handler=NULL)
  {
    if (is_string($handler))
      $this->m_method = $handler;
    else if (is_array($handler))
    {
      $this->m_object = &$handler[0];
      $this->m_method = $handler[1];
    }
  }

  /**
   * Returns an instance of the default meta handler determined
   * using the $config_meta_handler variable or if not set uses itself.
   *
   * @static
   */
  function defaultInstance(&$node)
  {
    $class = atkconfig("meta_handler");
    
    if ($class != null)
    {
      atkimport($class);
      $class = strrchr($class, ".") ? substr(strrchr($class, "."), 1) : $class;
      if ($class != "atkmetahandler")
        return new $class($node);
    }
      
    return new atkMetaHandler(array($node, "meta"));
  }
  
  /**
   * Handle.
   */
  function handle(&$metaPolicy, &$node)
  {
    $object = &$this->m_object;
    $method = $this->m_method;
    
    if ($object != NULL)
      $object->$method($metaPolicy, $node);

    else if ($method != NULL)
      $method($metaPolicy, $node);
  }
}
?>