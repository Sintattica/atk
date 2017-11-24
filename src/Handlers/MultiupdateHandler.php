<?php

namespace Sintattica\Atk\Handlers;

class MultiupdateHandler extends ActionHandler
{
    /**
     * The action handler method.
     */
    public function action_multiupdate()
    {
        $data = $this->getNode()->m_postvars['atkdatagriddata'];
        foreach ($data as $entry) {
            $entry = $this->getNode()->updateRecord($entry, $this->getNode()->m_editableListAttributes);
            $record = $this->getNode()->select($entry['atkprimkey'])->mode('edit')->getFirstRow();
            $record = array_merge($record, $entry);
            $this->getNode()->updateDb($record, true, '', $this->getNode()->m_editableListAttributes);
        }

        $this->getNode()->getDb()->commit();
    }
}
