<?php

  /**
   * Extension on the atkNode class. Here you will find all 
   * functions for the tree view. If you want to use the treeview, you must define the atkTreeNode 
   * instead of atkNode. Example:
   * class classname extends atkTreeNode
   * {
   *      $this->atkTreeNode("nodeclass",NF_TREE|NF_COPY); 
   *
   * }
   *
   * @author Martin Roest (martin@ibuildings.nl) 
   *         Sandy Pleyte (sandy@achievo.org)
   * @version $Revision$
   *
   * $Id$
   *
   */
  include_once(atkconfig("atkroot")."atk/atktreetools.inc");
  
  define("NF_TREE_NO_ROOT_DELETE", NF_SPECIFIC_1); // No root elements can be deleted
  define("NF_TREE_NO_ROOT_COPY", NF_SPECIFIC_2); // No root elements can be copied
  define("NF_TREE_NO_ROOT_ADD", NF_SPECIFIC_3); // No root elements can be added
  
  global $g_maxlevel;
  $g_maxlevel = 0;
  
  class atkTreeNode extends atkNode
  {  
    var $m_tree = array(); 
    
    /**
     * parent Attribute flag (treeview)
     */
     var $m_parent;    
  
    /**
     * Constructor
     * @Param $name Node name
     * @Param $flags Node flags
     */
    function atkTreeNode($name,$flags=0)
    {
      $this->atkNode($name,$flags);  
    }  

    /**
     * Action "admin" handler method, we override this method because we don't want
     * an add form when the flag NF_TREE_NO_ROOT_ADD. Because the add form is only
     * used to add root elements.
     */
    function action_admin()
    {
      if ($this->hasFlag(NF_TREE_NO_ROOT_ADD)) $this->m_flags|=NF_NO_ADD;
      return $this->m_handler->action_admin();
    }
    
    function buildTree()
    {
      atkdebug("atktreenode::buildtree() ".$this->m_parent);
      $recordset = $this->selectDb($this->m_postvars['atkfilter'],"","",$this->m_listExcludes,"", "admin");

      $t = new tree;
      for($i=0;$i<count($recordset);$i++)
      {              
        //$t->addNode($recordset[$i][$this->m_primaryKey[0]],$this->descriptor($group),$recordset[$i][$this->m_parent][$this->m_primaryKey[0]]);    
        $t->addNode($recordset[$i][$this->m_primaryKey[0]],$recordset[$i],$recordset[$i][$this->m_parent][$this->m_primaryKey[0]]);    
      }      
      return $t;
    }

    /**
     * Admin page displays records and the actions that can be performed on
     * them (edit, delete) in a Treeview
     */
    function adminPage(&$handler)
    {
      global $g_maxlevel, $PHP_SELF;
      
      $this->addStyle("style.css");

      $ui = &$this->getUi();      
      
      $content = "";
                
      $adminHeader = $handler->adminHeader();
      if ($adminHeader!="")
      {
        $content.= $adminHeader."<br><br>";
      }

      atkdebug("Entering treeview page.");
      //$this->addStickyVar($p);
      
      $t = $this->buildTree();          
      
      $this->m_tree[0]["level"]=0; 
      //$this->m_tree[0]["label"]="&nbsp;"; 
	    $this->m_tree[0]["id"]=''; 	    
	    $this->m_tree[0]["isleaf"]=1; 

      $this->treeToArray($t->m_tree);

      $g_maxlevel=$g_maxlevel+2;            

       $width = ($g_maxlevel*16)+600;
      $content.="<table border=\"0\" cellspacing=0 cellpadding=0 cols=".($g_maxlevel+2)." width=".$width.">\n";

      if (!$this->hasFlag(NF_NO_ADD) && $this->hasFlag(NF_ADD_LINK) && $this->allowed("add"))
      {
        $addurl = $PHP_SELF."?atknodetype=".$this->atknodetype()."&atkaction=add&atkfilter=".rawurlencode($this->m_parent.".".$this->m_primaryKey[0]."='0'");
        if ($GLOBALS["txt_link_".getNodeType($this->m_type)."_add"]!="")
        {
          // specific text
          $label = text("link_".getNodeType($this->m_type)."_add");
        }
        else
        {
          // generic text
          $label = text(getNodeType($this->m_type))." ".text("add");
        }
        $content.=href($addurl,$label,SESSION_NESTED).'<br><br>';
      }
      
      $content.= $this->GraphTreeRender();
      $content.= "</table><br>";        
      
      atkdebug("Generating treeview finished!");          
      
      return $ui->renderBox(array("title"=>text('title_'.$this->m_type.'_tree'),
                           "content"=>$content));
            
    }
   
/**
     * Recursive funtion whitch fills an array with all the items of the tree.  
     * DEPRECATED, use treeToArray instead.
     * @param $tree Tree
     * @param $level Level
     */   
    function Fill_tree($tree="", $level=0)
    {
      atkdebug("WARNING: use of deprecated function Fill_tree, use treeToArray instead");
      return $this->treeToArray($tree, $level);
    }
    
    /**
     * Recursive funtion whitch fills an array with all the items of the tree. 
     * @param $tree Tree
     * @param $level Level
     */
    function treeToArray($tree="",$level=0)
    {
      static $s_count = 1;
      global $g_maxlevel, $exp_index;
      while (list($id,$objarr) = each($tree))
      {        
        $this->m_tree[$s_count]["level"]=$level+1; 		    
        // Store extra info in the record, so the recordActions override can make 
        // use of some extra info to determine whether or not to show certain actions.
        if (is_array($objarr->m_label)) $objarr->m_label["subcount"] = count($objarr->m_sub);
        $this->m_tree[$s_count]["label"]=$objarr->m_label; 
        $this->m_tree[$s_count]["img"]=$objarr->m_img;         
		    $this->m_tree[$s_count]["id"]=$objarr->m_id; 
        $exp_index[$objarr->m_id] = $s_count;        		    
		    $this->m_tree[$s_count]["isleaf"]=0; 
		    if ($this->m_tree[$s_count]["level"] > $g_maxlevel) $g_maxlevel=$this->m_tree[$s_count]["level"];           
              
		    $s_count++;
        if (count($objarr->m_sub)>0)  
        {          
          $this->treeToArray($objarr->m_sub, $level+1);
        }                      
      }
      return "";  
    }
    

    /**
     * Recursive funtion which fills an array with all the items of the tree. 
     * @param $tree Tree
     * @param $level Level
     */
    function GraphTreeRender($showactions=true, $expandAll=false, $foldable=true)
    {
      global $g_maxlevel,$g_theme, $exp_index, $PHP_SELF,$config_atkroot;
      if(count($this->m_tree)==1) return "";  
          
      if ($expandAll) $this->m_tree[0]["expand"] = 1;
      if($g_theme["TreeIcon_dir"]!=""&&is_dir($config_atkroot."atk/themes/".$g_theme["Name"]."/".$g_theme["TreeIcon_dir"]))
      {
        $icon_path =$config_atkroot."atk/themes/".$g_theme["Name"]."/".$g_theme["TreeIcon_dir"]."/";
      }
      else
      {
        $icon_path =$config_atkroot."atk/themes/default/tree_icons/";
      }
      $img_expand   = $icon_path."tree_expand.gif"; 
	    $img_collapse = $icon_path."tree_collapse.gif"; 
	    $img_line     = $icon_path."tree_vertline.gif";   
	    $img_split	  = $icon_path."tree_split.gif"; 
	    $img_plus     = $icon_path."tree_split_plus.gif";
	    $img_minus    = $icon_path."tree_split_minus.gif";
	    $img_end      = $icon_path."tree_end.gif";
      $img_end_plus = $icon_path."tree_end_plus.gif";
      $img_end_minus= $icon_path."tree_end_minus.gif";
	    $img_leaf     = $icon_path."tree_leaf.gif"; 
      $img_leaflink = $icon_path."tree_leaf_link.gif"; 
	    $img_spc      = $icon_path."tree_space.gif"; 
      $img_extfile  = $icon_path."tree_extfile.gif"; 
       
      $res="";
      $lastlevel=0;
     // echo $this->m_tree[0]["expand"]."--".$this->m_tree[0]["colapse"];
      if ($this->m_tree[0]["expand"] !=1 && $this->m_tree[0]["colapse"] !=1) // normal operation
      {      
       for ($i=0; $i<count($this->m_tree); $i++) 
	     {    
        if($this->m_tree[$i]["level"]<2)
        {
          if($this->m_tree[$i]["isleaf"]==1&&$this->m_tree[$i]["level"]<1)
          {
     	      $expand[$i]=1; 
	   	      $visible[$i]=1;
          }
          else
          {
     	      $expand[$i]=0; 
	   	      $visible[$i]=1;
          }
        }
        else
        {
     	    $expand[$i]=0; 
	   	    $visible[$i]=0;
        }
	    	$levels[$i]=0; 
	     } 
       if ($this->m_postvars["atktree"]!="") $explevels = explode("|",$this->m_postvars["atktree"]); 
      }
      elseif ($this->m_tree[0]["expand"] == 1) // expand all mode!
      {       
        for ($i=0; $i<count($this->m_tree); $i++) 
	      {
          $expand[$i]=1; 
          $visible[$i]=1;
       	  $levels[$i]=0; 
	      }
        $this->m_tree[0]["expand"]=0; // next time we are back in normal view mode!
      }
      elseif ($this->m_tree[0]["colapse"]== 1) //  colapse all mode!
      {
       for ($i=0; $i<count($this->m_tree); $i++) 
	     {         
        if($this->m_tree[$i]["level"]<2)
         {
          if($this->m_tree[$i]["isleaf"]==1&&$this->m_tree[$i]["level"]<1)
          {
     	      $expand[$i]=1; 
	   	      $visible[$i]=1;
          }
          else
          {
     	      $expand[$i]=0; 
	   	      $visible[$i]=1;
          }
         }
         $levels[$i]=0; 
        
	     }
       $this->m_tree[0]["colapse"]=0; // next time we are back in normal view mode!
       $explevels ="";
      }
      /*********************************************/ 
    	/*  Get Node numbers to expand               */ 
    	/*********************************************/ 
	    $i=0;
	    while($i<count($explevels)) 
	    { 
	    	//$expand[$explevels[$i]]=1; 
        $expand[$exp_index[$explevels[$i]]]=1; 
      
	    	$i++; 
	    } 
      /*********************************************/ 
	    /*  Find last nodes of subtrees              */ 
	    /*********************************************/ 
   
	    $lastlevel=$g_maxlevel; 
    
	    for ($i=count($this->m_tree)-1; $i>=0; $i--) 
	    { 
	    	if ( $this->m_tree[$i]["level"] < $lastlevel ) 
	    	{ 
    			for ($j=$this->m_tree[$i]["level"]+1; $j <= $g_maxlevel; $j++)    
			    {         
			    	$levels[$j]=0; 
			    } 
		    } 
		    if ( $levels[$this->m_tree[$i]["level"]]==0 ) 
		    { 
		    	$levels[$this->m_tree[$i]["level"]]=1;
		    	$this->m_tree[$i]["isleaf"]=1; 
		    } 
		    else 
		    $this->m_tree[$i]["isleaf"]=0; 
		    $lastlevel=$this->m_tree[$i]["level"];   
	    }
      /*********************************************/   
    	/*  Determine visible nodes                  */ 
    	/*********************************************/ 
   
	    $visible[0]=1;   // root is always visible 
    	for ($i=0; $i<count($explevels); $i++) 
    	{ 
    		$n=$exp_index[$explevels[$i]]; 
    		if ( ($visible[$n]==1) && ($expand[$n]==1) )    
    		{ 
    			$j=$n+1; 
    			while ( $this->m_tree[$j]["level"] > $this->m_tree[$n]["level"] ) 
    			{ 
    				if ($this->m_tree[$j]["level"]==$this->m_tree[$n]["level"]+1) $visible[$j]=1;      
    				$j++; 
    			} 
    		} 
    	} 



      for ($i=0; $i<$g_maxlevel; $i++) $levels[$i]=1; 
      
      $res.= "<tr>"; 
      // Make cols for max level
	    for ($i=0; $i<$g_maxlevel; $i++) $res.= "<td width=16>&nbsp;</td>\n"; 
      // Make the last text column
	    $res.= "<td width=300>&nbsp;</td>";
      // Column for the functions
      if ($showactions)
      {
        $res.= "<td width=300>&nbsp;</td>";
      }
      $res.= "</tr>\n"; 
    	$cnt=0; 
      while ($cnt<count($this->m_tree)) 
	    { 
	    	if ($visible[$cnt]) 
		    { 
			    /****************************************/ 
			    /* start new row                        */ 
			    /****************************************/
          if($tree_color==$tree_color1) { $tree_color=$tree_color2; } else { $tree_color=$tree_color1; }
			
          $res.="<tr>"; 
			    /****************************************/ 
			    /* vertical lines from higher levels    */ 
			    /****************************************/ 
			    $i=0; 
			    while ($i<$this->m_tree[$cnt]["level"]-1)  
			    { 
				    if ($levels[$i]==1) 
            {
              $res.= "<td><img src=\"".$img_line."\" border=0></td>\n";
            }
            else
            {
              $res.= "<td><img src=\"".$img_spc."\" border=0></td>\n";
            }
				    $i++; 
			    } 
       
			    /****************************************/ 
			    /* corner at end of subtree or t-split  */ 
			    /****************************************/          
          if ($this->m_tree[$cnt]["isleaf"]==1 && $this->m_tree[$cnt+1]["level"]<$this->m_tree[$cnt]["level"])  
			    { 
			    	if ($cnt!=0) $res.= "<td><img src=\"".$img_end."\" border=0></td>\n"; 
            $levels[$this->m_tree[$cnt]["level"]-1]=0; 
			    }
			    else 
			    {
				    if ($expand[$cnt]==0) 
				    {
					    if ($this->m_tree[$cnt+1]["level"]>$this->m_tree[$cnt]["level"]) 
					    {
						    /****************************************/ 
						    /* Create expand/collapse parameters    */ 
						    /****************************************/ 
						    $i=0; $params="atktree="; 
						    while($i<count($expand)) 
						    { 
							    if ( ($expand[$i]==1) && ($cnt!=$i) || ($expand[$i]==0 && $cnt==$i)) 
							    { 
								    $params=$params.$this->m_tree[$i]["id"];
								    $params=$params."|"; 
							    } 
							    $i++; 
						    } 
                if($this->m_tree[$cnt]["isleaf"]==1)
                {
						      if($cnt!=0) $res.= "<td>".href($PHP_SELF."?atknodetype=".$this->atknodetype()."&atkaction=".$this->m_action."&".$params,"<img src=\"".$img_end_plus."\" border=0>")."</td>\n";
                }
                else
                {
						      if($cnt!=0) $res.= "<td>".href($PHP_SELF."?atknodetype=".$this->atknodetype()."&atkaction=".$this->m_action."&".$params,"<img src=\"".$img_plus."\" border=0>")."</td>\n";
                }
					    }
					    else
					    {
						    $res.="<td><img src=\"".$img_split."\" border=0></td>\n";
					    }
				    }
				    else
			    	{
			    		if ($this->m_tree[$cnt+1]["level"]>$this->m_tree[$cnt]["level"]) 
			    		{
						    /****************************************/ 
						    /* Create expand/collapse parameters    */ 
						    /****************************************/ 
					    	$i=0; $params="atktree="; 
						    while($i<count($expand)) 
						    { 
							    if ( ($expand[$i]==1) && ($cnt!=$i) || ($expand[$i]==0 && $cnt==$i)) 
							    { 
								    $params=$params.$this->m_tree[$i]["id"];
								    $params=$params."|"; 
							    } 
							    $i++; 
						    }                
                if($this->m_tree[$cnt]["isleaf"]==1)
                {                  
                  if($cnt!=0) 
                  {
                    if ($foldable)
                    {                    
                      $res.="<td>".href($PHP_SELF."?atknodetype=".$this->atknodetype()."&atkaction=".$this->m_action."&".$params,"<img src=\"".$img_end_minus."\" border=0>")."</td>\n";
                    }
                    else
                    {                    
                      $res.="<td><img src=\"".$img_end."\" border=0></td>\n";
                    }
                  }
                }
                else
                {                
						      if($cnt!=0) 
                  {
                    if ($foldable)
                    {
                      $res.="<td>".href($PHP_SELF."?atknodetype=".$this->atknodetype()."&atkaction=".$this->m_action."&".$params,"<img src=\"".$img_minus."\" border=0>")."</td>\n";
                    }
                    else
                    {
                      $res.="<td><img src=\"".$img_split."\" border=0></td>\n";
                    }
                  }
                }
					    }
					    else
					    {
					    	$res.="<td><img src=\"".$img_split."\" border=0></td>\n";
					    }
				    }
            if($this->m_tree[$cnt]["isleaf"]==1)
            {
              $levels[$this->m_tree[$cnt]["level"]-1]=0;
            }
            else
            {
				      $levels[$this->m_tree[$cnt]["level"]-1]=1;
            }
			    }  
       
			    /********************************************/ 
			    /* Node (with subtree) or Leaf (no subtree) */ 
			    /********************************************/ 
			    if ($this->m_tree[$cnt+1]["level"]>$this->m_tree[$cnt]["level"]) 
			    { 
    				/****************************************/ 
	    			/* Create expand/collapse parameters    */ 
		    		/****************************************/            
            if ($foldable)            
            {            
			    	  $i=0; $params="atktree="; 
			    	  while($i<count($expand)) 
				      { 
  					    if ( ($expand[$i]==1) && ($cnt!=$i) || ($expand[$i]==0 && $cnt==$i)) 
					      { 
  					    	$params=$params.$this->m_tree[$i]["id"];
					    	  $params=$params."|"; 
					      } 
					      $i++; 
				      } 
				                    
				      if ($expand[$cnt]==0) 
  					    $res.= "<td>".href($PHP_SELF."?".$params,"<img src=\"".$img_expand."\" border=0>")."</td>\n"; 
				      else 
  					    $res.= "<td>".href($PHP_SELF."?".$params,"<img src=\"".$img_collapse."\" border=0>")."</td>\n";          
            }
            else
            {
              $res.= "<td><img src=\"".$img_collapse."\" border=0></td>\n";          
            }
			    } 
			    else 
			    { 
			    	/*************************/ 
			    	/* Tree Leaf             */ 
			    	/*************************/ 
            $img = $img_leaf; // the image is a leaf image by default, but it can be overridden 
                              // by putting img to something else
            if ($this->m_tree[$cnt]["img"]!="") 
            {
              $imgname = $this->m_tree[$cnt]["img"];
              $img = $$imgname;
            }
			    	$res.= "<td><img src=\"".$img."\"></td>\n";          
			    } 
       
			    /****************************************/ 
			    /* output item text                     */ 
			    /****************************************/ 
          // If there's an array inside the 'label' thingee, we have an entire record.
          // Else, it's probably just a textual label.
          if (is_array($this->m_tree[$cnt]["label"]))
          {
            $label = $this->descriptor($this->m_tree[$cnt]["label"]);
          }
          else
          {
            $label = $this->m_tree[$cnt]["label"];
          }
  				$res.= "<td colspan=".($g_maxlevel-$this->m_tree[$cnt]["level"])." nowrap><font size=2>".$label."</font></td>\n"; 
      
    			/****************************************/ 
    			/* end row   with the functions                      */     
    			/****************************************/ 
          if ($showactions)
          {
            $res.='<td nowrap> ';
            $actions = array();
            
            if (!$this->hasFlag(NF_NO_ADD) && !($this->hasFlag(NF_TREE_NO_ROOT_ADD) && $this->m_tree[$cnt]["level"] == 0))
            {
              $actions["add"] = $PHP_SELF."?atknodetype=".$this->atknodetype()."&atkaction=add&atkfilter=".$this->m_parent.".".$this->m_primaryKey[0].rawurlencode("='".$this->m_tree[$cnt]["id"]."'");
            }
            if($cnt>0)
            {            
              if (!$this->hasFlag(NF_NO_EDIT))
              {
                $actions["edit"] = $PHP_SELF."?atknodetype=".$this->atknodetype()."&atkaction=edit&atkselector=".$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt]["id"];
              }
              if(($this->hasFlag(NF_COPY)&&$this->allowed("add")&&!$this->hasflag(NF_TREE_NO_ROOT_COPY))||($this->m_tree[$cnt]["level"]!=1&&$this->hasFlag(NF_COPY)&&$this->allowed("add"))) 
              {    
                $actions["copy"] = $PHP_SELF."?atknodetype=".$this->atknodetype()."&atkaction=copy&atkselector=".$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt]["id"];
              }
              if($this->hasFlag(NF_NO_DELETE)||($this->hasFlag(NF_TREE_NO_ROOT_DELETE)&&$this->m_tree[$cnt]["level"]==1))
              {
                 // Do nothing
              }
              else
              {
                $actions["delete"] = $PHP_SELF."?atknodetype=".$this->atknodetype()."&atkaction=delete&atkselector=".$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt]["id"];
              }
            }
            
            // Look for custom record actions.
            $recordactions = $actions;
            $this->recordActions($this->m_tree[$cnt]["label"], $recordactions,$dummy);    
            
            foreach($recordactions as $name => $url)
            {
              if (!empty($url))
              {
                /* dirty hack */
                $atkencoded = strpos($url, "_1") > 0;
   
                $url = str_replace("%5B","[",     $url);
                $url = str_replace("%5D","]",     $url);
                $url = str_replace("_1"."5B","[", $url);
                $url = str_replace("_1"."5D","]", $url);
  
                if ($atkencoded) $url = str_replace('[pk]', atkurlencode(rawurlencode($this->primaryKey($this->m_tree[$cnt]["label"])), false), $url);
                else $url = str_replace('[pk]', rawurlencode($this->primaryKey($this->m_tree[$cnt]["label"])), $url);
   
                $url = stringparse($url, $this->m_tree[$cnt]["label"], true);              
                      
                $res.=href($url, text($name), SESSION_NESTED)."&nbsp;";
              }
            }
            
           $res.= "</td>";
          }
          $res.="</tr>\n";       
		    } 
	    	$cnt++;     
	    } 
            
      return $res;
    }
    
    /** 
          * Copies a record and the Childs if there are any
          *
          *@param $selector The 'where' clause that indicates which records to select.
          */
    function copyDb($record)
    {   
      $oldparent=$record[$this->m_primaryKey[0]];
      $this->addDb($record, true, "copy");
      if(!empty($this->m_parent)) 
      {
        atkdebug("copyDb - Main Record added");
        $newparent=$record[$this->m_primaryKey[0]];
        atkdebug('CopyDbCopychildren('.$this->m_parent.'='.$oldparent.','.$newparent.')');
        $this->copyChildren($this->m_table.'.'.$this->m_parent.'='.$oldparent, $newparent);         	
      }
      return true;
    }
    
    /**
        * This is a recursive function to copy the children from a parent.
        *
        * @param $selector Selector
        * @param $parent Parent ID
        */
    function copyChildren($selector, $parent="")
    {
      $recordset = $this->selectDb($selector,"","","","","copy");
      
      if(count($recordset)>0)
      {                
        for($i=0;$i<count($recordset);$i++)
        {  
          $recordset[$i][$this->m_parent] = array(""=>"",$this->m_primaryKey[0]=>$parent);                    
          $oldrec = $recordset[$i];
          $this->addDb($recordset[$i], true, "copy");
          
          atkdebug("Child Record added");
          $newparent=$recordset[$i][$this->m_primaryKey[0]];
          atkdebug('CopyChildren('.$this->m_parent.'='.$oldrec[$this->m_primaryKey[0]].','.$newparent.')');
          $this->copyChildren($this->m_table.'.'.$this->m_parent.'='.$oldrec[$this->m_primaryKey[0]], $newparent);
        }
      }
      else
      {
        atkdebug("No records found with Selector: $selector - $parent");
      }
      return "";
    }
    
    /**
         * delete record from the database also the childrecords.
         * todo: instead of delete, set the deleted flag.
         * @param $selector Selector
         */
    function deleteDb($selector)
    {
      global $g_db;
      atkdebug("Retrieve record");
      $recordset = $this->selectDb($selector,"","","","","delete");
      for ($i=0;$i<count($recordset);$i++)
      {
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          if ($p_attrib->hasFlag(AF_CASCADE_DELETE))
          {
            $p_attrib->delete($recordset[$i]);
          }
        }
      }   
      $parent = $recordset[0][$this->m_primaryKey[0]];
      if ($this->m_parent!="")
      {
        atkdebug("Check for child records");
        $children = $this->selectDb($this->m_table.'.'.$this->m_parent.'='.$parent,"","","","","delete");
      
        if(count($children)>0)
        {
          atkdebug('DeleteChildren('.$this->m_table.'.'.$this->m_parent.'='.$parent.','.$parent.')');
          $this->deleteChildren($this->m_table.'.'.$this->m_parent.'='.$parent,$parent);
        }
      }
      
       $query = "DELETE FROM ".$this->m_table." WHERE ".$selector;
       $g_db->query($query);
         
      for ($i=0;$i<count($recordset);$i++)
      {
        $this->postDel($recordset[$i]);
      }
      
      return $recordset;
      // todo: instead of delete, set the deleted flag.
    }
    
    /**
         * Recursive function whitch deletes all the child records of a parent
         *
         * @Param $selector Selector
         * @Param $parent Parent
         */
    function deleteChildren($selector,$parent)
    {
      global $g_db;
      atkdebug("Check for child records of the Child");
      $recordset = $this->selectDb($this->m_table.'.'.$this->m_parent.'='.$parent,"","","","","delete");
      for ($i=0;$i<count($recordset);$i++)
      {
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          if ($p_attrib->hasFlag(AF_CASCADE_DELETE))
          {
            $p_attrib->delete($recordset[$i]);
          }
        }
      }   
      
      if(count($recordset)>0)
      {
        for($i=0;$i<count($recordset);$i++)
        {
          $parent = $recordset[$i][$this->m_primaryKey[0]];
          atkdebug('DeleteChildren('.$this->m_table.'.'.$this->m_parent.'='.$recordset[$i][$this->m_primaryKey[0]].','.$parent.')');
          $this->deleteChildren($this->m_table.'.'.$this->m_parent.'='.$recordset[$i][$this->m_primaryKey[0]],$parent);
        }
      }
      $query = "DELETE FROM ".$this->m_table." WHERE ".$selector;
      
      $g_db->query($query);

      
    }
  }

 
?>
