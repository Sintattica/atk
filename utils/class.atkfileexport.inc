<?php

  class atkfileexport
  {
   /**
    * export2file   exporteert data naar een file
    *@param  $data  -- the content
    *@param $fileName -- bestandsnaam
    *@param  $ext   -- extension of the file
    *@param  $type  -- the type (cvs / excel)
    *@param $compression -- Compression technique (bzip / gzip)
    * used by specialRecordist();
    * BROWSER BUG:
    * IE has problems with the use of attachment; needs atachment (someone at MS can't spell) or none.
    * however ns under version 6 accepts this also.
    * NS 6+ has problems with the absense of attachment; and the misspelling of attachment;
    * at present ie 5 on mac gives wrong filename and NS 6+ gives wrong filename.
    */

   function export($data, $fileName, $type, $ext, $compression="")
   {
    if($compression=="bzip")
    {
      $mime_type='application/x-bzip';
      $ext = "bz2";
    }
    elseif($compression=="gzip")
    {
      $mime_type='application/x-gzip';
      $ext = "gz";
    }
    elseif($type=="csv"||$type="excel")
    {
      $mime_type='text/x-csv';
      $ext = "csv";
    }
    else
    {
      $mime_type='application/octetstream';
    }

    header('Content-Type: '. $mime_type);
    header('Content-Disposition:  filename="'.$fileName.'.'.$ext.'"');
    header('Pragma: no-cache');
    header('Expires: 0');

    // 1. as a bzipped file
    if($compression=="bzip")
    {
      if (@function_exists('bzcompress'))
      {
        echo bzcompress($data);
      }
    }
    // 2. as a gzipped file
    else if ($compression == 'gzip')
    {
      if (@function_exists('gzencode'))
      {
        // without the optional parameter level because it bug
        echo gzencode($data);
      }
    }
    // 3. on screen
    else
    {
      echo $data;
    }

    exit;
   }
  }

?>