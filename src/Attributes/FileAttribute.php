<?php

namespace Sintattica\Atk\Attributes;

use Exception;
use SimpleXMLElement;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\StringParser;

/**
 * With this you can upload, select and remove files in a given directory.
 *
 * TODO:
 *  - Code clean up (del variable is dirty)
 *  - Support for storing the file itself in the db instead of on disk.
 *
 * @author Martin Roest <martin@ibuildings.nl>
 */
class FileAttribute extends Attribute
{
    /** flag(s) specific for the FileAttribute */
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

    /*
     * Directory with images
     */
    public $m_dir = '';
    public $m_url = '';

    /*
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

    /*
     * When set to true, a file is auto-renumbered if another record exists with the
     * same filename.
     *
     * @var boolean
     */
    public $m_autonumbering = false;

    /*
     * List of mime types which uses is allowed to upload
     * Example: array('image/jpeg');
     *
     * @var array
     */
    public $m_allowedFileTypes = [];

    private $checkUnique = true;
    private $fileMaxSize;
    private $fileMaxLength;
    private $onlyPreview = false;
    private $noSanitize = false;
    private $previewHeight = '100px';
    private $previewWidth = '100px';
    private $thumbnail = false; // TODO: handle thumbnail (v. display)
    private $thumbnailDir = 'thumbnail';
    private $thumbnailExt = null;

    // legacy mode: to show only link
    private $hideEditWidget = false;

    // set it TRUE to stream file from a directory that is not public
    // you need to implement the method 'action_ . DOWNLOAD_STREAM_ACTION_PREFIX . {attribute_file_name}'
    private $stream = false;
    // set it TRUE to show ad additional button to open the file in a new tab
    private $inline = false;
    const INLINE_PARAM = 'inline_file_attribute';
    const ALLOWED_INLINE_MIMETYPE = ['application/pdf', 'image/jpeg', 'image/png'];// potenzialmente estendibile

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param string[]|string $dir Can be a string with the Directory with images/files or an array with a Directory and a Display Url
     */
    public function __construct($name, $flags = 0, $dir = null)
    {
        if (!$dir) {
            $dir = ['./' . Config::getGlobal('default_upload_dir'), '/' . Config::getGlobal('default_upload_url')];
        }

        parent::__construct($name, $flags | self::AF_CASCADE_DELETE | self::AF_FILE_NO_SELECT | self::AF_FILE_PHYSICAL_DELETE);

        $this->setDir($dir);
        $this->setStorageType(self::POSTSTORE | self::ADDTOQUERY);
    }

    /**
     * returns a string with a / on the end.
     *
     * @param string $dir_url String with the url/dir
     *
     * @return string with a / on the end
     */
    public function addSlash(string $dir_url): string
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
    public static function rmdir(string $dir): bool
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
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record Record
     * @param string $fieldprefix Field prefix
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string piece of html code with a browsebox
     * @throws Exception
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $result = '';

        // When in add mode or when we have errors, don't show the filename above the input.
        $hasErrors = isset($record[$this->fieldName()]['error']) && $record[$this->fieldName()]['error'] != 0;
        if ($mode != 'add' && !$hasErrors) {
            if (method_exists($this->getOwnerInstance(), $this->fieldName() . '_display')) {
                $method = $this->fieldName() . '_display';
                $fileLink = $this->m_ownerInstance->$method($record, 'view');
            } else {
                $fileLink = $this->display($record, $mode);
            }

            $relativeFilePath = $this->getRelativeFilePath($record);
            if (isset($relativeFilePath) && $relativeFilePath != '') {
                // a file was loaded
                $fileLinkXML = new SimpleXMLElement($fileLink);

                if ($this->hideEditWidget) {
                    // show only link
                    $widgetFile = '<div><a href="' . ($fileLinkXML['href'] ?? '') . '" target="_blank">' . $relativeFilePath . '</a></div>';

                } else {
                    $widgetFile = '<div class="existing-file input-group">
                    <div class="input-group-prepend">
                      <span class="input-group-text"><i class="fas fa-file"></i></span>
                    </div>
                    <input type="text" class="form-control form-control-sm" disabled value="' . $relativeFilePath . '" />
                    <div class="input-group-append">
                     <a href="' . ($fileLinkXML['href'] ?? "") . '" target="_blank" class="btn btn-sm btn-outline-primary"><i class="fas fa-arrow-alt-circle-down"></i></a>
                    </div>
                  </div>';
                }

                if ($this->fileExists($record)) {
                    // the loaded file exists
                    if (!$this->isStream() && $this->isImage($record) && !$this->hasFlag(self::AF_FILE_NO_AUTOPREVIEW)) {
                        // add the preview before the widget
                        $result .= $fileLink;
                    }
                    $result .= $widgetFile;

                } else {
                    // the loaded file does not exist: no widget
                    $result .= $fileLink;
                }
            }
        }

        if (!is_dir($this->m_dir) || !is_writable($this->m_dir)) {
            Tools::atkwarning('atkFileAttribute: ' . $this->m_dir . ' does not exist or is not writeable');
            return Tools::atktext('no_valid_directory', 'atk') . ': ' . $this->m_dir;
        }

        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);

        $style = '';
        foreach ($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }
        if ($style != '') {
            $style = ' style="' . $style . "'";
        }

        if (isset($record[$this->fieldName()]['orgfilename'])) {
            $result .= '<input type="hidden" name="' . $name . '_orgfilename" value="' . $record[$this->fieldName()]['orgfilename'] . '">';
        }

        $onchange = '';
        if (Tools::count($this->m_onchangecode)) {
            $onchange = ' onChange="' . $id . '_onChange(this);"';
            $this->_renderChangeHandler($fieldprefix);
        }

        if (!$this->hasFlag(self::AF_FILE_NO_UPLOAD)) {
            $result .= '<input class="mt-2" type="file" id="' . $id . '" name="' . $name . '" ' . $onchange . $style . '>';
        }

        $relativeFilePath = $this->getRelativeFilePath($record);
        if (!$this->hasFlag(self::AF_FILE_NO_SELECT)) {
            $file_arr = $this->getFiles($this->m_dir);
            if (Tools::count($file_arr) > 0) {
                natcasesort($file_arr);

                $result .= '<select id="' . $id . '_select" name="' . $name . '[select]" ' . $onchange . $style . ' class="form-control select-standard">';
                // Add default option with value NULL
                $result .= '<option value="" selected>' . Tools::atktext('selection', 'atk');
                foreach ($file_arr as $val) {
                    (isset($relativeFilePath) && $relativeFilePath == $val) ? $selected = 'selected' : $selected = '';
                    if (is_file($this->m_dir . $val)) {
                        $result .= '<option value="' . $val . "\" $selected>" . $val;
                    }
                }
                $result .= '</select>';
            }
        } else {
            if (!empty($relativeFilePath)) {
                $result .= '<input type="hidden" name="' . $name . '[select]" value="' . $relativeFilePath . '">';
            }
        }

        if (!$this->hasFlag(self::AF_FILE_NO_CHECKBOX_DELETE) && isset($record[$this->fieldName()]['orgfilename']) && $record[$this->fieldName()]['orgfilename'] != '') {
            $result .= '<div class="icheck-primary"><input id="' . $id . '_del" type="checkbox" name="' . $name . '[del]" ' . $this->getCSSClassAttribute() . '>';
            $result .= '<label for="' . $id . '_del">' . Tools::atktext('remove_current_file', 'atk') . '</label></div>';
        }

        if ($this->m_allowedFileTypes) {
            // show allowed extensions
            $result .= '<span style="color: #737373">(' . implode(', ', $this->m_allowedFileTypes) . ')</span>';
        }

        return $result;
    }

    /**
     * Display values.
     *
     * @param array<string, mixed> $record Array with fields
     * @param string $mode
     *
     * @return string Filename or Nothing
     */
    public function display(array $record, string $mode): string
    {
        // Get random number to use as param when displaying images
        // while updating images was not allways visible due to caching
        $randval = mt_rand();

        $relativeFilePath = $this->getRelativeFilePath($record);
        if (!isset($relativeFilePath)) {
            return '';
        }

        $ret = '';
        if ($relativeFilePath) {
            if ($this->fileExists($record)) {
                $url = $this->m_url . $relativeFilePath;
                $isImage = $this->isImage($record);

                if ($this->thumbnail && !$this->thumbnailExists($record)) {
                    // thumbnail not found
                    $ret .= '<div>' . $this->getRelativeThumbnailPath($record);
                    if ($mode != 'add') {
                        $ret .= ' (<span style="color: #ff0000">' . Tools::atktext('file_not_exist', 'atk') . '</span>)';
                    }
                    $ret .= '</div>';

                } else {

                    if ($this->isStream()) {
                        $node = $this->getOwnerInstance();

                        $justify = $mode === 'list' ? 'justify-content-center' : '';
                        if ($this->thumbnail) {
                            // show thumbnail
                            $content = base64_encode(file_get_contents($this->getThumbnailUrl($record)));
                            $ret .= "<div class='row no-gutters $justify'>";
                            $ret .= "<img src='data:image/png;base64,$content' 
                                style='max-height: {$this->getPreviewHeight()};
                                max-width: {$this->getPreviewWidth()}; 
                                margin: 5px 0;'/>";
                            $ret .= '</div>';
                        }

                        $downloadAttr = (new ActionButtonAttribute('btn_download_file_attribute_' . $this->fieldName()))
                            ->setNode($node)
                            ->setText($this->text('download'))
                            ->setAction(Node::ACTION_DOWNLOAD_FILE_ATTRIBUTE)
                            ->setTarget('_blank')
                            ->setParams([
                                Node::PARAM_ATKSELECTOR => $node->getPrimaryKey($record),
                                Node::PARAM_ATTRIBUTE_NAME => $this->fieldName()
                            ]);
                        $downloadAttr->setOwnerInstance($node);
                        $ret .= "<div class='row no-gutters $justify'>";
                        $ret .= $downloadAttr->display($record, 'view');

                        if ($this->isInlineButtonEnabled() && in_array($mode, ['list', 'view', 'edit'])) {
                            $this->setMinWidth('125px');
                            $openNewTabAttr = (new ActionButtonAttribute('btn_download_file_attribute_' . $this->fieldName()))
                                ->setNode($node)
                                ->setText($this->text('open'))
                                ->setAction(Node::ACTION_DOWNLOAD_FILE_ATTRIBUTE)
                                ->setTarget('_blank')
                                ->setParams([
                                    Node::PARAM_ATKSELECTOR => $node->getPrimaryKey($record),
                                    Node::PARAM_ATTRIBUTE_NAME => $this->fieldName(),
                                    self::INLINE_PARAM => true
                                ]);
                            $openNewTabAttr->setOwnerInstance($node);
                            $openNewTabAttr->addCSSClass('ml-1');
                            $ret .= "{$openNewTabAttr->display($record, 'view')}";
                        }
                        $ret .= '</div>';

                    } else {
                        // not stream

                        // link target blank
                        $ret = sprintf('<a target="_blank" href="%s">', $url);

                        if (!$isImage || $this->hasFlag(self::AF_FILE_NO_AUTOPREVIEW) || !$this->onlyPreview) {
                            $ret .= basename($relativeFilePath);
                        }

                        if ($isImage && !$this->hasFlag(self::AF_FILE_NO_AUTOPREVIEW)) {
                            if (!$this->onlyPreview) {
                                $ret .= '<br/>';
                            }

                            $displayUrl = $this->thumbnail ? $this->getThumbnailUrl($record) : $url;
                            $ret .= "<img src='$displayUrl?b=$randval' style=' 
                                max-height: {$this->getPreviewHeight()};
                                max-width: {$this->getPreviewWidth()}; 
                                margin: 5px 0;'/>";
                        }
                        $ret .= '</a>';
                    }
                }

            } else {
                // file not found
                $ret = '<div>' . basename($relativeFilePath);
                if ($mode != 'add') {
                    $ret .= ' (<span style="color: #ff0000">' . Tools::atktext('file_not_exist', 'atk') . '</span>)';
                }
                $ret .= '</div>';
            }
        }

        return $ret;
    }

    /**
     * Convert value to string.
     *
     * @param array $record Array with fields
     *
     * @return array Array with tmpfile, orgfilename,filesize
     */
    public function db2value($record)
    {
        $retData = [
            'tmpfile' => null,
            'orgfilename' => null,
            'filename' => null,
            'filesize' => null,
        ];

        if (isset($record[$this->fieldName()])) {
            $retData = [
                'tmpfile' => $this->m_dir . $record[$this->fieldName()],
                'orgfilename' => $record[$this->fieldName()],
                'filename' => $record[$this->fieldName()],
                'filesize' => '?',
            ];
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
     * @param string $mode The mode for which should be validated ('add'|'update')
     */
    public function validate(&$record, $mode)
    {
        parent::validate($record, $mode);

        $this->isAllowedFileType($record);

        $error = $record[$this->fieldName()]['error'] ?? 0;
        if ($error > 0) {
            $error_text = $this->fetchFileErrorType($error);
            Tools::atkTriggerError($record, $this, $error_text, Tools::atktext($error_text, 'atk'));
        }

        if ($record[$this->fieldName()]['error']) {
            return;
        }

        if ($this->isNewFileLoaded($record) && ($mode == 'add' || !$this->hasFlag(self::AF_READONLY_EDIT))) {
            // a new file is loaded and we are in add mode or the attribute is not readonly edit

            // check file size
            if ($this->fileMaxSize && $record[$this->fieldName()]['filesize'] > $this->fileMaxSize) {
                Tools::atkTriggerError($record, $this, 'error_file_size');
                return;
            }

            // check filename length
            if ($this->fileMaxLength && strlen($this->getRelativeFilePath($record)) > $this->fileMaxLength) {
                $realMaxLength = $this->fileMaxLength - strlen(pathinfo($this->getRelativeFilePath($record), PATHINFO_DIRNAME)) - 1;
                Tools::atkTriggerError($record, $this, sprintf(Tools::atktext('error_file_length'), $realMaxLength));
                return;
            }

            // sanitize filename
            $realname = $this->sanitizeFilename($record[$this->fieldName()]['orgfilename']);
            if (!$realname) {
                Tools::atkTriggerError($record, $this, 'filename_invalid');
                return;
            }
            $record[$this->fieldName()]['filename'] = $realname;
            $record[$this->fieldName()]['orgfilename'] = $realname;

            if ($this->checkUnique) {
                // check that the uploaded file is unique (in the destination dir)
                // Possible uploading cases:
                // - in add
                // - in edit, uploading a file that wasn't there before
                // - in edit, uploading a file with different name
                // - in edit, uploading a file with same name -> N THIS CASE, IT SHOULD NOT PREVENT IT (must be overwritten)

                $oldFilename = '';
                $oldFile = [];
                if ($mode != 'add') { // in edit
                    // retrieves the file already uploaded previously
                    $node = $this->getOwnerInstance();
                    $oldRec = $node->select($record['atkprimkey'])->includes($this->fieldName())->getFirstRow();
                    $oldFile = $oldRec[$this->fieldName()];
                    if ($oldFile['filename']) {
                        $oldFilename = $oldFile['filename'];
                    }
                }

                if (!$oldFilename || $oldFilename != $realname) {
                    // check the file is unique
                    if (file_exists($this->m_dir . $realname)) {
                        if ($oldFilename) {
                            $record[$this->fieldName()] = $oldFile;
                        } else {
                            $record[$this->fieldName()] = null;
                        }
                        Tools::atkTriggerError($record, $this, 'error_attachment_unique', sprintf($this->text('error_attachment_unique'), basename($realname)));
                    }
                }
            }
        }
    }

    function sanitizeFilename($filename): ?string
    {
        // for some file the orgfilename is an array instead of a string
        if (is_array($filename) && isset($filename['atkfiles']) && isset($filename['atkfiles']['name'])) {
            $filename = $filename['atkfiles']['name'];
        }

        $pathInfo = pathinfo($filename);
        $filename = $this->noSanitize ? $pathInfo['basename'] : Tools::sanitizeFilename($pathInfo['basename']);

        return $filename ? $pathInfo['dirname'] . '/' . $filename : null;
    }

    /**
     * Check whether the filetype is is one of the allowed
     * file formats. If the FileType array is empty this assumes that
     * all formats are allowed!
     *
     * @param array $rec
     *
     * @return bool
     *
     * TODO: It turns out that handling mimetypes is not that easy
     *  the mime_content_type has been deprecated and there is no
     *  Os independend alternative! For now we only support a few
     *  image mime types.
     *
     */
    public function isAllowedFileType(array &$rec): bool
    {
        if (Tools::count($this->m_allowedFileTypes) == 0) {
            return true;
        }

        // detect whether the file is uploaded or is an existing file.
        $filename = (!empty($rec[$this->fieldName()]['tmpfile'])) ? $rec[$this->fieldName()]['tmpfile'] : $this->m_dir . $rec[$this->fieldName()]['filename'];

        if (@empty($rec[$this->fieldName()]['postdel']) && $filename != $this->m_dir) {

            if (function_exists('getimagesize')) {
                $size = @getimagesize($filename);
                if (in_array($size['mime'], $this->m_allowedFileTypes)) {
                    return true;
                }
            }

            $orgFilename = $rec[$this->fieldName()]['orgfilename'] ?? null;
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
    public static function fetchFileErrorType(int $error): string
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
        $del = $postvars[$this->fieldName()]['del'] ?? null;

        $basename = $this->fieldName();

        if (is_array($postvars['atkfiles']) || ($postvars[$this->fieldName()]['select'] != '') || ($postvars[$this->fieldName()]['filename'] != '')) { // php4
            // if an error occured during the upload process
            // and the error is not 'no file' while the field isn't obligatory or a file was already selected
            $fileselected = isset($postvars[$this->fieldName()]['select']) && $postvars[$this->fieldName()]['select'] != '';
            if (isset($postvars['atkfiles'][$basename]) && $postvars['atkfiles'][$basename]['error'] > 0 && !((!$this->hasFlag(self::AF_OBLIGATORY) || $fileselected) && $postvars['atkfiles'][$basename]['error'] == UPLOAD_ERR_NO_FILE)) {
                return [
                    'filename' => $postvars['atkfiles'][$this->fieldName()]['name'],
                    'error' => $postvars['atkfiles'][$this->fieldName()]['error'],
                ];

            } elseif (Tools::count($postvars['atkfiles']) == 0 || $postvars['atkfiles'][$basename]['tmp_name'] == 'none' || $postvars['atkfiles'][$basename]['tmp_name'] == '') {
                // No file to upload, then check if the select box is filled
                if ($fileselected) {
                    Tools::atkdebug('file selected!');
                    $filename = $postvars[$this->fieldName()]['select'];
                    $orgfilename = $filename;
                    $postdel = '';
                    if (isset($del) && $del == 'on') {
                        $filename = '';
                        $orgfilename = '';
                        $postdel = $postvars[$this->fieldName()]['select'];
                    }
                    $result = [
                        'tmpfile' => '',
                        'filename' => $filename,
                        'filesize' => 0,
                        'orgfilename' => $orgfilename,
                        'postdel' => $postdel,
                    ];

                } elseif (isset($postvars[$this->fieldName()]['filename']) && $postvars[$this->fieldName()]['filename'] != '') {
                    // restores data from session
                    $result = $postvars[$this->fieldName()];

                } else {
                    $filename = (isset($postvars[$basename . '_orgfilename'])) ? $postvars[$basename . '_orgfilename'] : '';

                    if ($del == 'on') {
                        $filename = '';
                    }

                    // Note: without file_exists() check, calling filesize() generates an error message:
                    $result = [
                        'tmpfile' => $filename == '' ? '' : $this->m_dir . $filename,
                        'filename' => $filename,
                        'filesize' => is_file($this->m_dir . $filename) ? filesize($this->m_dir . $filename) : 0,
                        'orgfilename' => $filename,
                    ];
                }
            } else {
                $realname = $this->_filenameMangle($postvars, $postvars['atkfiles'][$basename]['name']);

                if ($this->m_autonumbering) {
                    $realname = $this->filenameUnique($postvars, $realname);
                }

                $result = [
                    'tmpfile' => $postvars['atkfiles'][$basename]['tmp_name'],
                    'filename' => $realname,
                    'filesize' => $postvars['atkfiles'][$basename]['size'],
                    'orgfilename' => $realname,
                ];
            }

            return $result;
        }

        return null;
    }

    /**
     * Determine the real filename of a file (based on m_filenameTpl).
     *
     * @param array $record The record
     * @param string $default The default filename
     *
     * @return string The real filename based on the filename template
     */
    public function filenameMangle(array $record, string $default): string
    {
        if ($this->m_filenameTpl == '') {
            $filename = $default;

        } else {
            $parser = new StringParser($this->m_filenameTpl);
            $includes = $parser->getAttributes();
            $recordUpdated = $this->m_ownerInstance->updateRecord($record, $includes, [$this->fieldName()]);
            $recordUpdated[$this->fieldName()] = substr($default, 0, strrpos($default, '.'));
            $ext = $this->getFileExtension($default);
            $filename = $parser->parse($recordUpdated) . ($ext != '' ? '.' . $ext : '');
        }

        return str_replace(' ', '_', $filename);
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
            $file = $this->m_dir . $record[$this->fieldName()]['orgfilename'];

            return $this->deleteFile($file);
        }

        return true;
    }

    /**
     * @param string $file
     * @return bool
     */
    protected function deleteFile(string $file): bool
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
        return ['substring', 'exact', 'wildcard', 'regexp'];
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
                $result .= '<input type="hidden" name="' . $this->getHtmlName($fieldprefix) . '[' . $key . ']" ' . 'value="' . $value . '">';
            }
        } else {
            $result = '<input type="hidden" name="' . $this->getHtmlName($fieldprefix) . '" value="' . $field . '">';
        }

        return $result;
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return string "string" which is the 'generic' type of the database field for this attribute.
     */
    public function dbFieldType()
    {
        return 'string';
    }

    /**
     * @throws Exception
     */
    public function store($db, $record, $mode)
    {
        if ($mode == 'update' && !empty($record[$this->fieldName()]['postdel'])) {
            $file = $this->m_dir . $record[$this->fieldName()]['postdel'];
            return $this->deleteFile($file);
        }

        $relativeFilePath = $this->getRelativeFilePath($record);
        $absoluteFilePath = $this->getAbsoluteFilePath($record);

        if ($record[$this->fieldName()]['tmpfile'] && $absoluteFilePath != $record[$this->fieldName()]['tmpfile']) {
            if ($relativeFilePath != '') {
                $dirname = dirname($absoluteFilePath);

                if (!$this->mkdir($dirname)) {
                    Tools::atkerror("File could not be saved, unable to make directory '$dirname'");
                    return false;
                }

                if (@copy($record[$this->fieldName()]['tmpfile'], $absoluteFilePath)) {
                    $this->processFile($this->m_dir, $relativeFilePath);
                    return $this->escapeSQL($relativeFilePath);

                } else {
                    Tools::atkerror("File could not be saved, unable to copy file '{$record[$this->fieldName()]['tmpfile']}' to destination '$absoluteFilePath'");
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
    public static function mkdir(string $path): bool
    {
        $path = preg_replace('/(\/){2,}|(\\\){1,}/', '/', $path); //only forward-slash
        $dirs = explode('/', $path);

        $path = '';
        foreach ($dirs as $element) {
            $path .= $element . '/';
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
    public function processFile(string $filepath, string $filename)
    {
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record = [], $level = 0, $mode = '')
    {
        if ($mode == 'add' || $mode == 'update') {
            if (empty($rec[$this->fieldName()]['postdel']) && $this->isEmpty($record) && !$this->hasFlag(self::AF_OBLIGATORY) && !$this->isNotNullInDb()) {
                $query->addField($this->fieldName(), 'NULL', '', '', false, true);
            } else {
                $query->addField($this->fieldName(), $this->value2db($record), '', '', !$this->hasFlag(self::AF_NO_QUOTES), true);
            }
        } else {
            $query->addField($this->fieldName(), '', $tablename, $fieldaliasprefix, !$this->hasFlag(self::AF_NO_QUOTES), true);
        }
    }

    /**
     * Check if the attribute is empty.
     *
     * @param array $record the record
     *
     * @return bool true if empty
     */
    public function isEmpty($record)
    {
        return empty($this->getRelativeFilePath($record));
    }

    /**
     * Convert value to record for database.
     *
     * @param array $record Array with Fields
     *
     * @return string Nothing or Fieldname or Original filename
     */
    public function value2db(array $record)
    {
        $del = $record[$this->fieldName()]['postdel'] ?? null;

        $relativeFilePath = $this->getRelativeFilePath($record);
        if ($record[$this->fieldName()]['tmpfile'] == '' && $relativeFilePath != '' && ($del != null || $del != $relativeFilePath)) {
            return $this->escapeSQL($relativeFilePath);
        }

        if ($del != null) {
            return '';
        }

        return $this->escapeSQL($record[$this->fieldName()]['orgfilename']);
    }

    /**
     * Sets the directory into which uploaded files are saved.  (See setAutonumbering() and setFilenameTemplate()
     * for some other ways of manipulating the names of uploaded files.).
     *
     * @param string[]|string $dir string with directory path or array with directory path and display url (see constructor)
     * @return FileAttribute
     */
    public function setDir($dir): self
    {
        if (is_array($dir)) {
            $this->m_dir = $this->addSlash($dir[0]);
            $this->m_url = $this->addSlash($dir[1]);
        } else {
            $this->m_dir = $this->addSlash($dir);
            $this->m_url = $this->addSlash($dir);
        }

        return $this;
    }

    public function getDir(): string
    {
        return $this->m_dir;
    }

    public function getUrl(): string
    {
        return $this->m_url;
    }

    /**
     * Get the file extension.
     *
     * @param array|string $filename Filename
     *
     * @return string The file extension
     */
    public function getFileExtension($filename): string
    {
        if (is_array($filename) && isset($filename['atkfiles']) && isset($filename['atkfiles']['name'])) {
            $filename = $filename['atkfiles']['name'];
        }

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
     * @param string $del
     *
     * @return array Array with files in specified dir
     * @throws Exception
     */
    public function getFiles(string $dir, string $del = ''): array
    {
        $dirHandle = dir($dir);
        $file_arr = [];
        if (!$dirHandle) {
            Tools::atkerror("Unable to open directory $dir");

            return [];
        }

        while ($item = $dirHandle->read()) {
            if (Tools::count($this->m_allowedFileTypes) == 0) {
                if (is_file($this->m_dir . $item)) {
                    $file_arr[] = $item;
                }
            } else {
                $extension = $this->getFileExtension($item);

                if (in_array($extension, $this->m_allowedFileTypes)) {
                    if (is_file($this->m_dir . $item)) {
                        $file_arr[] = $item;
                    }
                }
            }
        }
        $dirHandle->close();

        return $file_arr;
    }

    public function setFilenameTemplate(string $template): self
    {
        $this->m_filenameTpl = $template;
        return $this;
    }

    /**
     * Set the allowed file types. This can either be mime types (detected by the / in the middle
     * or file extensions (without the leading dot!).
     *
     * @param array $types
     *
     * @return FileAttribute
     */
    public function setAllowedFileTypes(array $types): self
    {
        $this->m_allowedFileTypes = $types;
        return $this;
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
    public function setAutonumbering(bool $autonumbering = true): self
    {
        $this->m_autonumbering = $autonumbering;
        return $this;
    }

    function setCheckUnique(bool $value): self
    {
        $this->checkUnique = $value;
        return $this;
    }

    public function isCheckUnique(): bool
    {
        return $this->checkUnique;
    }

    public function getFileMaxSize(): ?int
    {
        return $this->fileMaxSize;
    }

    public function getFileMaxLength(): ?int
    {
        return $this->fileMaxLength;
    }

    public function isOnlyPreview(): bool
    {
        return $this->onlyPreview;
    }

    public function isNoSanitize(): bool
    {
        return $this->noSanitize;
    }

    public function isThumbnail(): bool
    {
        return $this->thumbnail;
    }

    function showOnlyPreview(bool $value): self
    {
        $this->onlyPreview = $value;
        return $this;
    }

    function setNoSanitize(bool $value): self
    {
        $this->noSanitize = $value;
        return $this;
    }

    function isStream(): bool
    {
        return $this->stream;
    }

    function setStream(bool $stream): self
    {
        $this->stream = $stream;
        return $this;
    }

    function isInlineButtonEnabled(): bool
    {
        return $this->inline;
    }

    function enableInlineButton(bool $inline): self
    {
        $this->inline = $inline;
        return $this;
    }

    /**
     * Sets the max size of the file.
     *
     * @param int $maxSize
     * @param string $um Eventuale unità di misura (B, KB, MB, GB)
     * @return FileAttribute
     */
    function setMaxFileSize(int $maxSize, string $um = 'B'): self
    {
        switch ($um) {
            case 'KB':
                $maxSize *= 1024;
                break;
            case 'MB':
                $maxSize *= 1024 * 1024;
                break;
            case 'GB':
                $maxSize *= 1024 * 1024 * 1024;
                break;
        }

        $this->fileMaxSize = $maxSize;
        return $this;
    }

    function getMaxFileSize(): int
    {
        return $this->fileMaxSize ?? Tools::getFileUploadMaxSize();
    }

    function setMaxFileLength($length): self
    {
        $this->fileMaxLength = $length;
        return $this;
    }

    public function getPreviewHeight(): ?string
    {
        return $this->previewHeight;
    }

    public function setPreviewHeight(?string $previewHeight): self
    {
        if (!$previewHeight) {
            $previewHeight = 'auto';
        }
        $this->previewHeight = $previewHeight;
        $this->previewWidth = 'auto';
        return $this;
    }

    function useThumbnail(bool $value): self
    {
        $this->thumbnail = $value;
        return $this;
    }

    public function getPreviewWidth(): ?string
    {
        return $this->previewWidth;
    }

    public function setPreviewWidth(?string $previewWidth): self
    {
        if (!$previewWidth) {
            $previewWidth = 'auto';
        }
        $this->previewWidth = $previewWidth;
        $this->previewHeight = 'auto';
        return $this;
    }

    protected function formatPostfixLabel(): string
    {
        return "";
    }

    public function setFilename(array &$record, string $filename, string $extension)
    {
        $record[$this->fieldName()]['filename'] = "$filename.$extension";
        $record[$this->fieldName()]['orgfilename'] = "$filename.$extension";
    }

    public function getRelativeFilePath(array $record): ?string
    {
        return $record[$this->fieldName()]['filename'] ?? null;
    }

    public function getAbsoluteFilePath(array $record, bool $realPath = false): ?string
    {
        $relativeFilePath = $this->getRelativeFilePath($record);
        if (!$relativeFilePath) {
            return null;
        }
        $fileDir = $this->getDir();
        if (!str_ends_with($fileDir, '/')) {
            $fileDir .= '/';
        }
        $absoluteFilePath = $fileDir . $relativeFilePath;
        return $realPath ? realpath($absoluteFilePath) : $absoluteFilePath;
    }

    public function setThumbnailDir(string $thumbnailDir): self
    {
        $this->thumbnailDir = $thumbnailDir;
        return $this;
    }

    public function setThumbnailExt(string $thumbnailExt): self
    {
        $this->thumbnailExt = $thumbnailExt;
        return $this;
    }

    public function getRelativeThumbnailPath(array $record): ?string
    {
        $realtiveFilePath = $this->getRelativeFilePath($record);
        if (!$realtiveFilePath) {
            return null;
        }

        $relativeThumbnailDir = pathinfo($realtiveFilePath, PATHINFO_DIRNAME) . '/' . $this->thumbnailDir;
        if (!str_ends_with($relativeThumbnailDir, '/')) {
            $relativeThumbnailDir .= '/';
        }

        // per gestire un'eventuale extension diversa dal filename originale
        return $relativeThumbnailDir .
            ($this->thumbnailExt ? (pathinfo($realtiveFilePath, PATHINFO_FILENAME) . '.' . $this->thumbnailExt) : basename($realtiveFilePath));
    }

    public function getAbsoluteThumbnailPath(array $record, bool $realPath = false): ?string
    {
        $relativeThumbnailPath = $this->getRelativeThumbnailPath($record);
        if (!$relativeThumbnailPath) {
            return null;
        }
        $fileDir = $this->getDir();
        if (!str_ends_with($fileDir, '/')) {
            $fileDir .= '/';
        }
        $absoluteThumbnailPath = $fileDir . $relativeThumbnailPath;
        return $realPath ? realpath($absoluteThumbnailPath) : $absoluteThumbnailPath;
    }

    public function getThumbnailUrl(array $record): ?string
    {
        $relativeThumbnailPath = $this->getRelativeThumbnailPath($record);
        if (!$relativeThumbnailPath) {
            return null;
        }
        $fileUrl = $this->getUrl();
        if (!str_ends_with($fileUrl, '/')) {
            $fileUrl .= '/';
        }
        return $fileUrl . $relativeThumbnailPath;
    }

    public function isHideEditWidget(): bool
    {
        return $this->hideEditWidget;
    }

    public function setHideEditWidget(bool $hideEditWidget): self
    {
        $this->hideEditWidget = $hideEditWidget;
        return $this;
    }

    public function isImage(array $record): bool
    {
        $absoluteFilePath = $this->getAbsoluteFilePath($record);
        if (!$absoluteFilePath) {
            return false;
        }

        return (bool)getimagesize($absoluteFilePath);
    }

    public function fileExists(array $record): bool
    {
        $absoluteFilePath = $this->getAbsoluteFilePath($record);
        if (!$absoluteFilePath) {
            return false;
        }
        return is_file($absoluteFilePath);
    }

    public function thumbnailExists(array $record): bool
    {
        $absoluteThumnailPath = $this->getAbsoluteThumbnailPath($record);
        if (!$absoluteThumnailPath) {
            return false;
        }
        return is_file($absoluteThumnailPath);
    }

    /**
     * Call this method only in postAdd() or postUpdate()
     */
    public function isNewFileLoaded(array $record): bool
    {
        return str_contains($record[$this->fieldName()]['tmpfile'], sys_get_temp_dir());
    }

    /**
     * Call this method only postUpdate()
     */
    public function isFileRemoved(array $record): bool
    {
        return $record[$this->fieldName()]['postdel'];
    }

    /**
     * Give the file a uniquely numbered filename.
     *
     * @param array $rec The record for which the file was uploaded
     * @param string $filename The name of the uploaded file
     *
     * @return string The name of the uploaded file, renumbered if necessary
     */
    private function filenameUnique(array $rec, string $filename): string
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
        $db = $owner->getDb();
        $tableSql = $db->escapeSQL($this->m_ownerInstance->getTable());
        $fieldnameSql = $db->escapeSQL($this->fieldName());
        $filenameSql = $db->escapeSQL($filename);
        $nameSql = $db->escapeSQL($name);
        $extSql = $db->escapeSQL($ext);

        $sql = "SELECT `$fieldnameSql` AS filename FROM `$tableSql` WHERE `$fieldnameSql` = '$filenameSql' OR `$fieldnameSql` LIKE '$nameSql-%$extSql'";
        if ($rec[$owner->primaryKeyField()] != '') {
            $sql .= " AND NOT (" . $owner->primaryKey($rec) . ")";
        }

        $records = $db->getRows($sql);

        if (Tools::count($records) > 0) {
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
            $filename = $name . '-' . ($max_count + 1) . $ext;
        }

        Tools::atkdebug('FileAttribute::_filenameUnique() -> New filename = ' . $filename);

        return $filename;
    }

    /**
     * Determine the real filename of a file.
     *
     * If a method <fieldname>_filename exists in the owner instance this method
     * is called with the record and default filename to determine the filename. Else
     * if a file template is set this is used instead and otherwise the default
     * filename is returned.
     *
     * @param array $record The record
     * @param string $default The default filename
     *
     * @return string The real filename
     */
    private function _filenameMangle(array $record, string $default): string
    {
        $method = $this->fieldName() . '_filename';
        if (method_exists($this->m_ownerInstance, $method)) {
            return $this->m_ownerInstance->$method($record, $default);
        } else {
            return $this->filenameMangle($record, $default);
        }
    }
}
