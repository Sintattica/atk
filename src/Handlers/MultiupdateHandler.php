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
            // updateRecord() drops a non-primary-key atkprimkey; skip such rows so we
            // never select/update with an empty or unsafe condition.
            if (empty($entry['atkprimkey']) || !$this->isRequestSelectorAllowed($entry['atkprimkey'])) {
                continue;
            }
            $record = $this->getNode()->select($entry['atkprimkey'])->mode('edit')->getFirstRow();
            $record = array_merge($record, $entry);
            $this->getNode()->updateDb($record, true, '', $this->getNode()->m_editableListAttributes);
        }

        $this->getNode()->getDb()->commit();
    }
}
