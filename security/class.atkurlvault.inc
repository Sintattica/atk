<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage security
   *
   * @copyright (c)2007 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Vault for URLs
   *
   * This class is a 'vault' where you can store urls, and check if a given url
   * is in the vault. By adding all urls to the vault that are displayed on screen
   * and checking the url on each request, you can guard an application against
   * unauthorized url manipulation.
   *
   * The vault stores its state in the session, so it works across requests.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage security
   */
  class atkUrlVault
  {
    /**
     * The array containing all urls in the vault.
     * @access private
     * @var array
     */
    var $m_vault;

    /**
     * The maximum capacity of the vault.
     * @access private
     * @var int
     */
    var $m_vaultsize = 100;

    /**
     * Constructor. Do not use directly, atkUrlVault is a singleton.
     * @access private
     * @return atkUrlVault
     */
    function atkUrlVault()
    {
      atkdebug("Creating url vault");
      $sm = &atkGetSessionManager();
      $session = &$sm->getSession();
      if (!isset($session["atkurlvault"])||!is_array($session["atkurlvault"])) $session["atkurlvault"] = array();
      $this->m_vault = &$session["atkurlvault"];
    }

    /**
     * Get an instance of the vault.
     * @static
     * @return atkUrlVault
     */
    function &getInstance()
    {
      static $s_instance = NULL;

      if ($s_instance==NULL)
      {
        $s_instance = new atkUrlVault();
      }

      return $s_instance;
    }

    /**
     * Add a URL to the vault.
     * If there are more URLS in the vault than its capacity, the oldest
     * URLS are purged.
     *
     * @param String $url The url to add to the vault.
     */
    function addUrl($url)
    {
      $this->m_vault[] = md5($url);
      $this->_purge();
    }

    /**
     * Check if an url is in the vault.
     *
     * @param String $url
     * @return True if the url is in the vault. False if not.
     */
    function containsUrl($url)
    {
      return in_array(md5($url), $this->m_vault);
    }

    /**
     * Remove superfluous URLS from the vault.
     * @access private
     */
    function _purge()
    {
      $cnt = count($this->m_vault);
      if ($cnt>$this->m_vaultsize)
      {
        array_splice($this->m_vault, 0, $cnt-$this->m_vaultsize);
      }
    }

    /**
     * Set the size of the vault.
     * The default size is 100. Note that if the vaultsize is decreased,
     * any items in the vault that do not fit the new capacity are purged.
     *
     * @param int $size The new size.
     */
    function setSize($size)
    {
      $this->m_vaultsize = $size;
      $this->_purge(); // cut off
    }

    /**
     * Get the size of the vault.
     *
     * @return int The current capacity
     */
    function getSize()
    {
      return $this->m_vaultsize;
    }

  }

?>