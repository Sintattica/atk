<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage relations
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * @internal Include base class.
   */   
  userelation("atkrelation");
  
  /**
   * flag(s) specific for atkManyToOneRelation 
   */
  define("AF_RESTRICTED_DELETE", AF_SPECIFIC_1); // Only allow deletion of master item when there are no child records
  
  /**
   * Implementation of one-to-many relationships.
   * 
   * Can be used to create one to many relations ('1 library has N books').
   * A common term for this type of relation is a master-detail relationship.
   * The detailrecords can be edited inline.  
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage relations
   *
   */
  class atkOneToManyRelation extends atkRelation
  {    
    /**
     * List of keys from the destination node that refer to the master record.
     * @access private
     * @var array 
     */
    var $m_refKey=array();

    /**
     * The maximum number of detail records. If the number of detail records
     * exceeds this maximum, the link for adding new details disappears.
     * @var int
     */
    var $m_maxRecords = 0;
    
    
    /**
     * The load method might build a custom filter. When it does, we might want
     * to use it again in other methods.
     * @var string
     */
    var $m_loadFilter = "";

    /**
     * Default constructor.
     *
     * <b>Example: </b> Suppose a department has many employees. To edit the 
     * list of employees in a department, this relationship can be built like
     * this, in the department node:
     * <code>
     * $this->add(new atkOneToManyRelation("employees", "mymod.employee", "department_id"));
     * </code>
     *
     * @param String $name The unique name of this relation within a node.
     *                     In contrast with moth other attributes, the name
     *                     does not correspond to a database field. (Because
     *                     in one2many relations, the databasefield that
     *                     stores the link, is in the destination node and not
     *                     in the owner node).     
     * @param String $destination The node to which the relationship is made
     *                            (in module.nodename notation).
     * @param mixed $refKey For regular oneToMany relationships, $refKey is 
     *                      name of the referential key in the destination
     *                      node. In the case of multi-foreign key
     *                      relationships, $refKey can be an array of fields.
     * @param int $flags Attribute flags that influence this attributes'
     *                   behavior.     
     */
    function atkOneToManyRelation($name, $destination, $refKey="", $flags=0)
    {
      $this->atkRelation($name, $destination, $flags|AF_HIDE_ADD|AF_HIDE_SEARCH|AF_NO_SORT); 
      // 1toM Relations are NEVER edited when adding a rec. And for now, we 
      // cannot search or sort them.
                                                                                 
      if (is_array($refKey))
      {
        $this->m_refKey = $refKey;
      }
      else
      {
        $this->m_refKey[] = $refKey;
      }
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * The atkOneToManyRelation displays a list of detail records in "view" 
     * mode, in the form of a readonly recordlist. In "list" mode, a plain
     * list of detail record descriptors is displayed.
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list" 
     *                     for displaying in recordlists)
     * @return String HTML String 
     */
    function display($record, $mode="list")
    {      
      $myrecords = $record[$this->fieldName()];
      
      if (!is_array($myrecords))
      {
        $myrecords = $this->load(NULL, $record, "admin");
      }
      
      $result = text("none","","atk"); // default result

      if ($this->createDestination())
      {

        /* record list */
        if (count($myrecords) > 0)
        {
          $result = "";
          /* display in list mode */
          if ($mode == "list")
            for ($i = 0; $i < count($myrecords);$i++)
              $result .= ($i > 0 ? "<br>" : "").$this->m_destInstance->descriptor($myrecords[$i]);

          /* display in view mode */
          else
          {
            if (!empty($this->m_destinationFilter)) $filter = rawurlencode($this->m_destinationFilter);
            if (!$this->m_destInstance->hasFlag(NF_NO_VIEW))
              $actions = array("view" => dispatch_url($this->m_destination, "view", array("atkselector" => "[pk]", "atkfilter" => $filter)));
            else $actions = array();
            $recordlist = &atknew("atk.recordlist.atkrecordlist");
            $result = $recordlist->render($this->m_destInstance, $myrecords, $actions, RL_NO_SORT|RL_NO_SEARCH, $this->m_refKey)."<br>";
          }
        }
      }
      return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value. 
     *
     * The atkOneToManyRelation's edit method returns a recordlist in which 
     * detail records can be removed, added and edited.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record="", $fieldprefix="")
    {
      if ($this->createDestination())
      {
      
        // If someone pressed 'save', no records are loaded from the db.
        // since we don't post all records in this relation like we do with
        // other attributes, we have to load them at 'edit' time.
        // we pass 'admin' as mode, since we show a recordlist, and we need
        // all fields that we also get when we would be on an admin screen.
        $myrecords = $this->load(NULL, $record, "admin");
        $output.=$this->editHeader($record, $myrecords);
     
        if (count($myrecords)!=0)
        {     
          $actions = array();
          $params = array();
          
          if ($this->m_destinationFilter!="")
          {
            $params["atkfilter"]=$this->m_destinationFilter;
          }
                    
          $actions = $this->m_destInstance->defaultActions("relation", $params);   

          $recordlist = &atknew("atk.recordlist.atkrecordlist");          
          $flags = RL_EMBED;
          $flags |= $this->m_destInstance->hasFlag(NF_MRA) ? RL_MRA : 0;
          $flags |= $this->m_destInstance->hasFlag(NF_MRPA) ? RL_MRPA : 0;          
          $output.=$recordlist->render($this->m_destInstance, $myrecords, $actions, $flags, $this->m_refKey, "entryform");
          
        }
        if (($this->m_maxRecords==0||$this->m_maxRecords>count($myrecords))&&!$this->m_destInstance->hasFlag(NF_NO_ADD))
        {
          //$strfilter = $this->m_refKey.".".$this->m_ownerInstance->primaryKeyField()."=".$record[$this->m_ownerInstance->primaryKeyField()];
          //echo $record["atkprimkey"];
          if ($this->destinationHasRelation())
          {
            // we need to set the filter of the record we are going to add. 
            // The referential key must be set to the value of the current 
            // primary key. 
            for ($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
            {             
              $primkeyattr = &$this->m_ownerInstance->m_attribList[$this->m_ownerInstance->m_primaryKey[$i]];            
              
              $filterelems[] = $this->m_refKey[0].".".$this->m_ownerInstance->m_primaryKey[$i]."=".$primkeyattr->value2db($record);
              
            }            
          }
          else
          {
            for ($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
            {
              $filterelems[] = $this->m_refKey[$i]."=".$record[$this->m_ownerInstance->m_primaryKey[$i]];
            }
          }
          $strfilter = implode(" AND ", $filterelems);
          if ($this->m_destinationFilter!="")
          {
            $strfilter.=' AND '.$this->m_destinationFilter;
          }
          $add_url = 'dispatch.php?atknodetype='.$this->m_destination.'&atkaction=add&atkfilter='.rawurlencode($strfilter);
          $key = "link_".$this->fieldName()."_add";
                    
          $label = text($key);
          if ($label==""||strtolower($label)==str_replace('_',' ',strtolower($key)))
          {
            $key = "link_".getNodeType($this->m_destination)."_add";
            $label = text($key);
            if ($label==""||strtolower($label)==str_replace('_',' ',strtolower($key)))
            {
              $key = text(getNodeType($this->m_destination))." ".strtolower(text("add","","atk"));
              $label = text($key);
            }
          } 
          $output.= href($add_url,$label,SESSION_NESTED,true);
        }
        $output.=$this->editFooter($record, $myrecords);
        $output.="<br>";
        $this->_fetchNavigationLinks($output);
        return $output;
      }
      return "";
    }
    
    /**
     * Retrieve header for the recordlist.
     * 
     * The regular atkOneToManyRelation has no implementation for this method, 
     * but it may be overridden in derived classes to add extra information 
     * (text, links, whatever) to the top of the attribute, right before the
     * recordlist. This is similar to the adminHeader() method in atkNode.
     *
     * @param array $record The master record that is being edited.
     * @param array $childrecords The childrecords in this master/detail
     *                            relationship.
     * @return String a String to be added to the header of the recordlist.
     */
    function editHeader($record=NULL, $childrecords=NULL)
    {
      return "";
    }
    
    /**
     * Retrieve footer for the recordlist.
     * 
     * The regular atkOneToManyRelation has no implementation for this method, 
     * but it may be overridden in derived classes to add extra information 
     * (text, links, whatever) to the bottom of the attribute, just after the
     * recordlist. This is similar to the adminFooter() method in atkNode.
     *
     * @param array $record The master record that is being edited.
     * @param array $childrecords The childrecords in this master/detail
     *                            relationship.
     * @return String a String to be added at the bottom of the recordlist.
     */
    function editFooter($record=NULL, $childrecords=NULL)
    {
      return "";
    }

    /**
     * Retrieve detail records from the database.
     *
     * Called by the framework to load the detail records.
     *
     * @param atkDb $db The database used by the node. (Usually equal to 
     *                  $g_db)
     * @param array $record The master record
     * @param String $mode The mode for loading (admin, select, copy, etc)
     * 
     * @return array Recordset containing detailrecords, or NULL if no detail
     *               records are present. Note: when $mode is edit, this
     *               method will always return NULL. This is a framework 
     *               optimization because in edit pages, the records are
     *               loaded on the fly.
     */
    function load($db, $record, $mode="")
    {
      if ($mode=="edit")
      {
        // when editing a record with a onetomanyrelation, we don't
        // need to load anything, since that is done in the edit()
        // function.
        return NULL;
      }
      else
      {
        if ($this->createDestination())
        {
          if (count($this->m_refKey)==0||$this->m_refKey[0]=="") $this->m_refKey[0]=$this->m_owner;
          
          for ($i=0, $_i = count($this->m_refKey); $i<$_i; $i++)
          {
            $primkeyattr = &$this->m_ownerInstance->m_attribList[$this->m_ownerInstance->m_primaryKey[$i]];
            $whereelems[] = $this->m_destInstance->m_table.".".$this->m_refKey[$i]."='".$primkeyattr->value2db($record)."'";
          }          
          
          $where = implode(" AND ", $whereelems);

          if ($this->m_destinationFilter!="")
          {
            $this->m_destInstance->addFilter($this->m_destinationFilter);
          }

          if ($mode=="admin" || $mode=="select" || $mode=="view")
          {
            // we only need fields that are displayed in the descriptor.
            $excludes = $this->m_destInstance->m_listExcludes;
          }
          else
          {
            // we probably need everything
            $excludes = "";
          }
          $recordset = $this->m_destInstance->selectDb($where,"",$this->fetchLimitArray(),$excludes,"",$mode);

          $this->m_loadFilter = $where;
          return $recordset;
        }
      }
      return NULL;
    }
    
    /**
     * Override isEmpty function - in a oneToMany relation we should check if the
     * relation contains any records. When there aren't any, the relation is empty,
     * otherwise it isn't
     *
     * @param	array	&$record The record to check
     * @return bool true if a destination record is present. False if not.
     */
    function isEmpty(&$record)
    {
    	if ($this->hasFlag(AF_OBLIGATORY))
    	{
	    	$loaded = $this->load(null,$record);
	    	return empty($loaded);
    	}
	    else
	    {
	    	return parent::isEmpty($record);
	    }
    }


    /** 
     * The delete method is called by the framework to inform the attribute
     * that the master record is deleted.
     *
     * Note that the framework only calls the method when the
     * AF_CASCADE_DELETE flag is set. When calling this method, all detail
     * records belonging to the master record are deleted.
     *
     * @param array $record The record that is deleted.
     * @return boolean true if cleanup was successful, false otherwise.
     */
    function delete($record)
    {
      $classname = $this->m_destination;
      $cache_id = $this->m_owner.".".$this->m_name;
      $rel = &getNode($classname,$cache_id);
      
      for ($i=0, $_i = count($this->m_refKey); $i<$_i; $i++)
      {
         $primkeyattr = &$this->m_ownerInstance->m_attribList[$this->m_ownerInstance->m_primaryKey[$i]]; 
         $whereelems[] = $rel->m_table.".".$this->m_refKey[$i]."='".$primkeyattr->value2db($record)."'";
      }
      $where = implode(" AND ", $whereelems);

      if ($where!="") // double check, so we never by accident delete the entire db
      {
        return $rel->deleteDb($where);
      }
      return true;
    }

    /**
     * Store detail records in the database.
     *     
     * For onetomanyrelation, this function does not have much use, since it
     * stores records using its 'add link'. Thehe copyDb function however uses
     * this method to clone detail records.
     * If mode is anything other than "copy" this method does not do anything.
     *
     * @param atkDb $db The database used by the node. (Usually equal to 
     *                  $g_db)
     * @param array $record The master record which has the detail records
     *                      embedded.
     * @return boolean true if store was successful, false otherwise. 
     */
    function store($db, $record, $mode)
    {
      if ($mode=="copy") // when copying, mode is "copy".
      {
        $onetomanyrecs = $record[$this->fieldName()];
        if (is_array($onetomanyrecs)&&count($onetomanyrecs)>0)
        {
          if ($this->createDestination())
          {
            for ($i=0;$i<count($onetomanyrecs);$i++)
            {
              // the referential key of the onetomanyrecs could be wrong, if we
              // are called for example from a copy function. So just in case,
              // we reset the correct key.
              if(!$this->destinationHasRelation())
              {
                for ($j=0, $_j = count($this->m_refKey); $j<$_j; $j++)
                {
                  $onetomanyrecs[$i][$this->m_refKey[$j]] = $record[$this->m_ownerInstance->m_primaryKey[$j]];
                }
              }  
              else
              {
                for ($j=0, $_j = count($this->m_refKey); $j<$_j; $j++)
                {
                   $onetomanyrecs[$i][$this->m_refKey[0]][$this->m_ownerInstance->m_primaryKey[$j]] = $record[$this->m_ownerInstance->m_primaryKey[$j]];
                }                              
              }        
              
              if (!$this->m_destInstance->addDb($onetomanyrecs[$i], true, $mode))
              {
                // error
                return false;
              }

            }
          }
        }
      }
      return true;
    }
    
    /**
     * Returns a piece of html code for hiding this attribute in an HTML form.
     *
     * Because the oneToMany has nothing to hide, we override the default
     * hide() implementation with a dummy method.
     *
     * @return String An empty string.
     */
    function hide()
    {    
      //Nothing to hide..
      return "";
    }
    
    /**
     * Returns a piece of html code that can be used to get search terms input
     * from the user.
     *
     * Because oneToManyRelations can not be searched directly from the master
     * node, we override the default search() implementation with a dummy
     * function.
     * @return boolean false
     */
    function search()
    {
      return FALSE;
    }

    /**
     * Determine the type of the foreign key on the other side.
     *
     * On the other side of a oneToManyRelation (in the destination node),
     * there may be a regular atkAttribute for the referential key, or an
     * atkManyToOneRelation pointing back at the source. This method discovers
     * which of the 2 cases we are dealing with.
     * @return boolean True if the foreign key on the other side is a
     *                 relation, false if not.
     */
    function destinationHasRelation()
    {
      if ($this->createDestination())
      {
        // If there's a relation back, it's in the destination node under the name of the first refkey element.
        $attrib = $this->m_destInstance->m_attribList[$this->m_refKey[0]];
        if (is_object($attrib) && strpos(get_class($attrib), "elation")!==false) return true;
      }
      return false;
    }
    
    function deleteAllowed()
    {
    	if ($this->hasFlag(AF_RESTRICTED_DELETE))
    	{
    		// Get the destination node
    		$classname = $this->m_destination;
      	$cache_id = $this->m_owner.".".$this->m_name;
      	$rel = &getNode($classname,$cache_id);
        
      	// Get the current atkselector
      	$where = $this->translateSelector($this->m_ownerInstance->m_postvars['atkselector']);      	

      	if ($where)
      	{
      		$childrecords = $rel->selectDb($where);
        	if (!empty($childrecords)) return atktext("restricted_delete_error");
        }
        else return;
    	}
    	return true;
    }
    
    /**
     * Here we check if the selector is on the owner or on the destination
     * if it's on the destination, we leave it alone.
     * Otherwise we translate it back to the destination.
     *
     * @todo when we translate the selector, we get the last used refKey
     *       but how do we know what is the right one?
     * @param string $selector the selector we have to translate
     * @return string the new selector
     */
    function translateSelector($selector)
    {
      // All standard SQL operators
      $sqloperators = array('=','<>','>','<','>=','<=','BETWEEN','LIKE','IN');
      $this->createDestination();
      
      // Check the filter for every SQL operators
      for ($counter=0;$counter!==-1;$counter++)
      {
        if ($sqloperators[$counter])
        {
          list($key, $value) = explode($sqloperators[$counter], $selector);
          // if the operator is in the filter
          if ($value)
          {
            // check if it's on the destination
            for ($refkeycount=0;$refkeycount<count($this->m_refKey);$refkeycount++)
            {
              $destinationkey = $this->m_destInstance->m_table.".".$this->m_refKey[$refkeycount];
              // if the selector is on the destination, we pass it back
              if ($key ==  $destinationkey || $key == $this->m_refKey[$refkeycount])
              {
                return $selector;
              }
            }
            // otherwise we set it on the destination
            return $destinationkey.$sqloperators[$counter].$value;
          }
        }
        // We never found a value, something is wrong with the filter
        else return "";
      }
    }
    
   /**
    * Retrieves navigation links (prev/pagenumbers/next) and puts it
    * on the top and the bottom of the output string
    *
    * @param string &$output Pointer to the output string
    */
    function _fetchNavigationLinks(&$output)
    {
      global $ATK_VARS;

      // create temporary node and set the right parameters
      $this->createDestination();
      $destNode = &$this->m_destInstance;

      $limitArray = $this->fetchLimitArray();
      $destNode->m_postvars["atklimit"]     = $limitArray["limit"];
      $destNode->m_postvars["atkstartat"]   = $limitArray["offset"];
      $destNode->m_postvars["limitPostfix"] = $this->fieldName();
      $destNode->m_action                   = $ATK_VARS["atkaction"];
      
      // If we build a custom loadFilter (in the load method), use this filter
      if ($this->m_loadFilter) $destfilter=$this->m_loadFilter;
      // If not, a destinationFilter takes precedense over all other filters
      elseif ($this->m_destinationFilter) $destfilter = $this->m_destinationFilter;
      // If we have a selector, we need to make sure the navigator filters on this
      elseif ($ATK_VARS['atkselector'])
      {
        // if the filter is valid on the destination (highly unlikely) we just use it
        if ($this->m_destInstance->validateFilter($ATK_VARS['atkselector'])) 
          $destfilter = $ATK_VARS['atkselector'];
        // if the filter is on the owner, we translate it back to the destination
        else if ($this->m_ownerInstance->validateFilter($ATK_VARS['atkselector']))
        	$destfilter = $this->m_destInstance->validateFilter($this->translateSelector($ATK_VARS['atkselector']));
        // otherwise we just don't set a filter
      }

      // the type of the temporary node has to be the type of the owner
      if ($destNode->m_type == $this->m_destInstance->m_type)
          $destNode->m_type  = $this->m_owner;

      // create a navigator and let it create the navigation links using the temporary node
      $navigator  = &atknew("atk.utils.atknavigator");
      $navigation = $navigator->getNavigationUrls($destNode,"admin", $destfilter);
      $navstr     = $navigator->buildNavigation($destNode,"admin",$navigation, true);

      // add the navigation to the top and bottom of the page
      $output = $navstr . "<br />" . $output . "<br />" . $navstr . "<br /><br />";
  	}
  }
?>
