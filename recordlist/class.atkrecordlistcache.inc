<?php

/**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage recordlist
   *
   * @copyright (c)2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   */
   
  /**
   * RecordlistCaching class
   *
   * @author Boy Baukema <boy@ibuildings.nl>
   * @package atk
   * @subpackage recordlist
   */
  class atkRecordlistCache
  {
    var $m_cachedir;
    
    var $m_cacheid;
    
    var $m_postvars;
    
    var $m_node;
    
    var $m_cacheidentifiers;
    
    function atkRecordlistCache($node="",$postvars="")
    {
      $this->m_node = $node;
      $this->m_postvars = $postvars;
    }
    
    function setNode($node)
    {
      $this->m_node = $node;
    }
    
    function setPostvars($postvars)
    {
      $this->m_postvars = $postvars;
    }
    
    function getCache()
    {
      $output=false;
      $this->_setCacheId();
      
      if (file_exists($this->m_cacheid) && filesize($this->m_cacheid) && !$this->noCaching())
      {
        atkimport("atk.ui.atktheme");
        $theme = &atkTheme::getInstance();
        $page = &atkPage::getInstance();
        
        $page->register_style($theme->stylePath("recordlist.css"));
        $page->register_script(atkconfig("atkroot")."atk/javascript/formselect.js"); 
        $page->register_script(atkconfig("atkroot")."atk/javascript/recordlist.js");
        
        $page->register_loadscript(file_get_contents($this->m_cacheid."_actionloader"));
        $output = file_get_contents(atkconfig("atkroot").$this->m_cacheid);
      }
      return $output;
    }
    
    function _setCacheId()
    {
      $this->m_cachedir = atkconfig("atktempdir")."rlcache/";
      $identifiers = $this->getIdentifiers();
      $this->m_cacheid  = $this->m_cachedir.implode("_",$identifiers)."_".$this->m_postvars['atkstartat'];

      if (!file_exists($this->m_cachedir) || !is_dir($this->m_cachedir)) mkdir($this->m_cachedir,0700);
    }
    
    function writeCache($output, $actionloader)
    {
      if (!$this->noCaching())
      {
        if (file_exists($this->m_cacheid)) unlink($this->m_cacheid);
        $fp = &fopen($this->m_cacheid,"a+");
        
        if ($fp)
        {
          fwrite($fp,$output);
          fclose($fp);
        }
        else 
        {
          return atkerror("Couldn't open {$this->m_cacheid} for writing!");
        }
        
        $fp = &fopen($this->m_cacheid."_actionloader","a+");
        if ($fp)
        {
          fwrite($fp,$actionloader);
          fclose($fp);
        }
        else 
        {
          return atkerror("Couldn't open {$this->m_cacheid}_actionloader for writing!");
        }
        atkdebug("New cache created for {$this->m_node->m_module}.{$this->m_node->m_type} and written to: $this->m_cacheid");
      }
      return;
    }
    
    function noCaching()
    {
      if ($this->m_postvars['atkorderby'] || ($this->m_postvars['atksearch'] && atk_value_in_array($this->m_postvars['atksearch'])))
        return true;
      else
        return false;
    }
    
    function clearCache()
    {
      atkimport("atk.utils.atkdirectorytraverser");
      $cachedir = atkconfig("atktempdir")."rlcache/";
      $atkdirtrav = new atkDirectoryTraverser();
      
      $identifiers = $this->getIdentifiers();
      
      foreach ($atkdirtrav->_getDirContents($cachedir) as $cachefile)
      {
        $unsignificant=false;
        if (!empty($identifiers))
        {
          foreach ($identifiers as $identifier)
          {
            if (!strstr($cachefile, $identifier))
            { 
              $unsignificant=true;
            }
          }
        }
        if (!in_array($cachefile,array(".","..")) && !$unsignificant)
        {
          unlink($cachedir.$cachefile);
        }
      }
      atkdebug("Cache for {$this->m_node->m_module}.{$this->m_node->m_type} cleared");
    }
    
    function getIdentifiers()
    {
      $identifiers=array();
      $identifiers[] = $this->m_node->atkNodeType()."cache";
      if ($this->m_node->m_cacheidentifiers) 
      {
        $this->_formatIdentifiers($this->m_node->m_cacheidentifiers,$identifiers);
      }
      $this->_formatIdentifiers($this->m_cacheidentifiers, $identifiers);
      return $identifiers;
    }
    
    function _formatIdentifiers($identifiers, &$output)
    {
      foreach ($identifiers as $identifier)
      {
        $output[]="_".$identifier['key'].$identifier['value'];
      }
      return $output;
    }
    
    function addCacheIdentifier($identifier)
    {
      $this->m_cacheidentifiers[] = $identifier;
    }
  }

?>