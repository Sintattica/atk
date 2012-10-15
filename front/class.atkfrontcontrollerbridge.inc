<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage front
   *
   * @copyright (c)2007 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6323 $
   * $Id$
   */

  /**
   * Imports
   * @access private
   */
  atkimport('atk.utils.atkdataholder');

  /**
   * Front-end controller bridge. This bridge can be (re-)implemented to
   * support different front-ends then a stand-alone ATK application.
   *
   * @author Tjeerd Bijlsma <tjeerd@ibuildings.nl>
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage front
   */
  class atkFrontControllerBridge
  {
    /**
     * Build url using the given URI and variables.
     *
     * @param array $vars Request vars.
     * @return string url
     */
    public function buildUrl($vars)
    {
      $url = atkSelf().'?'.http_build_query($vars);
      return $url;
    }

    /**
     * Redirect to the given url.
     *
     * @param string $url The URL.
     */
    public function doRedirect($url)
    {
      header('Location: '.$url);
    }

    /**
     * Register stylesheet of the given media type.
     *
     * @param string $file stylesheet filename
     * @param string $media media type (defaults to 'all')
     */
    public function registerStyleSheet($file,$media='all')
    {
      atkinstance('atk.ui.atkpage')->register_style($file,$media);
    }

    /**
     * Register stylesheet code.
     *
     * @param string $code stylesheet code
     */
    public function registerStyleCode($code)
    {
      atkinstance('atk.ui.atkpage')->register_stylecode($code);
    }

    /**
     * Register script file.
     *
     * @param string $file script filename
     */
    public function registerScriptFile($file)
    {
      atkinstance('atk.ui.atkpage')->register_script($file);
    }

    /**
     * Register JavaScript code.
     *
     * @param string $code
     */
    public function registerScriptCode($code)
    {
      atkinstance('atk.ui.atkpage')->register_scriptcode($code);
    }

    /**
     * Get the application root
     *
     * @return string The path to the application root
     */
    public function getApplicationRoot()
    {
      return atkconfig('application_dir');
    }
  }
