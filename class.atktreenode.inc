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
    * @author Martin Roest  (martin@ibuildings.nl) and Sandy Pleyte (sandy@achievo.com)
    *
    * $Id$
    * $Log$
    * Revision 4.7  2001/05/01 13:43:57  ivo
    * New session features: stackVars and globalVars..
    *
    * Revision 4.6  2001/05/01 12:32:54  sandy
    * removed the debug functions and created a new Tree relations
    *
    * Revision 4.5  2001/05/01 09:49:49  ivo
    * Replaced all require() and include() calls by require_once() and
    * include_once() calls. The if(!DEFINED)... inclusion protection in files
    * is now obsolete.
    *
    * Revision 4.4  2001/05/01 09:15:51  ivo
    * Initial session based atk version.
    *
    * Revision 4.3  2001/04/24 09:06:09  sandy
    * first level of the tree always visible, and fixed the NF_TREE flags.
    *
    * Revision 4.2  2001/04/23 13:10:48  sandy
    * small bugfix in the tree class
    *
    * Revision 4.1  2001/04/23 12:32:57  sandy
    * Made the TreeIcons themeable, updated the Themes howto, and removed some debugging code in the atktreenode.
    *
    * Revision 4.0  2001/04/23 10:19:38  ivo
    * Revision number update
    *
    * Revision 1.1.1.1  2001/04/23 10:17:06  ivo
    * Added atk4 to repository.
    *
    */
  include_once("atk/atktreetools.inc");
  
  $g_limit = 10;
  $g_start = 0;
  $g_teller = 0;
  $g_maxlevel = 0;
  
  class atkTreeNode extends atkNode
  {
  
    var $m_tree = array(); 
  
    /**
        * Constructor
        * @Param $name Node name
        * @Param $flags Node flags
        */
    function atkTreeNode($name,$flags=0)
    {
      if($this->hasFlag(NF_TREE_NO_ROOT_ADD))
      {
       //
      }
      $this->atkNode($name,$flags);    
    }  
    
    /**
          * Admin page displays records and the actions that can be performed on
          * them (edit, delete) in a Treeview
          */
    function adminPage()
    {
      global $g_securityManager,$g_layout,$g_maxlevel,$g_teller,$p;

      $g_layout->ui_top(text('title_'.$this->m_type.'_tree'));
      $g_layout->output('<br>');

      $adminHeader = $this->adminHeader();
      if ($adminHeader!="")
      {
        $g_layout->output($adminHeader."<br><br>");
      }

      atkdebug("Entering treeview page.");
      $this->addStickyVar($p);
      
      $this->selectDb($this->m_postvars['atkfilter'],$this->m_primaryKey[0],"",$this->m_listExcludes);

      $t = new tree;
      for($i=0;$i<count($this->m_records);$i++)
      {
        $group=$this->m_records[$i];
        $t->addNode($this->m_records[$i][$this->m_primaryKey[0]],$this->descriptor($group),$this->m_records[$i][$this->m_parent][$this->m_primaryKey[0]]);    
      }
      $this->m_tree[$g_teller][0]=0; 
      $this->m_tree[$g_teller][1]="&nbsp;"; 
	    $this->m_tree[$g_teller][2]=''; 
	    $this->m_tree[$g_teller][3]=''; 
	    $this->m_tree[$g_teller][4]=1; 
      $g_teller++;
      $test = $this->Fill_tree($t->m_tree);
      $g_maxlevel=$g_maxlevel+2;

      $g_layout->output("<table border=\"0\" cellspacing=0 cellpadding=0 cols=".($g_maxlevel+2)." width=".($g_maxlevel*30).">\n");

      if (!$this->hasFlag(NF_NO_ADD) && $this->hasFlag(NF_ADD_LINK) && $this->allowed("add"))
      {
        $addurl = "dispatch.php?atkaction=add&atkfilter=".rawurlencode($this->m_parent.".".$this->m_primaryKey[0]."='0'");
        $g_layout->output(href($addurl,text("clickheretoadd_prefix").text($this->m_type).text("clickheretoadd_postfix"),SESSION_NESTED).'<br><br>');
      }

      $g_layout->output($this->GraphTreeRender());
      
      $g_layout->output("</table>");      
      $g_layout->ui_bottom();
      atkdebug("Generating treeview finished!");
    }
    
    /**
         * Recursive funtion whitch fills an array with all the items of the tree. 
         * @param $tree Tree
         * @param $level Level
         */
    function Fill_tree($tree="",$level=0)
    {
      global $g_teller,$g_maxlevel;
      while (list($id,$objarr) = each($tree))
      {
        $this->m_tree[$g_teller][0]=$level+1; 
		    $this->m_tree[$g_teller][1]=$objarr->m_label; 
		    $this->m_tree[$g_teller][2]=$objarr->m_id; 
		    $this->m_tree[$g_teller][3]=''; 
		    $this->m_tree[$g_teller][4]=0; 
		    if ($this->m_tree[$g_teller][0] > $g_maxlevel) $g_maxlevel=$this->m_tree[$g_teller][0];     
		    $g_teller++;
        if (count($objarr->m_sub)>0)
        {          
          $this->Fill_tree($objarr->m_sub, $level+1);
        }              
      }
      return "";  
    }
    

    /**
         * Recursive funtion which fills an array with all the items of the tree. 
         * @param $tree Tree
         * @param $level Level
         */
    function GraphTreeRender()
    {
      global $g_limit,$g_teller,$g_maxlevel,$g_layout,$p,$g_theme;
      if(is_dir("themes/".$g_theme["Name"]."/".$g_theme["TreeIcon_dir"]))
      {
        $icon_path ="themes/".$g_theme["Name"]."/".$g_theme["TreeIcon_dir"]."/";
      }
      else
      {
        $icon_path ="atk/themes/default/tree_icons/";
      }
      $img_expand   = $icon_path."tree_expand.gif"; 
	    $img_collapse = $icon_path."tree_collapse.gif"; 
	    $img_line     = $icon_path."tree_vertline.gif";   
	    $img_split	  = $icon_path."tree_split.gif"; 
	    $img_plus     = $icon_path."tree_split_plus.gif";
	    $img_minus    = $icon_path."tree_split_minus.gif";
	    $img_end      = $icon_path."tree_end.gif"; 
	    $img_leaf     = $icon_path."tree_leaf.gif"; 
	    $img_spc      = $icon_path."tree_space.gif"; 
  
      $res="";
      $lastlevel=0;
      for ($i=0; $i<count($this->m_tree); $i++) 
	    { 
        if($this->m_tree[$i][0]<2)
        {
          if($this->m_tree[$i][4]==1&&$this->m_tree[$i][0]<1)
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
      /*********************************************/ 
    	/*  Get Node numbers to expand               */ 
    	/*********************************************/ 
   
	    if ($p!="") $explevels = explode("|",$p); 
   
	    $i=0;
	    while($i<count($explevels)) 
	    { 
	    	$expand[$explevels[$i]]=1; 
      
	    	$i++; 
	    } 
      /*********************************************/ 
	    /*  Find last nodes of subtrees              */ 
	    /*********************************************/ 
   
	    $lastlevel=$g_maxlevel; 
    
	    for ($i=count($this->m_tree)-1; $i>=0; $i--) 
	    { 
	    	if ( $this->m_tree[$i][0] < $lastlevel ) 
	    	{ 
    			for ($j=$this->m_tree[$i][0]+1; $j <= $g_maxlevel; $j++)    
			    { 
        
			    	$levels[$j]=0; 
			    } 
		    } 
		    if ( $levels[$this->m_tree[$i][0]]==0 ) 
		    { 
		    	$levels[$this->m_tree[$i][0]]=1;
		    	$this->m_tree[$i][4]=1; 
		    } 
		    else 
		    $this->m_tree[$i][4]=0; 
		    $lastlevel=$this->m_tree[$i][0];   
	    }
      /*********************************************/   
    	/*  Determine visible nodes                  */ 
    	/*********************************************/ 
   
	    $visible[0]=1;   // root is always visible 
    	for ($i=0; $i<count($explevels); $i++) 
    	{ 
    		$n=$explevels[$i]; 
    		if ( ($visible[$n]==1) && ($expand[$n]==1) )    
    		{ 
    			$j=$n+1; 
    			while ( $this->m_tree[$j][0] > $this->m_tree[$n][0] ) 
    			{ 
    				if ($this->m_tree[$j][0]==$this->m_tree[$n][0]+1) $visible[$j]=1;      
    				$j++; 
    			} 
    		} 
    	} 



      for ($i=0; $i<$g_maxlevel; $i++) $levels[$i]=1; 
      
      $res.= "<tr>"; 
      // Make cols for max level
	    for ($i=0; $i<$g_maxlevel; $i++) $res.= "<td width=10>&nbsp;</td>\n"; 
      // Make the last text column
	    $res.= "<td width=300>&nbsp;</td>";
      // Column for the functions
      $res.= "<td width=300>&nbsp;</td>";
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
			    while ($i<$this->m_tree[$cnt][0]-1)  
			    { 
				    if ($levels[$i]==1) 
            {
              $res.= "<td><img src=\"".$img_line."\"></td>\n";
            }
            else
            {
              $res.= "<td><img src=\"".$img_spc."\"></td>\n";
            }
				    $i++; 
			    } 
       
			    /****************************************/ 
			    /* corner at end of subtree or t-split  */ 
			    /****************************************/          
			    if ($this->m_tree[$cnt][4]==1)  
			    { 
			    	if ($cnt!=0) $res.= "<td><img src=\"".$img_end."\"></td>\n"; 
				    $levels[$this->m_tree[$cnt][0]-1]=0; 
			    } 
			    else 
			    {
				    if ($expand[$cnt]==0) 
				    {
					    if ($this->m_tree[$cnt+1][0]>$this->m_tree[$cnt][0]) 
					    {
						    /****************************************/ 
						    /* Create expand/collapse parameters    */ 
						    /****************************************/ 
						    $i=0; $params="p="; 
						    while($i<count($expand)) 
						    { 
							    if ( ($expand[$i]==1) && ($cnt!=$i) || ($expand[$i]==0 && $cnt==$i)) 
							    { 
								    $params=$params.$i;
								    $params=$params."|"; 
							    } 
							    $i++; 
						    } 
						    $res.= "<td>".href("dispatch.php?".$script.$params,"<img src=\"".$img_plus."\" border=no>")."</td>\n"; 
					    }
					    else
					    {
						    $res.="<td><img src=\"".$img_split."\" border=no></td>\n";
					    }
				    }
				    else
			    	{
			    		if ($this->m_tree[$cnt+1][0]>$this->m_tree[$cnt][0]) 
			    		{
						    /****************************************/ 
						    /* Create expand/collapse parameters    */ 
						    /****************************************/ 
					    	$i=0; $params="p="; 
						    while($i<count($expand)) 
						    { 
							    if ( ($expand[$i]==1) && ($cnt!=$i) || ($expand[$i]==0 && $cnt==$i)) 
							    { 
								    $params=$params.$i;
								    $params=$params."|"; 
							    } 
							    $i++; 
						    } 
						    $res.="<td>".href("dispatch.php?".$script.$params,"<img src=\"".$img_minus."\" border=no>")."</td>\n"; 
					    }
					    else
					    {
					    	$res.="<td><img src=\"".$img_split."\" border=no></td>\n";
					    }
				    }
				    $levels[$this->m_tree[$cnt][0]-1]=1;     
			    }  
       
			    /********************************************/ 
			    /* Node (with subtree) or Leaf (no subtree) */ 
			    /********************************************/ 
			    if ($this->m_tree[$cnt+1][0]>$this->m_tree[$cnt][0]) 
			    { 
    				/****************************************/ 
	    			/* Create expand/collapse parameters    */ 
		    		/****************************************/ 
			    	$i=0; $params="p="; 
			    	while($i<count($expand)) 
				    { 
					    if ( ($expand[$i]==1) && ($cnt!=$i) || ($expand[$i]==0 && $cnt==$i)) 
					    { 
					    	$params=$params.$i;
					    	$params=$params."|"; 
					    } 
					    $i++; 
				    } 
				                    
				    if ($expand[$cnt]==0) 
					    $res.= "<td>".href("dispatch.php?".$script.$params,"<img src=\"".$img_expand."\" border=no>")."</td>\n"; 
				    else 
					    $res.= "<td>".href("dispatch.php?".$script.$params,"<img src=\"".$img_collapse."\" border=no>")."</td>\n";          
			    } 
			    else 
			    { 
			    	/*************************/ 
			    	/* Tree Leaf             */ 
			    	/*************************/ 
			    	$res.= "<td><img src=\"".$img_leaf."\"></td>\n";          
			    } 
       
			    /****************************************/ 
			    /* output item text                     */ 
			    /****************************************/ 
  				$res.= "<td colspan=".($g_maxlevel-$this->m_tree[$cnt][0])."><font size=2>".$this->m_tree[$cnt][1]."</font></td>\n"; 
      
    			/****************************************/ 
    			/* end row   with the functions                      */     
    			/****************************************/ 
          $res.='<td> '.href("dispatch.php?atkaction=add&atkfilter=".$this->m_parent.".".$this->m_primaryKey[0].rawurlencode("='".$this->m_tree[$cnt][2]."'"), text("add"), SESSION_NESTED);
          if($cnt>0)
          {
            $res.=' '.href("dispatch.php?atkaction=edit&atkselector=".$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt][2], text("edit"),SESSION_NESTED);
            if(($this->hasFlag(NF_COPY)&&$this->allowed("add")&&!$this->hasflag(NF_TREE_NO_ROOT_COPY))||($this->m_tree[$cnt][0]!=1&&$this->hasFlag(NF_COPY)&&$this->allowed("add"))) 
            {    
              $res.=' '.href("dispatch.php?atkaction=copy&atkselector=".$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt][2], text("copy"));
            }
            if($this->hasFlag(NF_TREE_NO_ROOT_DELETE)&&$this->m_tree[$cnt][0]==1)
            {
               // Do nothing
            }
            else
            {
              $res.=' '.href("dispatch.php?atkaction=delete&atkselector=".$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt][2], text("delete"),SESSION_NESTED);
            }
          }
         $res.= "</td></tr>\n";       
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
    function copyDb($selector)
    {
      $this->selectDb($selector);
      $tmprecs = $this->m_records;
      if(count($tmprecs)>0)
      {      
        $this->addDb();
        atkdebug("copyDb - Main Record added");
        $newparent=$this->m_records[$this->m_currentRec][$this->m_primaryKey[0]];
        atkdebug('CopyDbCopychildren('.$this->m_parent.'='.$tmprecs[0][$this->m_primaryKey[0]].','.$newparent.')');
        $this->copyChildren($this->m_table.'.'.$this->m_parent.'='.$tmprecs[0][$this->m_primaryKey[0]], $newparent);
      }
      else
      {
        atkdebug("No records found with Selector: $selector - $parent");
      }
      return "";
    }
    
    /**
        * This is a recursive function to copy the childeren from a parent.
        *
        * @param $selector Selector
        * @param $parent Parent ID
        */
    function copyChildren($selector, $parent="")
    {
      $this->selectDb($selector);
      $tmprecs = $this->m_records;
      if(count($tmprecs)>0)
      {        
        atkdebug("Loop ".count($tmprecs)." Childrecords - current ".$this->m_currentRec." - New parent id = $parent");
        for($i=0;$i<count($tmprecs);$i++)
        {  
          $tmprecs[$i][$this->m_parent] = array(""=>"",$this->m_primaryKey[0]=>$parent);
          $this->m_currentRec = 0;
          $this->m_records[0] = $tmprecs[$i];
          $this->addDb();
          $newrec = $this->m_records[0];
          atkdebug("Child Record added");
          $newparent=$newrec[$this->m_primaryKey[0]];
          atkdebug('CopyChildren('.$this->m_parent.'='.$tmprecs[$i][$this->m_primaryKey[0]].','.$newparent.')');
          $this->copyChildren($this->m_table.'.'.$this->m_parent.'='.$tmprecs[$i][$this->m_primaryKey[0]], $newparent);
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
      $this->selectDb($selector);
      $tmpparentrecs = $this->m_records;
      $parent = $tmpparentrecs[0][$this->m_primaryKey[0]];
      atkdebug("Check for child records");
      $this->selectDb($this->m_table.'.'.$this->m_parent.'='.$parent);
      $tmprecs = $this->m_records;
      if(count($tmprecs)>0)
      {
        atkdebug('DeleteChilderen('.$this->m_table.'.'.$this->m_parent.'='.$parent.','.$parent.')');
        $this->deleteChilderen($this->m_table.'.'.$this->m_parent.'='.$parent,$parent);
      }
      $query = "DELETE FROM ".$this->m_table." WHERE ".$selector;
      $this->debug("deleteDb - query: ".$query);
      $g_db->query($query);
      // todo: instead of delete, set the deleted flag.
    }
    
    /**
         * Recursive function whitch deletes all the child records of a parent
         *
         * @Param $selector Selector
         * @Param $parent Parent
         */
    function deleteChilderen($selector,$parent)
    {
      global $g_db;
      atkdebug("Check for child records of the Child");
      $this->selectDb($this->m_table.'.'.$this->m_parent.'='.$parent);
      $tmprecs = $this->m_records;
      if(count($tmprecs)>0)
      {
        for($i=0;$i<count($tmprecs);$i++)
        {
          $parent = $tmprecs[$i][$this->m_primaryKey[0]];
          atkdebug('DeleteChilderen('.$this->m_table.'.'.$this->m_parent.'='.$tmprecs[$i][$this->m_primaryKey[0]].','.$parent.')');
          $this->deleteChilderen($this->m_table.'.'.$this->m_parent.'='.$tmprecs[$i][$this->m_primaryKey[0]],$parent);
        }
      }
      $query = "DELETE FROM ".$this->m_table." WHERE ".$selector;
      $this->debug("deleteChilderen - query: ".$query);
      $g_db->query($query);
      // todo: instead of delete, set the deleted flag.
    }
  }

 
?>