<?php
  /**
   * The OTM Session Grid Handler
   * Provides the OTM grid with data from the session,
   * when the OTM is in add mode.
   */
  class atkOneToManyRelationSessionGridHandler
  {
    private $_key;

    /**
     * Create a OTM session grid handler
     *
     * @param string $key
     */
    public function __construct($key)
    {
      $this->_key = $key;
    }

    /**
     * Select handler, returns the records for the grid
     *
     * @param atkDataGrid $grid
     * @return array Records for the grid
     */
    public function selectHandlerForAdd(atkDataGrid $grid)
    {
      $records = $this->getRecordsFromSession();

      $limit = $grid->getLimit();
      $offset = $grid->getOffset();
      $records_count = count($records);

      // If we don't need to limit the result, then we don't
      if ((int)$offset===0 && $limit>=$records_count)
      {
        // We have to sort the data first, because the datagrid
        // is very sensitive with regards to it's numerical keys
        // being sequential
        sort($records);
        return $records;
      }

      // Limit the search results and return the limited results
      $ret = array();
      $records_keys = array_keys($records);
      for ($i=$offset,$j=0;$i<$records_count && $j<$limit;$i++,$j++)
      {
        $ret[] = $records[$records_keys[$i]];
      }
      return $ret;
    }

    /**
     * Count handler, return the number of records there are in the session
     *
     * @return int
     */
    public function countHandlerForAdd()
    {
      return count($this->getRecordsFromSession());
    }

    /**
     * Get all records for the current key from the session
     *
     * @return array
     */
    private function getRecordsFromSession()
    {
      atkimport('atk.session.atksessionstore');
      return atkSessionStore::getInstance($this->_key)->getData();
    }
  }
