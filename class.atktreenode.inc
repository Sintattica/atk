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
    * Revision 4.24  2001/08/01 12:38:59  martin
    * added nowrap to some td's because of the ugly word wrap
    *
    * Revision 4.23  2001/07/18 16:07:45  ivo
    * Replaced dispatch.php by PHP_SELF
    *
    * Revision 4.22  2001/07/13 12:58:46  sandy
    * Small bugfix for the Tree, it now shows a correct tree in NS6
    *
    * Revision 4.21  2001/07/03 06:55:41  ivo
    * Treenode still refered to stacktrace() which should now be statusbar()
    *
    * Revision 4.20  2001/06/29 11:26:59  ivo
    * Patches to oracle support.
    *
    * Revision 4.19  2001/06/25 15:14:16  sandy
    * when there are no tree items, it won't show a leaf icon anymore
    *
    * Revision 4.18  2001/06/25 13:31:07  sandy
    * - Added 2 new tree icons to the CVS. And fixed the atktreenode. Now it
    * will also show a + (or -) symbol when the folder is at the end of a tree.
    * Don't forget to copy the new icons to your own theme when you upgrade atk.
    *
    * Revision 4.17  2001/06/19 11:42:35  ivo
    * New feature in treeview: leaf icon can now be userdefined.
    *
    * Revision 4.16  2001/06/12 13:19:55  ivo
    * Added a <br> to the bottom of the treeview.
    * Removed save and close button when editpage is bottom of the stack.
    *
    * Revision 4.15  2001/06/12 12:26:54  ivo
    * Fix to treenode: if no icondir is set, it takes the default.
    * Fix to atknode: display a '<br'> if no navigation is active.
    *
    * Revision 4.14  2001/06/11 13:18:36  sandy
    * - fixed a small bug in the copy tree function
    *
    * Revision 4.13  2001/06/08 14:33:30  ivo
    * Optimisations to atktreenode.
    * Smaller imp of row color alternationi feature.
    *
    * Revision 4.12  2001/05/18 07:49:42  ivo
    * Fixes to sessionmanagement.
    *
    * Revision 4.11  2001/05/17 12:39:37  ivo
    * Made atktreenode session aware (if you come back from an edit, your tree
    * state is restored)
    *
    * Revision 4.10  2001/05/10 08:31:00  ivo
    * Major upgrade. Changes:
    * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
    *   singletons by default, and nodefunctions pass around recordsets.
    * + Session management for forms. If you now leave a page through a click on
    *   a link, the session remembers everything from your form and restores it
    *   when you return.
    * + New relation: oneToOneRelation
    * + Reimplemented the embedded editForm feature (forms inside forms)
    *
    * Revision 4.9  2001/05/07 15:13:49  ivo
    * Put config_atkroot in all files.
    *
    * Revision 4.8  2001/05/02 13:06:23  sandy
    * fixed the AF_CASCADE_DELETE flag
    * CV: ----------------------------------------------------------------------
    *
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
  include_once($config_atkroot."atk/atktreetools.inc");
  
  $g_count = 1;
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
      global $g_securityManager,$g_layout,$g_maxlevel, $PHP_SELF;

      $g_layout->ui_top(text('title_'.$this->m_type.'_tree'));
      $g_layout->output($this->statusbar());
      $g_layout->output('<br>');

      $adminHeader = $this->adminHeader();
      if ($adminHeader!="")
      {
        $g_layout->output($adminHeader."<br><br>");
      }

      atkdebug("Entering treeview page.");
      //$this->addStickyVar($p);
      
      $recordset = $this->selectDb($this->m_postvars['atkfilter'],$this->m_primaryKey[0],"",$this->m_listExcludes);

      $t = new tree;
      for($i=0;$i<count($recordset);$i++)
      {
        $group=$recordset[$i];
        $t->addNode($recordset[$i][$this->m_primaryKey[0]],$this->descriptor($group),$recordset[$i][$this->m_parent][$this->m_primaryKey[0]]);    
      }
      $this->m_tree[0]["level"]=0; 
      $this->m_tree[0]["label"]="&nbsp;"; 
	    $this->m_tree[0]["id"]=''; 	    
	    $this->m_tree[0]["isleaf"]=1; 
      
      $test = $this->Fill_tree($t->m_tree);
      $g_maxlevel=$g_maxlevel+2;
      
      //var_dump($this->m_tree);

      $g_layout->output("<table border=\"0\" cellspacing=0 cellpadding=0 cols=".($g_maxlevel+2)." width=".(($g_maxlevel*30)+300).">\n");

      if (!$this->hasFlag(NF_NO_ADD) && $this->hasFlag(NF_ADD_LINK) && $this->allowed("add"))
      {
        $addurl = $PHP_SELF."?atkaction=add&atkfilter=".rawurlencode($this->m_parent.".".$this->m_primaryKey[0]."='0'");
        $g_layout->output(href($addurl,text("clickheretoadd_prefix").text($this->m_type).text("clickheretoadd_postfix"),SESSION_NESTED).'<br><br>');
      }

      $g_layout->output($this->GraphTreeRender());
      
      $g_layout->output("</table><br>");      
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
      global $g_count,$g_maxlevel, $exp_index;
      while (list($id,$objarr) = each($tree))
      {
        $this->m_tree[$g_count]["level"]=$level+1; 
		    $this->m_tree[$g_count]["label"]=$objarr->m_label; 
        $this->m_tree[$g_count]["img"]=$objarr->m_img;         
		    $this->m_tree[$g_count]["id"]=$objarr->m_id; 
        $exp_index[$objarr->m_id] = $g_count;        		    
		    $this->m_tree[$g_count]["isleaf"]=0; 
		    if ($this->m_tree[$g_count]["level"] > $g_maxlevel) $g_maxlevel=$this->m_tree[$g_count]["level"];     
        
		    $g_count++;
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
      global $g_maxlevel,$g_layout,$g_theme, $exp_index, $PHP_SELF;
   
      if(count($this->m_tree)==1) return "";  
   
      if($g_theme["TreeIcon_dir"]!=""&&is_dir("themes/".$g_theme["Name"]."/".$g_theme["TreeIcon_dir"]))
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
      $img_end_plus = $icon_path."tree_end_plus.gif";
      $img_end_minus= $icon_path."tree_end_minus.gif";
	    $img_leaf     = $icon_path."tree_leaf.gif"; 
	    $img_spc      = $icon_path."tree_space.gif"; 
  
      $res="";
      $lastlevel=0;
      
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
      /*********************************************/ 
    	/*  Get Node numbers to expand               */ 
    	/*********************************************/ 
   
	    if ($this->m_postvars["atktree"]!="") $explevels = explode("|",$this->m_postvars["atktree"]); 
   
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
			    while ($i<$this->m_tree[$cnt]["level"]-1)  
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
          if ($this->m_tree[$cnt]["isleaf"]==1 && $this->m_tree[$cnt+1]["level"]<$this->m_tree[$cnt]["level"])  
			    { 
			    	if ($cnt!=0) $res.= "<td><img src=\"".$img_end."\"></td>\n"; 
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
						      if($cnt!=0) $res.= "<td>".href($PHP_SELF."?".$params,"<img src=\"".$img_end_plus."\" border=no>")."</td>\n";
                }
                else
                {
						      if($cnt!=0) $res.= "<td>".href($PHP_SELF."?".$params,"<img src=\"".$img_plus."\" border=no>")."</td>\n";
                }
					    }
					    else
					    {
						    $res.="<td><img src=\"".$img_split."\" border=no></td>\n";
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
						      if($cnt!=0) $res.="<td>".href($PHP_SELF."?".$params,"<img src=\"".$img_end_minus."\" border=no>")."</td>\n";
                }
                else
                {
						      if($cnt!=0) $res.="<td>".href($PHP_SELF."?".$params,"<img src=\"".$img_minus."\" border=no>")."</td>\n";
                }
					    }
					    else
					    {
					    	$res.="<td><img src=\"".$img_split."\" border=no></td>\n";
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
					    $res.= "<td>".href($PHP_SELF."?".$params,"<img src=\"".$img_expand."\" border=no>")."</td>\n"; 
				    else 
					    $res.= "<td>".href($PHP_SELF."?".$params,"<img src=\"".$img_collapse."\" border=no>")."</td>\n";          
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
  				$res.= "<td colspan=".($g_maxlevel-$this->m_tree[$cnt]["level"])." nowrap><font size=2>".$this->m_tree[$cnt]["label"]."</font></td>\n"; 
      
    			/****************************************/ 
    			/* end row   with the functions                      */     
    			/****************************************/ 
          $res.='<td nowrap> ';
          if (!$this->hasFlag(NF_NO_ADD))
          {
            $res.=href($PHP_SELF."?atkaction=add&atkfilter=".$this->m_parent.".".$this->m_primaryKey[0].rawurlencode("='".$this->m_tree[$cnt]["id"]."'"), text("add"), SESSION_NESTED);
          }
          if($cnt>0)
          {
            if (!$this->hasFlag(NF_NO_EDIT))
            {
              $res.=' '.href($PHP_SELF."?atkaction=edit&atkselector=".$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt]["id"], text("edit"),SESSION_NESTED);
            }
            if(($this->hasFlag(NF_COPY)&&$this->allowed("add")&&!$this->hasflag(NF_TREE_NO_ROOT_COPY))||($this->m_tree[$cnt]["level"]!=1&&$this->hasFlag(NF_COPY)&&$this->allowed("add"))) 
            {    
              $res.=' '.href($PHP_SELF."?atkaction=copy&atkselector=".$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt]["id"], text("copy"));
            }
            if($this->hasFlag(NF_NO_DELETE)||($this->hasFlag(NF_TREE_NO_ROOT_DELETE)&&$this->m_tree[$cnt]["level"]==1))
            {
               // Do nothing
            }
            else
            {
              $res.=' '.href($PHP_SELF."?atkaction=delete&atkselector=".$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt]["id"], text("delete"),SESSION_NESTED);
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
      $recordset = $this->selectDb($selector);
      
      if(count($recordset)>0)
      { 
        $oldparent=$recordset[0][$this->m_primaryKey[0]];
        $this->addDb($recordset[0]);
        atkdebug("copyDb - Main Record added");
        $newparent=$recordset[0][$this->m_primaryKey[0]];
        atkdebug('CopyDbCopychildren('.$this->m_parent.'='.$oldparent.','.$newparent.')');
        $this->copyChildren($this->m_table.'.'.$this->m_parent.'='.$oldparent, $newparent);
      }
      else
      {
        atkdebug("No records found with Selector: $selector - $parent");
      }
      return "";
    }
    
    /**
        * This is a recursive function to copy the children from a parent.
        *
        * @param $selector Selector
        * @param $parent Parent ID
        */
    function copyChildren($selector, $parent="")
    {
      $recordset = $this->selectDb($selector);
      
      if(count($recordset)>0)
      {                
        for($i=0;$i<count($recordset);$i++)
        {  
          $recordset[$i][$this->m_parent] = array(""=>"",$this->m_primaryKey[0]=>$parent);                    
          $oldrec = $recordset[$i];
          $this->addDb(&$recordset[$i]);
          
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
      $recordset = $this->selectDb($selector);
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
      atkdebug("Check for child records");
      $children = $this->selectDb($this->m_table.'.'.$this->m_parent.'='.$parent);
      
      if(count($children)>0)
      {
        atkdebug('DeleteChildren('.$this->m_table.'.'.$this->m_parent.'='.$parent.','.$parent.')');
        $this->deleteChildren($this->m_table.'.'.$this->m_parent.'='.$parent,$parent);
      }
      $query = "DELETE FROM ".$this->m_table." WHERE ".$selector;

      $g_db->query($query);
      
      
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
      $recordset = $this->selectDb($this->m_table.'.'.$this->m_parent.'='.$parent);
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
      // todo: instead of delete, set the deleted flag.
    }
  }

 
?>
