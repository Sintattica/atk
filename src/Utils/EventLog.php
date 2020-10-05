<?php

namespace Sintattica\Atk\Utils;

use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Core\Config;

/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 *
 * @copyright (c)2005 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 2955 $
 * $Id$
 */

/**
 * The EventLog is a ready to use ActionListener for logging events
 * in a table.
 *
 * You can use the atkEventLog by adding an instance to a node using
 * Node's addListener() method.
 *
 * In order to use the atkEventLog, you have to have a table in the database
 * named 'atkeventlog' with the following structure:
 *
 * CREATE TABLE atkeventlog
 * (
 *   id INT(10),
 *   userid INT(10),
 *   stamp DATETIME,
 *   node VARCHAR(100),
 *   action VARCHAR(100),
 *   primarykey VARCHAR(255)
 * }
 *
 * The current implementation only supports the logging.
 *
 * @todo Add visualisation of the log.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class EventLog extends ActionListener
{
    /**
     * This method handles the storage of the action in the database.
     *
     * @param string $action The action being performed
     * @param array $record The record on which the action is performed
     */
    public function actionPerformed($action, $record)
    {
        $user = SecurityManager::atkGetUser();
        $userid = $user[Config::getGlobal('auth_userpk')];
        if ($userid == '') {
            $userid = 0;
        } // probably administrator
        $db = $this->m_node->getDb();
        $query = $db->createQuery('atkeventlog');
        $query->addFields([
            'userid' => $userid,
            'stamp' => date('Y-m-d H:i:s'),
            'node' => $this->m_node->atkNodeUri(),
            'action' => $action,
            'primarykey' => $this->m_node->primaryKeyString($record)
        ]);
        $query->executeInsert();

        $db->commit();
    }
}
