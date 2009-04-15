<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage datatypes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6318 $
   * $Id$
   */
  atkimport('atk.datatypes.atkdatatype');

  /**
   * The 'time' datatype.
   * Useful for calculating with time in different formats.
   *
   * Accepts ATK time format (an array) and timestamps.
   * Created for easily and fluently calculating durations.
   *
   * @deprecated Scheduled for removal.
   * @author Boy Baukema <boy@achievo.org>
   * @package atk
   * @subpackage datatypes
   */
  class atkTime extends atkDataType
  {
    /**
     * @var atkString The template to display the time with
     *                defaults to [hours]:[minutes]:[seconds]
     */
    private $m_template="";

    /**
     * @var int The time in UNIX timestamp format
     */
    private $m_time=0;

    /**
     * @var string The date format in PHP format
     *             defaults to H:i:s
     */
    private $m_format='H:i:s';

    /***************** BASICS *****************/

    /**
     * Create an atkTime datatype, accepts ATK internal format
     * (an array) or timestamps.
     *
     * @param array|int $time The time for the datatype, for example:
     *                        new atkTime(3600) (one hour) or:
     *                        new atkTime($record['begintime'])
     */
    public function __construct($time=0)
    {
      $this->setFormat('H:i:s');
      $this->setTemplate('[hours]:[minutes]:[seconds]');
      if (is_array($time))
      {
        $this->m_time = gmmktime($time['hours'],$time['minutes'],
                                 $time['seconds'],1,1,1970);
      }
      else if (is_numeric($time)) $this->m_time = $time;
      else atkerror("Unknown time format! Called with: ".
                    "new atkTime($time)");
    }

    /**
     * Factory method to create a new atkTime object
     *
     * @param mixed $time The time for the atkTime object
     * @return atkTime The new atkTime object
     */
    public function create($time)
    {
      return new atkTime($time);
    }

    /**
     * Get the time
     *
     * @return int The time
     */
    protected function getData()
    {
      return $this->m_time;
    }

    /***************** OPERATIONS *****************/

    /**
     * Add time to the current time object.
     *
     * @param atkTime $add The time to add
     * @return atkTime The current (modified) time object
     */
    public function add(atkTime $add)
    {
      $this->m_time+=$add->getTotalSeconds();
      return $this;
    }

    /**
     * Subtract time from the current time object
     *
     * @param atkTime $subtract The time to subtract
     * @return atkTime The current (modified) time object
     */
    public function subtract(atkTime $subtract)
    {
      $this->m_time-=$subtract->getTotalSeconds();
      return $this;
    }

    /***************** SETTERS *****************/

    /**
     * Set a description template.
     *
     * Description templates can be used to parse data in the string
     * the same way you do with the descriptor_def method of your node.
     *
     * @param string $template The template for the description
     * @return atkTime The current (modified) time object
     */
    public function setTemplate($template)
    {
      $this->m_template= atknew('atk.datatypes.atkstringdt',$template);
      return $this;
    }

    /**
     * Set the display format (PHP date style)
     *
     * @param string $format The display format (PHP date style)
     * @return atkTime
     */
    public function setFormat($format)
    {
      $this->m_format = $format;
      return $this;
    }

    /***************** GETTERS *****************/

    /**
     * Gets the time for the current time object in array (ATK) format.
     *
     * @param bool $leadingzero Wether or not to use a leading zero
     *                          in the hours, minutes and seconds
     *                          when applicable.
     * @return array The array with the time in array format
     */
    public function getTimeArray($leadingzero=true)
    {
      return array('hours'=>  $this->getHours($leadingzero),
                   'minutes'=>$this->getMinutes($leadingzero),
                   'seconds'=>$this->getSeconds($leadingzero));
    }

    /**
     * Gets the hours of the current time object.
     *
     * example: atkTime::create(3677)->getHours() returns 1
     * example: atkTime::create(3680)->getHours(true) returns "01"
     *
     * @param bool $leadingzero Wether or not to use a leading zero
     *                          when applicable.
     * @return string|int The hours for the current time object
     */
    public function getHours($leadingzero=false)
    {
      $hours = (int)($this->m_time/3600);
      if ($leadingzero && $hours<10) $hours="0$hours";
      return $hours;
    }

    /**
     * Gets the minutes of the current time object
     *
     * example: atkTime::create(126)->getMinutes() returns 2
     * example: atkTime::create(126)->getMinutes(true) returns "02"
     *
     * @param bool $leadingzero Wether or not to use a leading zero
     *                          when applicable.
     * @return string|int The
     */
    public function getMinutes($leadingzero=false)
    {
      $minutes = (int)(($this->m_time-($this->getHours()*3600))/60);
      if ($leadingzero && $minutes<10) $minutes="0$minutes";
      return $minutes;
    }

    /**
     * Gets the seconds for the current time object.
     *
     * example: atkTime::create(45)->getSeconds() returns 45
     * example: atkTime::create(5)->getSeconds(true) returns "05"
     *
     * @param bool $leadingzero Wether or not to use a leading zero
     *                          when applicable.
     * @return string|int The seconds of the time.
     */
    public function getSeconds($leadingzero=false)
    {
      $seconds = (int)($this->m_time-($this->getHours()*3600) -
                                ($this->getMinutes()*60));
      if ($leadingzero && $seconds<10) $seconds="0$seconds";
      return $seconds;
    }

    /**
     * Get the total amount of minutes for the current time object
     *
     * example: 2 hours would return 120
     *
     * @return int The total minutes
     */
    public function getTotalMinutes()
    {
      return (int)($this->m_time/60);
    }

    /**
     * Gets the total amount of seconds for the current time object.
     *
     * example: 5 minutes would return 300
     *
     * @return int The total seconds
     */
    public function getTotalSeconds()
    {
      return (int)$this->m_time;
    }

    /**
     * Get the description for the current time object.
     *
     * example: atkTime::create(3666)->getDescription(
     *            '[hours] hours and [minutes] minutes');
     *
     * @param string $template The template to use, if none is passed
     *                         it will use the template set on the
     *                         object by setTemplate.
     * @return string The description for the current time object.
     */
    public function getDescription($template='')
    {
      return $this->m_template->parse($this->getTimeArray())
                              ->getString();
    }

    /**
     * Get the formatted time for the current time object
     * (PHP date style).
     *
     * @param string $format The format to use, if none is passed
     *                       it will use the format set on the
     *                       object by setFormat.
     * @return string The formatted time
     */
    public function getFormatted($format='')
    {
      $format = ($format?$format:$this->m_format);
      atkdebug("formatting time: {$this->m_time} with format $format");
      return date($format,$this->m_time);
    }

    /**
     * Get the difference between the current time object and
     * another time object as a time object.
     *
     * example: atkTime::create(3600)->getDiff(new atkTime(600)) will
     *          return a time object with 3000 seconds or 50 minutes.
     *
     * @param atkTime $compare The time object to compare against.
     * @return atkTime The difference between the current time object
     *                 and the passed time object, as a time object.
     */
    public function getDiff(atkTime $compare)
    {
      return new atkTime((int)($this->m_time -
                               $compare->getTotalSeconds()));
    }

    /***************** QUERIES *****************/

    /**
     * Wether or not the time object passed is less than the current
     * time object.
     *
     * @param atkTime $compare The time object to compare against.
     * @return bool Wether the passed time is less
     */
    public function isLesserThan(atkTime $compare)
    {
      return ($this->m_time<$compare->getTotalSeconds());
    }

    /**
     * Wether or not the time object passed is more than the current
     * time object.
     *
     * @param atkTime $compare The time object to compare against.
     * @return bool Wether the passed time is more
     */
    public function isGreaterThan(atkTime $compare)
    {
      return ($this->m_time>$compare->getTotalSeconds());
    }

    /**
     * Wether or not the time object passed is equal to the current
     * time object.
     *
     * @param atkTime $compare The time object to compare against.
     * @return bool Wether the passed time is equal
     */
    public function isEqualTo(atkTime $compare)
    {
      return ($this->m_time===$compare->getTotalSeconds());
    }

    /**
     * Wether the current time is a negative value.
     *
     * @return bool Wether the current time is a negative value.
     */
    public function isNegative()
    {
      return ($this->m_time>0);
    }

    /**
     * Wether the current time equals zero.
     *
     * @return bool Wether the current time equals zero.
     */
    public function isZero()
    {
      return ($this->m_time===0);
    }
  }

?>
