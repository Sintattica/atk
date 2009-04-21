<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6348 $
   * $Id$
   */     

  /** 
   * Defines used by the frameset utilities.
   */
  define("FRAMESET_HORIZONTAL", 1);
  define("FRAMESET_VERTICAL", 2);
  
  define("FRAME_SCROLL_YES", "yes");
  define("FRAME_SCROLL_NO", "no");
  define("FRAME_SCROLL_AUTO", "auto");

  /**
   * Class for generating framesets.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   *
   */  
  class atkFrameSet extends atkAbstractFrame
  {  
    var $m_border;
    var $m_orientation;    
    var $m_noframes;

    /**
     * Frameset ID
     * @var string
     */
    var $m_id;

    /**
     * Default constructor.
     *
     * @param int $dimension The height or width of this frameset within its
     *                       parent frameset. Whether you are specifying
     *                       height or width depends on the orientation of the
     *                       parent frameset. 
     * @param int $orientation The orientation of child frames within this
     *                         frameset. Possible values: FRAMESET_VERTICAL,
     *                         FRAMESET_HORIZONTAL.
     * @param int $border The width in pixels of the border of the frameset.
     * @param String $noframes The text to display instead of the frameset if
     *                         frames are not supported by the browser. This 
     *                         param needs to be specified only for the
     *                         outmost frameset. It is ignored by inner 
     *                         framesets.
     *                        
     */     
    function atkFrameSet($dimension="*", $orientation=FRAMESET_VERTICAL, $border=0, $noframes='')
    {
      $this->atkAbstractFrame($dimension);
      
      $this->m_orientation = $orientation;
      $this->m_border = $border;      
      $this->m_noframes = $noframes;      
    }
		
    /**
     * Set the id of the frameset
     *
     * @param string $id
     * @return atkFrameSet
     */
		function setId($id)
		{
			$this->m_id = $id;
			return $this;
		}
    
    /**
     * Render the frameset to html
     * 
     * @param boolean $embedded If set to true, the frameset is treated as the
     *                          outmost frameset, causing it to render the
     *                          noframes tag for frameless browsers.
     * @return String HTML fragment containing a frameset.
     */
    function render($embedded=false)
    {
      $res = '<frameset '.($this->m_id!=''?'id="'.$this->m_id.'"':'').' '.($this->m_orientation==FRAMESET_VERTICAL?'rows':'cols').'="'.$this->getDimensions().'" frameborder="'.$this->m_border.'" border="'.$this->m_border.'">';
      foreach($this->m_childs as $abstractFrame)
      {
        $res.= $abstractFrame->render(true);
      }
      if (!$embedded&&$this->m_noframes!="")
      {
        $res.= "<noframes>\n".$this->m_noframes."</noframes>\n";
      }
      $res.= "</frameset>\n";
      return $res;
    }       
    
    /**
     * Returns a dimension specifier ("70,10,*") based on the dimensions of the
     * framesets' childs.
     * @return String
     */
    function getDimensions()
    {
      $dimensions = array();
      foreach($this->m_childs as $abstractFrame)
      {
        $dimensions[] = $abstractFrame->getDimension();
      }
      return implode(", ", $dimensions);
    }
        
  }
  
  /**
   * Class that represents a single frame.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   */  
  class atkFrame extends atkAbstractFrame
  {
    var $m_name;
    var $m_src;
    var $m_scrolling;
    var $m_resize;
    var $m_margin;
    
    /**
     * Default constructor
     *
     * @param int $dimension The height or width of this frame within its
     *                       parent frameset. Whether you are specifying
     *                       height or width depends on the orientation of the
     *                       parent frameset. 
     * @param String $name The name of the frame. 
     * @param String $src The url to load into the frame.
     * @param String $scrolling The scrolling policy for this frame. Possible 
     *                          values: FRAME_SCROLL_YES, FRAME_SCROLL_NO and
     *                          FRAME_SCROLL_AUTO.
     * @param boolean $resize When set to true, the frame is resizable by the
     *                        user. When set to false, it is not.
     * @param int $margin The margin (height and width) of the frame contents.
     *                        
     */
    function atkFrame($dimension, $name, $src, $scrolling=FRAME_SCROLL_NO, $resize=false, $margin=0)
    {
      $this->atkAbstractFrame($dimension);
      $this->m_name = $name;
      $this->m_src = $src;
      $this->m_scrolling = $scrolling;
      $this->m_resize = $resize;
      $this->m_margin = $margin;
    }
    
    /**
     * Render the frame to html     
     * @return String
     */
    function render()
    {
      return '<frame name="'.$this->m_name.'" scrolling="'.$this->m_scrolling.'" '.(!$this->m_resize?"noresize":"").
                       ' src="'.$this->m_src.'" marginwidth="'.$this->m_margin.'" marginheight="'.$this->m_margin.'">'."\n";
    }
        
  }
  
  /**
   * Null frame to optionally hook the topmost frame into. This frame
   * has no html output, just the output of its child. 
   * The rootframeset should contain only one child frameset.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   */  
  class atkRootFrameset extends atkAbstractFrame 
  {  
    /**
     * Render the root frameset.
     * @return String
     */     
    function render()
    {
      $res = "";
      foreach($this->m_childs as $abstractframe)
      {
        $res.=$abstractframe->render(false);
      }
      return $res;
    }
  }
  
  /**
   * Abstract base class for frames and framesets. It defines common 
   * functionality that frames and framesets share.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   */   
  class atkAbstractFrame
  {
    var $m_childs = array();
    var $m_dimension;
    
    /**
     * Default constructor
     * 
     * @param string $dimension
     */
    function atkAbstractFrame($dimension="*")
    {
      $this->m_dimension = $dimension;
    }
    
    /** 
     * Add a child to the frameset.
     * @param atkAbstractFrame $abstractframe The frame or frameset to add.     
     */
    function addChild(&$abstractframe)
    {
      $this->m_childs[] = &$abstractframe;
    }
    
    /**
     * Abstract render method. Derived classes should implement this method.
     * @return String
     */
    function render($embedded=false) { return ""; } 
    
    /**
     * Return the dimension of the frame/frameset, within its parent frameset.
     * @return String
     */
    function getDimension()
    {
      return $this->m_dimension;
    }    
  }

?>