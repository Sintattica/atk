<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Ui\Page;

/**
 * This is an extend of the famous atkfileattribute :). Now its possible
 * to insert one or more files in one database field.
 *
 * @author Martin Roest <martin@ibuildings.nl>
 */
class MultipleFileAttribute extends FileAttribute
{
    /*
     * private vars
     */
    public $m_delimiter = ';';

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param string|array $dir Can be a string with the Directory with images/files or an array with a Directory and a Display Url
     *
     */
    public function __construct($name, $flags = 0, $dir)
    {
        $flags = $flags | self::AF_CASCADE_DELETE;
        
        parent::__construct($name, $flags, '');
        
        if (is_array($dir)) {
            $this->m_dir = $this->AddSlash($dir[0]);
            $this->m_url = $this->AddSlash($dir[1]);
        } else {
            $this->m_dir = $this->AddSlash($dir);
            $this->m_url = $this->AddSlash($dir);
        }
    }

    /**
     * Returns an array with files extracted from the content of a databasefield.
     *
     * @param string $str content of dbfield
     * @param string $del delimiter
     *
     * @return array of files
     */
    public function getFiles($str, $del = '')
    {
        if ($del == '') {
            $del = $this->m_delimiter;
        }

        return explode($del, $str);
    }

    public function edit($record, $fieldprefix, $mode)
    {
        $file_arr = [];
        if (is_dir($this->m_dir)) {
            $d = dir($this->m_dir);
            while ($item = $d->read()) {
                if (is_file($this->m_dir.$item)) {
                    $file_arr[] = $item;
                }
            }
            $d->close();
        } else {
            return Tools::atktext('no_valid_directory');
        }

        if (Tools::count($file_arr) > 0) {
            $result = '<select multiple size="3" name="select_'.$this->fieldName().'[]" class="form-control">';
            for ($i = 0; $i < Tools::count($file_arr); ++$i) {
                $sel = '';
                if (in_array($file_arr[$i], $this->getFiles($record[$this->fieldName()]['orgfilename']))) {
                    $sel = 'selected';
                }
                if (is_file($this->m_dir.$file_arr[$i])) {
                    $result .= '<option value="'.$file_arr[$i].'" '.$sel.'>'.$file_arr[$i];
                }
            }
            if (Tools::count($file_arr) > 0) {
                $result .= '</select>';
            }
        } else {
            $result = 'No files found';
        }
        if (!$this->hasFlag(self::AF_FILE_NO_UPLOAD)) {
            $result .= ' <input type="file" name="'.$this->fieldName().'">';
        }

        return $result;
    }

    /**
     * Convert value to record for database.
     *
     * @param array $rec Array with Fields
     *
     * @return mixed Nothing or Fieldname or Original filename
     */
    public function value2db($rec)
    {
        $select = $_REQUEST['select_'.$this->fieldName()];
        $r = '';
        if (!$this->isEmpty($_POST)) {
            $file = $this->fetchValue($_POST);
            $file['filename'] = str_replace(' ', '_', $file['filename']);
            if ($file['filename'] != '') {
                @copy($file['tmpfile'], $this->m_dir.$file['filename']) or die('Save failed!');
                $r .= $file['filename'].';';
            }
        }
        if (is_array($$select)) {
            $r .= implode($this->m_delimiter, $$select);
        }

        return $r;
    }

    /**
     * Convert value to string.
     *
     * @param array $rec Array with fields
     *
     * @return array with tmpfile, orgfilename,filesize
     */
    public function db2value($rec)
    {
        return array(
            'tmpfile' => $this->m_dir.$rec[$this->fieldName()],
            'orgfilename' => $rec[$this->fieldName()],
            'filesize' => '?',
        );
    }

    /**
     * Display values.
     *
     * @param array $record Array with fields
     * @param string $mode
     *
     * @return string html
     */
    public function display($record, $mode)
    {
        $files = explode($this->m_delimiter, $record[$this->fieldName()]['orgfilename']);
        $prev_type = array('jpg', 'jpeg', 'gif', 'tif', 'png', 'bmp', 'htm', 'html', 'txt');  // file types for preview
        $imgtype_prev = array('jpg', 'jpeg', 'gif', 'png');  // types whitch are supported by GetImageSize
        $r = '';
        for ($i = 0; $i < Tools::count($files); ++$i) {
            if (is_file($this->m_dir.$files[$i])) {
                $ext = strtolower(substr($files[$i], strrpos($files[$i], '.') + 1, strlen($files[$i])));
                if (in_array($ext, $prev_type)) {
                    if (in_array($ext, $imgtype_prev)) {
                        $imagehw = getimagesize($this->m_dir.$files[$i]);
                    } else {
                        $imagehw = array('0' => '640', '1' => '480');
                    }

                    $r .= '<a href="'.$this->m_url.$files[$i].'" alt="'.$files[$i].'" onclick="ATK.Tools.newWindow(this.href,\'name\',\''.($imagehw[0] + 50).'\',\''.($imagehw[1] + 50).'\',\'yes\');return false;">'.$files[$i].'</a><br>';
                } else {
                    $r .= '<a href="'.$this->m_url."$files[$i]\" target=\"_new\">$files[$i]</a><br>";
                }
            } else {
                if (strlen($files[$i]) > 0) {
                    $r .= $files[$i].'(<font color="#ff0000">'.Tools::atktext('file_not_exist').'</font><br>)';
                }
            }
        }

        return $r;
    }
}
