<?php

namespace Sintattica\Atk\Core;

use Exception;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\TextAttribute;
use Sintattica\Atk\Handlers\EditHandler;
use Sintattica\Atk\Handlers\DeleteHandler;
use Sintattica\Atk\Utils\Selector;

/**
 * File editing node.
 *
 * This is a special derivative Node that does not have database
 * interaction, but that can be used to edit files in a directory on the
 * server.
 *
 * Note: This class does not support postAdd, postUpdate and postDelete
 * hooks. Other overrides may or me not be supported, but this has not been
 * tested.
 *
 * Derived classes need not add attributes. The only thing to specify in
 * derived classes is the baseclass constructor, which can be configured
 * with parameters. (See constuctor documentation)
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class FileEditor extends Node
{
    public $m_dir;
    public $m_basedir;
    public $m_filefilter;
    public $m_showdirs;

    /**
     * Constructor.
     *
     * This function is called when a new atkFileEditor is instantiated.
     *
     * @param string $nodeUri The name of the node.
     * @param string $dir The directory that the fileeditor lists. If you
     *                       want to enable addition of new files, make sure
     *                       that the webserver has write access to this dir.
     *                       Only regular files (not subdirs or special files)
     *                       are listed.
     * @param string $filter A regular expression that is used to filter which
     *                       files will be shown. Example: '.txt$' lists only
     *                       txt files in the directory.
     *                       Note: This parameter is also used to validate the
     *                       filename of newly created files. This means, you
     *                       cannot create a new file called test.css if your
     *                       filter param is set to '.tpl$';
     *                       Note 2: Watch out when using $ in your regular
     *                       expression; PHP parses this, so use single quotes
     *                       or escape the dollarsign with \.
     * @param int $flags The node flags. See Node for a list of possible
     *                       flags.
     */
    public function __construct($nodeUri, $dir = '', $filter = '', $flags = 0)
    {
        parent::__construct($nodeUri, $flags | Node::NF_ADD_LINK);
        $this->m_dir = $dir;
        $this->m_basedir = $dir;
        if ($dir == '') {
            $this->m_dir = './';
        }

        // dir must have a trailing slash.
        if (substr($this->m_dir, -1) != '/') {
            $this->m_dir .= '/';
        }

        $this->m_filefilter = $filter;
        $this->m_showdirs = true;

        $this->add(new Attribute('filename', Attribute::AF_PRIMARY | Attribute::AF_SEARCHABLE));
        $this->add(new TextAttribute('filecontent',Attribute::AF_HIDE_LIST));

        $this->addSecurityMap('dirchange', 'admin');

        $this->setOrder('dummy.filename');
        $this->setTable('dummy');
    }

    /**
     * This function is used to change the node directory.
     *
     * @param string $dir The name of the dir to change to
     */
    public function setDir($dir)
    {
        $this->m_dir = $dir;
    }

    /**
     * This function reads the contents of the directory.
     *
     * @param string $selector The selected item in the directory list
     *
     * @return int The number of element of the array containing the directory and file names
     *               from the currently selected directory
     */
    public function count($selector)
    {
        $d = dir($this->m_dir);
        $arr = [];
        while (false !== ($entry = $d->read())) {
            $this->addFileEntry($entry, $arr);
        }
        $d->close();

        return count($arr);
    }

    /**
     * This function adds a file or directory to the list in the node.
     *
     * @param string $entry The directory that is added to the list
     * @param array $arr The array containing the result of the
     *                      added item
     */
    public function addFileEntry($entry, &$arr)
    {
        // First we check if we are about to search and set a few things
        // if we are
        $searching = false;

        /* there may be search criteria, which we also filter */
        $searchArray = @$this->m_postvars['atksearch'];
        if (isset($searchArray['filename']) && $searchArray['filename'] != '') {
            $searching = true;
        }

        // list only regular files or directories
        if (is_file($this->m_dir . $entry) && ($this->m_filefilter == '' || preg_match($this->m_filefilter, $entry))) {
            if ($searching === true && (preg_match($searchArray['filename'], $entry))) {
                $arr[] = $entry;
            } elseif (!$searching) {
                $arr[] = $entry;
            }
        } elseif (is_dir($this->m_dir . $entry)) {
            if (!($entry == '.' || $entry == 'CVS' || $entry == '.svn')) {
                if (!($this->stripDir($this->m_basedir) == $this->stripDir($this->m_dir) && $entry == '..')) {
                    $arr[] = $entry;
                }
            }
        }
    }

    /**
     * This function sets the actions of the items in the list.
     *
     * @param array $record Identifier for the record
     * @param array $actions Result array containing the options
     * @param array $mraactions
     */
    public function recordActions(array $record, array &$actions, array &$mraactions): void
    {
        $this->m_dir = $this->stripDir($this->m_dir);
        if (is_dir($this->m_dir . '/' . $record['filename'])) {
            $actions['view'] = Tools::dispatch_url($this->atkNodeUri(), 'dirchange', [Node::PARAM_ATKSELECTOR => $this->m_dir . $record['filename']]);
            unset($actions['edit']);
            unset($actions['delete']);

            return;
        }

        // Remove edit/delete actions when a file is not writeable.
        if (!is_writeable($this->m_dir . '/' . $record['filename'])) {
            unset($actions['edit']);
            unset($actions['delete']);
        }
    }

    /**
     * This function loops through the items in a directory and
     * and calls functions to print the results.
     *
     * @param string|null $condition Identifier for the selected item
     * @param array $params The list of items is ordered by the item type
     *                         mentioned in this variable
     *
     * @return array|Selector
     */
    public function select(string|null $condition = '', array $params = array()): array|Selector
    {
        $res = [];
        SessionManager::getInstance()->stackVar('dirname', $this->m_dir);
        if ($condition == '') {
            // no file selected, generate list..
            $start = 0;
            $max = -1; // no max

            if (is_array($params) && Tools::count($params) == 2) {
                $start = $params['offset'];
                $max = $params['params'];
            }

            $d = dir($this->m_dir);
            if ($d->handle) {
                $arr = [];
                while (false !== ($entry = $d->read())) {
                    if ($this->m_showdirs || !is_dir($d->path . DIRECTORY_SEPARATOR . $entry)) {
                        $this->addFileEntry($entry, $arr);
                    }
                }
                $d->close();

                if (Tools::count($arr) > 0) {
                    if ($params == 'dummy.filename DESC') {
                        rsort($arr);
                    } else {
                        sort($arr);
                    }
                }

                $res = [];

                for ($i = 0; $i < Tools::count($arr); ++$i) {
                    if ($i >= $start && ($max == -1 || Tools::count($res) < $max)) {
                        $res[] = array('filename' => $arr[$i]);
                    }
                }
            } else {
                Tools::atkdebug('Dir ' . $this->m_dir . ' could not be read');
            }
        } else {
            // file selected, read file.
            // in the fileeditor, the selector is always dummy.filename=name
            // so we use the value of the decoded pair as a filename.
            $decodedselector = Tools::decodeKeyValuePair($condition);
            $filename = $decodedselector['dummy.filename'];
            $record['filename'] = $filename;

            // we must store original filename as primaryKey, for
            // atknode uses the value in some places.
            $record['atkprimkey'] = $this->primaryKey($record);
            if (is_file($this->m_dir . $filename)) {
                $record['filecontent'] = implode('', file($this->m_dir . $filename));
            } else {
                Tools::atkdebug("File $filename not found");
            }
            $res[] = $record;
        }

        return $res;
    }

    /**
     * This function controls actions on the selected file is allowed.
     *
     * @param array $record Array that contains the identifier of the record
     * @param string $mode The mode we're in
     */
    public function validate(array &$record, string $mode, array $ignoreList = []): bool
    {
        if (!preg_match($this->m_filefilter, $record['filename'])) {
            Tools::triggerError($record, 'filename', 'filename_invalid');
        } else {
            if ($mode == 'add' && file_exists($this->m_dir . $record['filename'])) {
                Tools::triggerError($record, 'filename', 'file_exists');
            }
        }
        return true;
    }

    /**
     * This function prints the current directory at the top of the list.
     *
     * @return string The text containing the directory name
     */
    public function adminHeader(): string
    {
        return '<p><b>' . $this->text('current_dir') . ': ' . substr_replace($this->m_dir, '', 0, strlen($this->m_basedir)) . '</b></p>';
    }

    /**
     * This function overrides the addDb function to add
     * a file to the selected directory.
     *
     * @param array $record Array that contains the name of the new file
     *
     * @return bool The result of the file addition
     * @throws Exception
     */
    public function addDb(array &$record, bool $exectrigger = true, string $mode = 'add', array $excludelist = []): bool
    {
        $sessmngr = SessionManager::getInstance();
        $this->m_dir = $this->stripDir($sessmngr->stackVar('dirname'));
        $fp = @fopen($this->m_dir . $record['filename'], 'wb');
        if ($fp == null) {
            Tools::atkerror('Unable to open file ' . $record['filename'] . " for writing. (Is directory '" . $this->m_dir . "' readable by webserver?");

            return false;
        } else {
            fwrite($fp, $record['filecontent']);
            fclose($fp);
            Tools::atkdebug('Wrote ' . $record['filename']);
        }

        return true;
    }

    /**
     * This function overrides the updateDb function to update
     * the contents of a file.
     *
     * @param array $record Array that contains the name of the file that
     *                      is updated
     *
     * @return bool The result of the file update
     * @throws Exception
     */
    public function updateDb(array &$record, bool $exectrigger = true, array|string $excludes = '', array|string $includes = ''): bool
    {
        // The record that must be updated is indicated by 'atkorgkey'
        // (not by atkselector, since the primary key might have
        // changed, so we use the atkorgkey, which is the value before
        // any update happened.)
        $sessmngr = SessionManager::getInstance();
        $this->m_dir = $this->stripDir($sessmngr->stackVar('dirname'));

        if ($record['atkprimkey'] != '') {
            if ($record['atkprimkey'] != $this->primaryKey($record)) {
                $decodedprimkey = Tools::decodeKeyValuePair($record['atkprimkey']);
                $filename = $this->m_dir . $decodedprimkey['dummy.filename'];

                unlink($filename);
                Tools::atkdebug("Filename changed. Deleted original '$filename'.");
            }
            $fp = @fopen($this->m_dir . $record['filename'], 'wb');
            if ($fp == null) {
                Tools::atkerror('Unable to open file ' . $record['filename'] . " for writing. (Is directory '" . $this->m_dir . "' readable by webserver?");
            } else {
                fwrite($fp, $record['filecontent']);
                fclose($fp);
                Tools::atkdebug('Wrote ' . $record['filename']);
                $record['atkprimkey'] = $record['filename'];
            }

            return true;

        } else {
            Tools::atkdebug('NOT UPDATING! NO SELECTOR SET!');

            return false;
        }
    }

    /**
     * This function overrides the deleteDb function to delete a file
     * from the selected directory.
     *
     * @param string $selector The identifier of the file that should be deleted
     *
     * @return bool The result of the file deletion
     * @throws Exception
     */
    public function deleteDb(string $selector, bool $exectrigger = true, bool $failwhenempty = false): bool
    {
        $sessmngr = SessionManager::getInstance();
        $this->m_dir = $this->stripDir($sessmngr->stackVar('dirname'));
        $decodedselector = Tools::decodeKeyValuePair($selector);
        $filename = $decodedselector['dummy.filename'];

        Tools::atk_var_dump($this->m_dir, 'm_dir');
        Tools::atk_var_dump($filename, 'filename');

        if (strpos($filename, '..') === false) {
            unlink($this->m_dir . $filename);
            Tools::atkdebug('Deleted ' . $this->m_dir . $filename);
        } else {
            Tools::atkerror('Cannot unlink relative files. Possible hack attempt detected!');
        }

        return true;
    }

    /**
     * This function overrides the edit action to first set the directory
     * before actually editing a file.
     *
     * @param EditHandler $handler
     */
    public function action_edit(EditHandler $handler): void
    {
        $this->m_dir = SessionManager::getInstance()->stackVar('dirname');
        $handler->action_edit();
    }

    /**
     * This function overrides the delete action to set the directory
     * before actually deleting a file.
     *
     * @param DeleteHandler $handler
     */
    public function action_delete(DeleteHandler $handler): void
    {
        $this->m_dir = SessionManager::getInstance()->stackVar('dirname');
        $handler->action_delete();
    }

    /**
     * This function implements the functionality to move up
     * and down directories.
     */
    public function action_dirchange(): void
    {
        $selectedDir = $this->stripDir($this->m_postvars[Node::PARAM_ATKSELECTOR]);
        SessionManager::getInstance()->stackVar('dirname', $selectedDir);

        $this->m_dir = $selectedDir;
        $this->callHandler('admin');
    }

    /**
     * This function strips a given directory to a valid relative path.
     *
     * @param string $dirname Path of the dir to change to
     * @return string Stripped directory path
     */
    public function stripDir(string $dirname): string
    {
        // normalizes the given string to a relative dir that should always start with the base directory
        if (strpos(realpath($dirname), realpath($this->m_basedir)) === 0) {
            $resultdir = rtrim(str_replace(realpath($this->m_basedir), $this->m_basedir, realpath($dirname)), '/') . '/';
            if ($resultdir == '' || !is_dir($resultdir)) {
                $resultdir = rtrim($this->m_basedir, '/') . '/';
            }
        } else {
            $resultdir = rtrim($this->m_basedir, '/') . '/';
        }

        return $resultdir;
    }

    /**
     * Show or hide the subdirectories.
     *
     * @param bool $bool
     */
    public function showDirs(bool $bool): void
    {
        $this->m_showdirs = (bool)$bool;
    }
}
