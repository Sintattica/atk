<?php namespace Sintattica\Atk\Handlers;
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage handlers
 *
 * @copyright (c) 2000-2009 Ibuildings.nl BV
 *
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 */
class Atk_MultiUpdatehandler extends Atk_ActionHandler
{

    /**
     * The action handler method.
     */
    function action_multiupdate()
    {
        $data = $this->getNode()->m_postvars['atkdatagriddata'];
        foreach ($data as $entry) {
            $entry = $this->getNode()->updateRecord($entry, $this->getNode()->m_editableListAttributes);
            $record = $this->getNode()->select($entry['atkprimkey'])->mode('edit')->firstRow();
            $record = array_merge($record, $entry);
            $this->getNode()->updateDb($record, true, '', $this->getNode()->m_editableListAttributes);
        }

        $this->getNode()->getDb()->commit();
    }
}
