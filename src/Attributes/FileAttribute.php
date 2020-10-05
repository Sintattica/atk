<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Utils\StringParser;

/**
 * With this you can upload, select and remove files in a given directory.
 *
 * @todo - Code clean up (del variable is dirty)
 *       - Support for storing the file itself in the db instead of on disk.
 *
 * @author Martin Roest <martin@ibuildings.nl>
 */
class FileAttribute extends Attribute
{
    /** flag(s) specific for the atkFileAttribute */
    /**
     * Disable uploading of files.
     */
    const AF_FILE_NO_UPLOAD = 33554432;

    /**
     * Disable selecting of files.
     */
    const AF_FILE_NO_SELECT = 67108864;

    /**
     * Disable deleting of files with the checkbox
     */
    const AF_FILE_NO_CHECKBOX_DELETE = 134217728;
    const AF_FILE_NO_DELETE = 134217728; // for backwards compatibility


    /**
     * Don't try to detect the file type (shows only filename).
     */
    const AF_FILE_NO_AUTOPREVIEW = 268435456;

    /**
     * Removed the files physically.
     */
    const AF_FILE_PHYSICAL_DELETE = 536870912;

    /**
     * Show preview in popup instead of inline.
     */
    const AF_FILE_POPUP = self::AF_POPUP;

    /**
     * Directory with images
     */
    public $m_dir = '';
    public $m_url = '';

    /**
     * Name mangle feature. If you set filename tpl, then uploaded files
     * are renamed to what you set in the template. You can use
     * fieldnames between brackets to have the filename determined by
     * the record.
     *
     * This is useful in the following case:
     * Say, you have a class for managing users. Each user has a photo associated
     * with them. Now, if two users would upload 'gandalf.gif', then you would
     * have a naming conflicht and the picture of one user is overwritten with the
     * one from the other user.
     * If you set m_filenameTpl to "picture_[name]", then the file is renamed before
     * it is stored. If the user's name is 'Ivo Jansch', and he uploads 'gandalf.gif',
     * then the file that is stored is picture_Ivo_Jansch.gif. This way, you have a
     * unique filename per user.
     */
    public $m_filenameTpl = '';

    /**
     * When set to true, a file is auto-renumbered if another record exists with the
     * same filename.
     *
     * @var boolean
     */
    public $m_autonumbering = false;

    /**
     * List of mime types which a uses is allowed to upload
     * Example: array('image/jpeg');
     *
     * @var array
     */
    public $m_allowedFileTypes = [];

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_STRING;

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param array|string $dir Can be a string with the Directory with images/files or an array with a Directory and a Display Url
     */
    public function __construct($name, $flags = 0, $dir)
    {
        $flags = $flags | self::AF_CASCADE_DELETE;
        parent::__construct($name, $flags);

        $this->setDir($dir);
        $this->setStorageType(self::POSTSTORE | self::ADDTOQUERY);
    }

    /**
     * Sets the directory into which uploaded files are saved.  (See setAutonumbering() and setFilenameTemplate()
     * for some other ways of manipulating the names of uploaded files.).
     *
     * @param mixed $dir string with directory path or array with directory path and display url (see constructor)
     * @return FileAttribute
     */
    public function setDir($dir)
    {
        if (is_array($dir)) {
            $this->m_dir = $this->AddSlash($dir[0]);
            $this->m_url = $this->AddSlash($dir[1]);
        } else {
            $this->m_dir = $this->AddSlash($dir);
            $this->m_url = $this->AddSlash($dir);
        }

        return $this;
    }

    /**
     * returns a string with a / on the end.
     *
     * @param string $dir_url String with the url/dir
     *
     * @return string with a / on the end
     */
    public function AddSlash($dir_url)
    {
        if (substr($dir_url, -1) !== '/') {
            $dir_url .= '/';
        }

        return $dir_url;
    }

    /**
     * Recursive rmdir.
     *
     * @see http://nl3.php.net/rmdir
     *
     * @param string $dir path to remove
     *
     * @return bool succes/failure
     *
     * @static
     */
    public static function rmdir($dir)
    {
        if (!($handle = @opendir($dir))) {
            return false;
        }

        while (false !== ($item = readdir($handle))) {
            if ($item != '.' && $item != '..') {
                if (is_dir("$dir/$item")) {
                    if (!self::rmdir("$dir/$item")) {
                        return false;
                    }
                } else {
                    if (!@unlink("$dir/$item")) {
                        return false;
                    }
                }
            }
        }
        closedir($handle);

        return @rmdir($dir);
    }

    /**
     * Turn auto-numbering of filenames on/off.
     *
     * When autonumbering is turned on, uploading a file with the same name as
     * the file of another record, will result in the file getting a unique
     * sequence number.
     *
     * @param bool $autonumbering
     * @return FileAttribute
     */
    public function setAutonumbering($autonumbering = true)
    {
        $this->m_autonumbering = $autonumbering;

        return $this;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record Record
     * @param string $fieldprefix Field prefix
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string piece of html code with a browsebox
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $result = '';

        // When in add mode or we have errors, don't show the filename above the input.
        $hasErrors = isset($record[$this->fieldName()]['error']) && $record[$this->fieldName()]['error'] != 0;
        if ($mode != 'add' && !$hasErrors) {
            if (method_exists($this->getOwnerInstance(), $this->fieldName().'_display')) {
                $method = $this->fieldName().'_display';
                $result = $this->m_ownerInstance->$method($record, 'view');
            } else {
                $result = $this->display($record, $mode);
            }
        }

        if (!is_dir($this->m_dir) || !is_writable($this->m_dir)) {
            Tools::atkwarning('atkFileAttribute: '.$this->m_dir.' does not exist or is not writeable');

            return Tools::atktext('no_valid_directory', 'atk').': '.$this->m_dir;
        }

        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);

        $style = '';
        foreach($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }
        if($style != ''){
            $style = ' style="'.$style."'";
        }

        if (isset($record[$this->fieldName()]['orgfilename'])) {
            $result .= '<br />';
            $result .= '<input type="hidden" name="'.$name.'_orgfilename" value="'.$record[$this->fieldName()]['orgfilename'].'">';
        }

        $onchange = '';
        if (Tools::count($this->m_onchangecode)) {
            $onchange = ' onChange="'.$id.'_onChange(this);"';
            $this->_renderChangeHandler($fieldprefix);
        }

        if (!$this->hasFlag(self::AF_FILE_NO_UPLOAD)) {
            $result .= '<input type="file" id="'.$id.'" name="'.$name.'" '.$onchange.$style.'>';
        }

        if (!$this->hasFlag(self::AF_FILE_NO_SELECT)) {
            $file_arr = $this->getFiles($this->m_dir);
            if (Tools::count($file_arr) > 0) {
                natcasesort($file_arr);

                $result .= '<select id="'.$id.'_select" name="'.$name.'[select]" '.$onchange.$style.' class="form-control select-standard">';
                // Add default option with value NULL
                $result .= '<option value="" selected>'.Tools::atktext('selection', 'atk');
                foreach($file_arr as $val){
                    (isset($record[$this->fieldName()]['filename']) && $record[$this->fieldName()]['filename'] == $val) ? $selected = 'selected' : $selected = '';
                    if (is_file($this->m_dir.$val)) {
                        $result .= '<option value="'.$val."\" $selected>".$val;
                    }
                }
                $result .= '</select>';
            }
        } else {
            if (isset($record[$this->fieldName()]['filename']) && !empty($record[$this->fieldName()]['filename'])) {
                $result .= '<input type="hidden" name="'.$name.'[select]" value="'.$record[$this->fieldName()]['filename'].'">';
            }
        }

        if (!$this->hasFlag(self::AF_FILE_NO_CHECKBOX_DELETE) && isset($record[$this->fieldName()]['orgfilename']) && $record[$this->fieldName()]['orgfilename'] != '') {
            $result .= '<br class="atkFileAttributeCheckboxSeparator"><label for="'.$id.'_del"><input id="'.$id.'_del" type="checkbox" name="'.$name.'[del]" '.$this->getCSSClassAttribute('atkcheckbox').'>&nbsp;'.Tools::atktext('remove_current_file',
                    'atk').'</label>';
        }

        return $result;
    }

    /**
     * Display values.
     *
     * @param array $record Array with fields
     * @param string $mode
     *
     * @return string Filename or Nothing
     */
    public function display($record, $mode)
    {
        // Get random number to use as param when displaying images
        // while updating images was not allways visible due to caching
        $randval = mt_rand();

        $filename = isset($record[$this->fieldName()]['filename']) ? $record[$this->fieldName()]['filename'] : null;
        Tools::atkdebug($this->fieldName()." - File: $filename");
        $prev_type = array(
            'jpg',
            'jpeg',
            'gif',
            'tif',
            'png',
            'bmp',
            'htm',
            'html',
            'txt',
        );  // file types for preview
        $imgtype_prev = array('jpg', 'jpeg', 'gif', 'png');  // types which are supported by GetImageSize
        if ($filename != '') {
            if (is_file($this->m_dir.$filename)) {
                $ext = $this->getFileExtension($filename);
                if ((in_array($ext, $prev_type) && $this->hasFlag(self::AF_FILE_NO_AUTOPREVIEW)) || (!in_array($ext, $prev_type))) {
                    return '<a href="'.$this->m_url."$filename\" target=\"_blank\">$filename</a>";
                } elseif (in_array($ext, $prev_type) && $this->hasFlag(self::AF_FILE_POPUP)) {
                    if (in_array($ext, $imgtype_prev)) {
                        $imagehw = getimagesize($this->m_dir.$filename);
                    } else {
                        $imagehw = array('0' => '640', '1' => '480');
                    }
                    return '<a href="'.$this->m_url.$filename.'" alt="'.$filename.'" onclick="ATK.Tools.newWindow(this.href,\'name\',\''.($imagehw[0] + 50).'\',\''.($imagehw[1] + 50).'\',\'yes\');return false;">'.$filename.'</a>';
                }

                return '<img src="'.$this->m_url.$filename.'?b='.$randval.'" alt="'.$filename.'">';
            } else {
                return $filename.' (<font color="#ff0000">'.Tools::atktext('file_not_exist', 'atk').'</font>)';
            }
        }
    }

    /**
     * Get the file extension.
     *
     * @param string $filename Filename
     *
     * @return string The file extension
     */
    public function getFileExtension($filename)
    {
        if ($dotPos = strrpos($filename, '.')) {
            return strtolower(substr($filename, $dotPos + 1, strlen($filename)));
        }

        return '';
    }

    /**
     * Returns an array containing files in specified directory
     * optionally filtered by settings from setAllowedFileTypes method.
     *
     * @param string $dir Directory to read files from
     *
     * @return array Array with files in specified dir
     */
    public function getFiles($dir)
    {
        $dirHandle = dir($dir);
        $file_arr = [];
        if (!$dirHandle) {
            Tools::atkerror("Unable to open directory {$dir}");

            return [];
        }

        while ($item = $dirHandle->read()) {
            if (Tools::count($this->m_allowedFileTypes) == 0) {
                if (is_file($this->m_dir.$item)) {
                    $file_arr[] = $item;
                }
            } else {
                $extension = $this->getFileExtension($item);

                if (in_array($extension, $this->m_allowedFileTypes)) {
                    if (is_file($this->m_dir.$item)) {
                        $file_arr[] = $item;
                    }
                }
            }
        }
        $dirHandle->close();

        return $file_arr;
    }

    /**
     * Set the allowed file types. This can either be mime types (detected by the / in the middle
     * or file extensions (without the leading dot!).
     *
     * @param array $types
     *
     * @return bool
     */
    public function setAllowedFileTypes($types)
    {
        if (!is_array($types)) {
            Tools::atkerror('FileAttribute::setAllowedFileTypes() Invalid types (types is not an array!');

            return false;
        }
        $this->m_allowedFileTypes = $types;

        return true;
    }

    /**
     * Convert value to string.
     *
     * @param array $rec Array with fields
     *
     * @return array Array with tmpfile, orgfilename,filesize
     */
    public function db2value($rec)
    {
        $retData = array(
            'tmpfile' => null,
            'orgfilename' => null,
            'filename' => null,
            'filesize' => null,
        );

        if (isset($rec[$this->fieldName()])) {
            $retData = array(
                'tmpfile' => $this->m_dir.$rec[$this->fieldName()],
                'orgfilename' => $rec[$this->fieldName()],
                'filename' => $rec[$this->fieldName()],
                'filesize' => '?',
            );
        }

        return $retData;
    }

    /**
     * Checks if the file has a valid filetype.
     *
     * Note that obligatory and unique fields are checked by the
     * atkNodeValidator, and not by the validate() method itself.
     *
     * @param array $record The record that holds the value for this
     *                       attribute. If an error occurs, the error will
     *                       be stored in the 'atkerror' field of the record.
     * @param string $mode The mode for which should be validated ("add" or
     *                       "update")
     */
    public function validate(&$record, $mode)
    {
        parent::validate($record, $mode);

        $this->isAllowedFileType($record);

        $error = isset($record[$this->fieldName()]['error']) ? $record[$this->fieldName()]['error'] : 0;
        if ($error > 0) {
            $error_text = $this->fetchFileErrorType($error);
            Tools::atkTriggerError($record, $this, $error_text, Tools::atktext($error_text, 'atk'));
        }
    }

    /**
     * Check whether the filetype is is one of the allowed
     * file formats. If the FileType array is empty this assumes that
     * all formats are allowed!
     *
     * @todo It turns out that handling mimetypes is not that easy
     * the mime_content_type has been deprecated and there is no
     * Os independend alternative! For now we only support a few
     * image mime types.
     *
     * @param array $rec
     *
     * @return bool
     */
    public function isAllowedFileType(&$rec)
    {
        if (Tools::count($this->m_allowedFileTypes) == 0) {
            return true;
        }

        // detect whether the file is uploaded or is an existing file.
        $filename = (!empty($rec[$this->fieldName()]['tmpfile'])) ? $rec[$this->fieldName()]['tmpfile'] : $this->m_dir.$rec[$this->fieldName()]['filename'];

        if (@empty($rec[$this->fieldName()]['postdel']) && $filename != $this->m_dir) {

            if (function_exists('getimagesize')) {
                $size = @getimagesize($filename);
                if (in_array($size['mime'], $this->m_allowedFileTypes)) {
                    return true;
                }
            }

            $orgFilename = isset($rec[$this->fieldName()]['orgfilename']) ? $rec[$this->fieldName()]['orgfilename'] : null;
            if ($orgFilename) {
                $extension = $this->getFileExtension($orgFilename);
                if (in_array($extension, $this->m_allowedFileTypes)) {
                    return true;
                }

                $rec[$this->fieldName()]['error'] = UPLOAD_ERR_EXTENSION;

                return false;
            }
        }

        return true;
    }

    /**
     * Tests the $_FILE error code and returns the corresponding atk error text token.
     *
     * @param int $error
     *
     * @return string error text token
     */
    public static function fetchFileErrorType($error)
    {
        switch ($error) {
            case UPLOAD_ERR_INI_SIZE:
            case UPLOAD_ERR_FORM_SIZE:
                $error = 'error_file_size';
                break;
            case UPLOAD_ERR_EXTENSION:
                $error = 'error_file_mime_type';
                break;
            case UPLOAD_ERR_NO_TMP_DIR:
            case UPLOAD_ERR_PARTIAL:
            default:
                $error = 'error_file_unknown';
        }

        return $error;
    }

    /**
     * Get filename out of Array.
     *
     * @param array $postvars Record
     *
     * @return array Array with tmpfile,filename,filesize,orgfilename
     */
    public function fetchValue($postvars)
    {
        $pvName = $this->getHtmlName();
        $del = $postvars[$pvName]['del'] ?? null;

        $basename = $this->fieldName();

        if (is_array($postvars['atkfiles']) || ($postvars[$pvName]['select'] != '') || ($postvars[$pvName]['filename'] != '')) { // php4
            // if an error occured during the upload process
            // and the error is not 'no file' while the field isn't obligatory or a file was already selected
            $fileselected = isset($postvars[$pvName]['select']) && $postvars[$pvName]['select'] != '';
            if (isset($postvars['atkfiles'][$basename]) && $postvars['atkfiles'][$basename]['error'] > 0 && !((!$this->hasFlag(self::AF_OBLIGATORY) || $fileselected) && $postvars['atkfiles'][$basename]['error'] == UPLOAD_ERR_NO_FILE)) {
                return array(
                    'filename' => $postvars['atkfiles'][$pvName]['name'],
                    'error' => $postvars['atkfiles'][$pvName]['error'],
                );
            } // if no new file has been uploaded..
            elseif (Tools::count($postvars['atkfiles']) == 0 || $postvars['atkfiles'][$basename]['tmp_name'] == 'none' || $postvars['atkfiles'][$basename]['tmp_name'] == '') {
                // No file to upload, then check if the select box is filled
                if ($fileselected) {
                    Tools::atkdebug('file selected!');
                    $filename = $postvars[$pvName]['select'];
                    $orgfilename = $filename;
                    $postdel = '';
                    if (isset($del) && $del == 'on') {
                        $filename = '';
                        $orgfilename = '';
                        $postdel = $postvars[$pvName]['select'];
                    }
                    $result = array(
                        'tmpfile' => '',
                        'filename' => $filename,
                        'filesize' => 0,
                        'orgfilename' => $orgfilename,
                        'postdel' => $postdel,
                    );
                }  // maybe we atk restored data from session
                elseif (isset($postvars[$pvName]['filename']) && $postvars[$pvName]['filename'] != '') {
                    $result = $postvars[$pvName];
                } else {
                    $filename = (isset($postvars[$basename.'_orgfilename'])) ? $postvars[$basename.'_orgfilename'] : '';

                    if ($del == 'on') {
                        $filename = '';
                    }

                    // Note: without file_exists() check, calling filesize() generates an error message:
                    $result = array(
                        'tmpfile' => $filename == '' ? '' : $this->m_dir.$filename,
                        'filename' => $filename,
                        'filesize' => is_file($this->m_dir.$filename) ? filesize($this->m_dir.$filename) : 0,
                        'orgfilename' => $filename,
                    );
                }
            } else {
                $realname = $this->_filenameMangle($postvars, $postvars['atkfiles'][$basename]['name']);

                if ($this->m_autonumbering) {
                    $realname = $this->_filenameUnique($postvars, $realname);
                }

                $result = array(
                    'tmpfile' => $postvars['atkfiles'][$basename]['tmp_name'],
                    'filename' => $realname,
                    'filesize' => $postvars['atkfiles'][$basename]['size'],
                    'orgfilename' => $realname,
                );
            }

            return $result;
        }
    }

    /**
     * Determine the real filename of a file.
     *
     * If a method <fieldname>_filename exists in the owner instance this method
     * is called with the record and default filename to determine the filename. Else
     * if a file template is set this is used instead and otherwise the default
     * filename is returned.
     *
     * @param array $rec The record
     * @param string $default The default filename
     *
     * @return string The real filename
     */
    public function _filenameMangle($rec, $default)
    {
        $method = $this->fieldName().'_filename';
        if (method_exists($this->m_ownerInstance, $method)) {
            return $this->m_ownerInstance->$method($rec, $default);
        } else {
            return $this->filenameMangle($rec, $default);
        }
    }

    /**
     * Determine the real filename of a file (based on m_filenameTpl).
     *
     * @param array $rec The record
     * @param string $default The default filename
     *
     * @return string The real filename based on the filename template
     */
    public function filenameMangle($rec, $default)
    {
        if ($this->m_filenameTpl == '') {
            $filename = $default;
        } else {
            $parser = new StringParser($this->m_filenameTpl);
            $includes = $parser->getAttributes();
            $record = $this->m_ownerInstance->updateRecord($rec, $includes, array($this->fieldName()));
            $record[$this->fieldName()] = substr($default, 0, strrpos($default, '.'));
            $ext = $this->getFileExtension($default);
            $filename = $parser->parse($record).($ext != '' ? '.'.$ext : '');
        }

        return str_replace(' ', '_', $filename);
    }

    /**
     * Give the file a uniquely numbered filename.
     *
     * @param array $rec The record for which the file was uploaded
     * @param string $filename The name of the uploaded file
     *
     * @return string The name of the uploaded file, renumbered if necessary
     */
    public function _filenameUnique($rec, $filename)
    {
        // check if there's another record using this same name. If so, (re)number the filename.
        Tools::atkdebug('FileAttribute::_filenameUnique() -> unique check');

        if ($dotPos = strrpos($filename, '.')) {
            $name = substr($filename, 0, strrpos($filename, '.'));
            $ext = substr($filename, strrpos($filename, '.'));
        } else {
            $name = $filename;
            $ext = '';
        }

        $owner = $this->m_ownerInstance;

        $query = $owner->getDb()->createQuery($this->m_ownerInstance->getTable());
        $query->addField($this->fieldName());
        $fieldnameSql = Db::quoteIdentifier($this->fieldName());
        $query->addCondition(
            "{$fieldnameSql}= :filename OR {$fieldnameSql} LIKE :name",
            [':filename' => $filename, ':name' => $name.'-%'.$ext]);
        if ($rec[$owner->primaryKeyField()] != '') {
            $query->addCondition($owner->primaryKey($rec, true));
        }

        $records = $query->executeSelect();

        if (!empty($records)) {
            // Check for the highest number
            $max_count = 0;
            foreach ($records as $record) {
                $dotPos = strrpos($record['filename'], '.');
                $dashPos = strrpos($record['filename'], '-');
                if ($dotPos !== false && $dashPos !== false) {
                    $number = substr($record['filename'], ($dashPos + 1), ($dotPos - $dashPos) - 1);
                } elseif ($dotPos === false && $ext == '' && $dashPos !== false) {
                    $number = substr($record['filename'], ($dashPos + 1));
                } else {
                    continue;
                }

                if (intval($number) > $max_count) {
                    $max_count = $number;
                }
            }
            // file name exists, so mangle it with a number.
            $filename = $name.'-'.($max_count + 1).$ext;
        }
        Tools::atkdebug('FileAttribute::_filenameUnique() -> New filename = '.$filename);

        return $filename;
    }

    /**
     * Deletes file from HD.
     *
     * @param array $record Array with fields
     *
     * @return bool False if the delete went wrong
     */
    public function postDelete($record)
    {
        if ($record[$this->fieldName()]['orgfilename'] != '') {
            $file = $this->m_dir.$record[$this->fieldName()]['orgfilename'];

            return $this->deleteFile($file);
        }

        return true;
    }

    /**
     * @param $file
     * @return bool
     */
    protected function deleteFile($file)
    {
        // return true even if the file is not physically deleted
        if (!$this->hasFlag(self::AF_FILE_PHYSICAL_DELETE)) {
            return true;
        }

        if (is_file($file) && !@unlink($file)) {
            return false;
        }

        return true;
    }

    /**
     * Retrieve the list of searchmodes which are supported.
     *
     * @return array List of supported searchmodes
     */
    public function getSearchModes()
    {
        // exact match and substring search should be supported by any database.
        // (the LIKE function is ANSI standard SQL, and both substring and wildcard
        // searches can be implemented using LIKE)
        // Possible values
        //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
        return array('substring', 'exact', 'wildcard', 'regexp');
    }

    /**
     * Set filename template.
     *
     * @param string $template
     */
    public function setFilenameTemplate($template)
    {
        $this->m_filenameTpl = $template;
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        $field = $record[$this->fieldName()];
        $result = '';
        if (is_array($field)) {
            foreach ($field as $key => $value) {
                $result .= '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'['.$key.']" '.'value="'.$value.'">';
            }
        } else {
            $result = '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'" value="'.$field.'">';
        }

        return $result;
    }

    public function store($db, $record, $mode)
    {
        if ($mode == 'update' && !empty($record[$this->fieldName()]['postdel'])) {
            $file = $this->m_dir.$record[$this->fieldName()]['postdel'];

            return $this->deleteFile($file);
        }

        $filename = $record[$this->fieldName()]['filename'];

        if ($record[$this->fieldName()]['tmpfile'] && $this->m_dir.$filename != $record[$this->fieldName()]['tmpfile']) {
            if ($filename != '') {
                $dirname = dirname($this->m_dir.$filename);
                if (!$this->mkdir($dirname)) {
                    Tools::atkerror("File could not be saved, unable to make directory '{$dirname}'");

                    return false;
                }

                if (@copy($record[$this->fieldName()]['tmpfile'], $this->m_dir.$filename)) {
                    $this->processFile($this->m_dir, $filename);

                    return true;
                } else {
                    Tools::atkerror("File could not be saved, unable to copy file '{$record[$this->fieldName()]['tmpfile']}' to destination '{$this->m_dir}{$filename}'");

                    return false;
                }
            }
        }

        return true;
    }

    /**
     * Recursive mkdir.
     *
     * @see http://nl2.php.net/mkdir
     *
     * @param string $path path to create
     *
     * @return bool success/failure
     *
     * @static
     */
    public static function mkdir($path)
    {
        $path = preg_replace('/(\/){2,}|(\\\){1,}/', '/', $path); //only forward-slash
        $dirs = explode('/', $path);

        $path = '';
        foreach ($dirs as $element) {
            $path .= $element.'/';
            if (!is_dir($path) && !mkdir($path)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Perform processing on an image right after it is uploaded.
     *
     * If you need any resizing or other postprocessing to be done on a file
     * after it is uploaded, you can create a derived attribute that
     * implements the processFile($filepath) method.
     * The default implementation does not do any processing.
     *
     * @param string $filepath The path of the uploaded file.
     * @param string $filename The name of the uploaded file.
     */
    public function processFile($filepath, $filename)
    {
    }

    /**
     * Check if the attribute is empty..
     *
     * @param array $record the record
     *
     * @return bool true if empty
     */
    public function isEmpty($record)
    {
        return @empty($record[$this->fieldName()]['filename']);
    }

    /**
     * Convert value to record for database.
     *
     * @param array $rec Array with Fields
     *
     * @return string Nothing or Fieldname or Original filename
     */
    public function value2db($rec)
    {
        $del = isset($rec[$this->fieldName()]['postdel']) ? $rec[$this->fieldName()]['postdel'] : null;

        if ($rec[$this->fieldName()]['tmpfile'] == '' && $rec[$this->fieldName()]['filename'] != '' && ($del != null || $del != $rec[$this->fieldName()]['filename'])) {
            return $rec[$this->fieldName()]['filename'];
        }

        if ($del != null) {
            return '';
        }

        return $rec[$this->fieldName()]['orgfilename'];
    }
}
