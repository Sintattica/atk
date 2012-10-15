<?php

/**
 * Session storage singleton, given a key (or a key in the session)
 * stores records to the current session
 */
class atkSessionStore
{
  /**
   * Instances of the session store, indexed by key
   *
   * @var array
   */
  private static $_instances = array();

  /**
   * Key to use
   *
   * @var mixed
   */
  private $_key;

  /**
   * Get the current instance for the session storage
   *
   * @param mixed $key   Key to use
   * @param bool  $reset Wether to reset the singleton
   * @return atkSessionStore Storage
   */
  public static function getInstance($key=false, $reset=false)
  {
    if (!$key) $key = self::getKeyFromSession();
    if (!isset(self::$_instances[$key]) || $reset) self::$_instances[$key] = new self($key);
    return self::$_instances[$key];
  }

  /**
   * Try to get the current key from the session
   *
   * @return mixed Key to use, false if we don't have a key
   */
  private static function getKeyFromSession()
  {
    $sessionmanager = self::getSessionManager();
    if (!$sessionmanager) return false;
    else                  return $sessionmanager->globalStackVar("atkstore_key");
  }

  /**
   * Create a new sessionstore
   *
   * @param mixed $key Key to use
   */
  private function __construct($key)
  {
    $this->_key = $key;
  }

  /**
   * Get the key for the current sessionstore
   *
   * @return mixed Key
   */
  public function getKey() { return $this->_key; }

  /**
   * Add a row to the current sessionstore.
   *
   * Also sets the primary key field to a fake negative id
   *
   * @param array  $row               Row to store in the session
   * @param string $primary_key_field Primary key field to use and set with the row key
   * @return mixed Primary key for the added record, or false if we don't have a session
   */
  public function addDataRow($row, $primary_key_field)
  {
    atk_var_dump($row, __CLASS__.'->'.__METHOD__.": Adding a new row to session store with primary key field '$primary_key_field' and key: ".$this->getKey());
    $data = $this->getData();
    if ($data===false) return false;

    $primary_key = -1 * count($data);
    $row[$primary_key_field] = $primary_key;
    $data[] = $row;

    $this->setData($data);

    return $primary_key;
  }

  /**
   * Get a data row from the session for an ATK/SQL selector
   *
   * @param string $selector
   * @return mixed Row or false if there is nothing
   */
  public function getDataRowForSelector($selector)
  {
    atk_var_dump($selector, __CLASS__.'->'.__METHOD__.": Getting row from session store with key: ".$this->getKey());
    $data = $this->getData();
    if (!$data) return false;

    $row_key = self::getRowKeyFromSelector($selector);
    if ($row_key===false) return false;

    return $data[$row_key];
  }

  /**
   * Update (set) a row in the session for an ATK/SQL selector
   *
   * @param string $selector ATK/SQL selector
   * @param array  $row      New row
   * @return mixed Updated row or false if updating failed
   */
  public function updateDataRowForSelector($selector, $row)
  {
    atk_var_dump($row, __CLASS__.'->'.__METHOD__.": Updating row in session store with key: ".$this->getKey()." and selector: $selector");
    $data = $this->getData();
    if (!$data) return false;

    $row_key = self::getRowKeyFromSelector($selector);
    if ($row_key===false) return false;

    $data[$row_key] = $row;

    $this->setData($data);
    return $row;
  }

  /**
   * Delete a row in the session for a given ATK/SQL selector
   *
   * @param string $selector ATK/SQL selector
   * @return bool Wether the deleting succeeded
   */
  public function deleteDataRowForSelector($selector)
  {
    atk_var_dump($selector, __CLASS__.'->'.__METHOD__.": Deleting row from session store with key: ".$this->getKey());
    $data = $this->getData();
    if (!$data) return false;

    $row_key = self::getRowKeyFromSelector($selector);
    if ($row_key===false) return false;

    unset($data[$row_key]);

    $this->setData($data);
    return true;
  }

  /**
   * Get the sessionmanager to use
   *
   * @return mixed Sessionmanager or false if we don't have a session
   */
  protected static function getSessionManager()
  {
    $sessionmanager = atkGetSessionManager();
    if (!$sessionmanager) return false;
    else                  return $sessionmanager;
  }

  /**
   * Get all the data in the session for the current key
   *
   * @return mixed Data in array form or false if we don't have a key or session
   */
  public function getData()
  {
    if (!$this->_key) return false;

    $sessionmanager = self::getSessionManager();
    if (!$sessionmanager) return false;

    $data = $sessionmanager->globalStackVar($this->_key);
    if (!is_array($data)) $data = array();
    return $data;
  }

  /**
   * Set ALL data in the session for the current key
   *
   * @param array $data Data to set
   * @return mixed Data that was set or false if we don't have a key or session
   */
  private function setData($data)
  {
    if (!$this->_key) return false;

    $sessionmanager = self::getSessionManager();
    if (!$sessionmanager) return false;

    $sessionmanager->globalStackVar($this->_key, $data);
    return $data;
  }

  /**
   * Get rowkey from an ATK/SQL selector
   *
   * We sneak rowkeys in the selectors as negative ids.
   *
   * @param string $selector
   * @return mixed Key in negative int form or false if we failed to get the key
   */
  private static function getRowKeyFromSelector($selector)
  {
    $selector = decodeKeyValuePair($selector);
    $selector_values = array_values($selector);

    if (count($selector_values)===1 && is_numeric($selector_values[0]) && $selector_values[0]<=0)
    {
      return -1*$selector_values[0];
    }
    return false;
  }
}