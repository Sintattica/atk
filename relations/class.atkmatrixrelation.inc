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
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /** flag(s) specific for atkMatrixRelation */
  define("AF_MATRIX_ACYCLIC", AF_SPECIFIC_1); // checks if a matrix is acyclic

  /** @internal include base class */
  userelation("atkrelation");

  /**
   * Many2Many relation that is represented by a clickable matrix.
   *
   * @deprecated This relation is no longer maintained and unsupported.
   *
   * @author Martin Pruijssers
   * @package atk
   * @subpackage relations
   *
   */
  class atkMatrixRelation extends atkRelation
  {
   var $m_name1;
   var $m_destination1;
   var $m_fk1;
   var $m_name2;
   var $m_destination2;
   var $m_fk2;

/**
    * Constructor
    * @param $name                 matrix table
    * @param $name1               row field name
    * @param $destination1     row table
    * @param $fk1                      forenkey in row table
    * @param $name2               colum field name
    * @param $destination2    colum table
     * @param $fk2                     forenkey in colum table
    * @param $flags                  AF_ACYCLIC if input must be acyclic
    */

    // 1 = row , 2 = col
    function atkMatrixRelation($name, $name1, $destination1, $fk1, $name2, $destination2, $fk2, $flags=0)
    {
      $this->atkRelation($name, $destination1, $flags);

      $this->m_name1 = $name1;
      $this->m_destination1 = $destination1;
      $this->m_fk1 = $fk1;
      $this->m_name2 = $name2;
      $this->m_destination2 = $destination2;
      $this->m_fk2 = $fk2;
    }


    function display($record)
    {

    }


    function load($db, $record)
    {

      $query = 'SELECT '.$this->m_name1.','.$this->m_name2.' FROM '.$this->m_name;
      $checked = $db->getRows($query);

      for ($i=0;$i<count($checked);$i++)
      {
         $field = $checked[$i][0].'_'.$checked[$i][1];
         $checkedArray[$field] = 'on';
      }

      return $checkedArray;
    }


    function edit($record="")
    {

      $checkedArray = $record[$this->fieldName()];

      $pkfield = $this->m_ownerInstance->m_primaryKey[0];

      $this->m_destination = $this->m_destination1;
      $this->m_destInstance="";
      $this->createDestination();

      if ($this->m_fk1!="") $where=$this->m_fk1."=".$record[$pkfield]; else $where="";

      $recordset = $this->m_destInstance->selectDb($where,"","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));

      for ($i=0;$i<count($recordset);$i++)
      {
        $matrix_fields_row[$i] = $this->m_destInstance->descriptor($recordset[$i]);

        $primaryKeyValue = split("'",$this->m_destInstance->primaryKey($recordset[$i]));
        $matrix_values_row[$i] = $primaryKeyValue[1];
      }

      $this->m_destination = $this->m_destination2;
      $this->m_destInstance="";
      $this->createDestination();

      if ($this->m_fk2!="") $where=$this->m_fk2."=".$record[$pkfield]; else $where="";

      $recordset = $this->m_destInstance->selectDb($where,"","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));

      for ($i=0;$i<count($recordset);$i++)
      {
        $matrix_fields_col[$i] = $this->m_destInstance->descriptor($recordset[$i]);

        $primaryKeyValue = split("'",$this->m_destInstance->primaryKey($recordset[$i]));
        $matrix_values_col[$i] = $primaryKeyValue[1];
      }


      if (count($matrix_fields_col)>1 && count($matrix_fields_row)>1)
      {      //matrix

        $form = '<table border=1><tr><td>&nbsp;</td>';
        for ($i=0;$i<count($matrix_fields_col);$i++)
        {
          $form .= '<td align="center" valign="top" bgcolor="#cccccc"><b><div style="line-height: 10px">';
          $text = $matrix_fields_col[$i];
          for ($j=0;$j<strlen($text);$j++)
          {
            $form .= $text[$j].'<br>';
          }
          $form .= '</div></b><br></td>';
        }
        $form .= '</tr>';

        $counter=0;
        for ($i=0;$i<count($matrix_fields_row);$i++)
        {
          $form .= '<tr><td bgcolor="#cccccc"><b>';
          $form .= '&nbsp;'.$matrix_fields_row[$i].'&nbsp;';
          $form .= '</b></td>';
          for ($j=0;$j<count($matrix_fields_col);$j++)
          {
            $key = $matrix_values_row[$i].'_'.$matrix_values_col[$j];
            if ($checkedArray[$key] == 'on') $check='CHECKED'; else $check='';

            $form .= '<td align="center">&nbsp;';
            $form .= '<input type="checkbox" name="'.$this->formName().'_AMDAE_'.$matrix_values_row[$i].'_'.$matrix_values_col[$j].'" '.$this->getCSSClassAttribute("atkcheckbox").' '.$check.'>';
            $form .= '&nbsp;</td>';
          }
          $form .= '</tr>';
        }
        $form .= '</table><br>';

      }
      elseif (count($matrix_fields_row)>1)
      { //many bool
        $form = '<table border=0 cellspacing=0 cellpadding=0>';

        $counter=0;
        for ($i=0;$i<count($matrix_fields_row);$i++)
        {
          $form .= '<tr>';
          for ($j=0;$j<count($matrix_fields_col);$j++)
          {
            $key = $matrix_values_row[$i].'_'.$matrix_values_col[$j];
            if ($checkedArray[$key] == 'on') $check='CHECKED'; else $check='';

            $form .= '<td align="center">&nbsp;';
            $form .= '<input type="checkbox" name="'.$this->formName().'_AMDAE_'.$matrix_values_row[$i].'_'.$matrix_values_col[$j].'" '.$this->getCSSClassAttribute("atkcheckbox").' '.$check.'>';
            $form .= '&nbsp;</td>';
            $form .= '<td>'.$matrix_fields_row[$i].'</td>';
          }
          $form .= '</tr>';
        }
        $form .= '</table><br>';
      }
      else
      {   //one or none bool
        $form = '';
      }

      return $form;
  }


    // check if the input is acyclic when flag is set
    function validate(&$record, $mode)
    {
      if ($this->hasFlag(AF_MATRIX_ACYCLIC))
      {
       // todo
      }
    }


    function isEmpty($notused)
    {
      return false;
    }


    function delete_before_store($db, $record)
    {
      if ($this->m_fk1=="" && $this->m_fk2!="") // manybool
      {
        $pkfield = $this->m_ownerInstance->m_primaryKey[0];

        $query = 'DELETE FROM '.$this->m_name.' WHERE '.$this->m_name2.' = '.$record[$pkfield];
        $db->query($query);

      }
      else
      {

        $pkfield = $this->m_ownerInstance->m_primaryKey[0];

        $this->m_destination = $this->m_destination1;
        $this->m_destInstance="";
        $this->createDestination();

        if ($this->m_fk1!="") $where=$this->m_fk1."=".$record[$pkfield]; else $where="";

        $recordset = $this->m_destInstance->selectDb($where,"","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));

        for ($i=0;$i<count($recordset);$i++)
        {
          $primaryKeyValue = split("'",$this->m_destInstance->primaryKey($recordset[$i]));
          $query = 'DELETE FROM '.$this->m_name.' WHERE '.$this->m_name1.' = '.$primaryKeyValue[1];
          $db->query($query);
        }
      }
    }

    function store($db, $record)
    {
      $this->delete_before_store($db, $record);

      $matrixcheckbox = $record[$this->fieldName()];

      if (is_array($matrixcheckbox)&&count($matrixcheckbox)>0)
      {
         while (list($key,$value)=each($matrixcheckbox))
         {
            $phases = split('_',$key);
            $query = 'INSERT INTO '.$this->m_name.' ('.$this->m_name1.','.$this->m_name2.') VALUES ('.$phases[0].','.$phases[1].')';
            $db->query($query);
         }
      }
      return true;
    }

  }

?>
