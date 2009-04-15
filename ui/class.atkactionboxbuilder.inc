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
 * @copyright (c) 2000-2008 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 5835 $
 * $Id$
 */

/**
 * Action box builder. Provides a fluent interface to create standardized
 * ATK action boxes.
 * 
 * This class is used/exposed by the atkPageBuilder class.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage ui
 * 
 * @see atkPageBuilder
 */
class atkActionBoxBuilder
{
  /**
   * Page builder.
   *
   * @var atkPageBuilder
   */
  protected $m_pageBuilder;
  
  /**
   * Box title.
   *
   * @var string
   */
  protected $m_title = null;
  
  /**
   * Action box parameters.
   *
   * @var array
   */
  protected $m_params = array();
  
  /**
   * Session status.
   *
   * @var int
   */
  protected $m_sessionStatus = SESSION_DEFAULT;
  
  /**
   * Constructor.
   *
   * @param atkPageBuilder $pageBuilder page builder
   */
  public function __construct(atkPageBuilder $pageBuilder)
  {
    $this->m_pageBuilder = $pageBuilder;
    $this->m_params = $pageBuilder->getNode()->getDefaultActionParams(false);
    
    $controller = atkController::getInstance();
    $controller->setNode($pageBuilder->getNode());
    
    $this->formStart('
      <form 
        id="entryform" 
        name="entryform" 
        enctype="multipart/form-data" 
        action="'.$controller->getPhpFile().'?'.SID.'" 
        method="post" 
        onsubmit="return globalSubmit(this)">'.
        $controller->getHiddenVarsString()
     );    
  }

  /**
   * Sets the box title
   * 
   * @param string $title title
   * 
   * @return atkActionBoxBuilder
   */
  public function title($title)
  {
    $this->m_title = $title;
    return $this;
  }
  
  /**
   * Locked?
   *
   * @param boolean $locked locked
   * 
   * @return atkActionBoxBuilder
   */
  public function locked($locked)
  {
    $this->m_params["lockstatus"] = $this->m_pageBuilder->getNode()->getLockStatusIcon($locked);
    return $this;    
  }
  
  /**
   * Set form start.
   * 
   * @param string $formStart form start
   * 
   * @return atkActionBoxBuilder
   */
  public function formStart($formStart)
  {
    $this->m_params['formstart'] = $formStart;
    return $this;
  }
  
  /**
   * Sets the session status.
   * 
   * The default session status is SESSION_DEFAULT. If you don't want an
   * automatically appended session form set the session status 
   * explicitly to null!
   *
   * @param int $status session status
   * 
   * @return atkActionBoxBuilder
   */
  public function sessionStatus($status)
  {
    $this->m_sessionStatus = $status;
    return $this;    
  }
  
  /**
   * Set form end.
   * 
   * @param string $formEnd form end
   * 
   * @return atkActionBoxBuilder
   */
  public function formEnd($formEnd)
  {
    $this->m_params['formend'] = $formEnd;
    return $this;
  }  
  
  /**
   * Set content.
   * 
   * @param string $content content
   * 
   * @return atkActionBoxBuilder
   */
  public function content($content)
  {
    $this->m_params['content'] = $content;
    return $this;
  }
  
  /**
   * Set form buttons.
   * 
   * @param string $buttons form buttons
   * 
   * @return atkActionBoxBuilder
   */
  public function buttons($buttons)
  {
    $this->m_params['buttons'] = $buttons;
    return $this;
  }
  
  /**
   * Stops building the action box and returns the page builder.
   * 
   * @return atkPageBuilder
   */
  public function endActionBox()
  {
    if ($this->m_sessionStatus !== null)
    {
      $this->m_params['formend'] = 
        session_form($this->m_sessionStatus).
        $this->m_params['formend'];
    }
      
    $this->m_pageBuilder->actionBox($this->m_params, $this->m_title);
    
    return $this->m_pageBuilder;
  }
}