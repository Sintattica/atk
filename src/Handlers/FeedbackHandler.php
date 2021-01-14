<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Tools;

/**
 * Handler class for the feedback action of a node. The handler draws a
 * screen with a message, giving the user feedback on some action that
 * occurred.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class FeedbackHandler extends ActionHandler
{
    /**
     * The action handler method.
     */
    public function action_feedback()
    {
        $page = $this->getPage();
        $output = $this->invoke('feedbackPage', $this->m_postvars['atkfbaction'], $this->m_postvars['atkactionstatus'], $this->m_postvars['atkfbmessage']);
        $page->addContent($this->m_node->renderActionPage('feedback', $output));
    }

    /**
     * The method returns a complete html page containing the feedback info.
     *
     * @param string $action The action for which feedback is provided
     * @param int $actionstatus The status of the action for which feedback is
     *                             provided
     * @param string $message An optional message to display in addition to the
     *                             default feedback information message.
     *
     * @return string The feedback page as an html String.
     */
    public function feedbackPage($action, $actionstatus, $message = '')
    {
        $node = $this->m_node;
        $ui = $this->getUi();

        $params['content'] = '<br>'.Tools::atktext('feedback_'.$action.'_'.Tools::atkActionStatus($actionstatus), $node->m_module, $node->m_type);
        if ($message) {
            $params['content'] .= ' <br>'.$message;
        }

        $sm = SessionManager::getInstance();

        if ($sm->atkLevel() > 0) {
            $params['formstart'] = '<form method="get">'.$sm->formState(SessionManager::SESSION_BACK);
            $params['buttons'][] = '<input type="submit" class="btn btn-sm btn-default btn_cancel" value="&lt;&lt; '.Tools::atktext('back').'">';
            $params['formend'] = '</form>';
        }

        $output = $ui->renderAction($action, $params);

        return $ui->renderBox(array(
            'title' => $node->actionTitle($action),
            'content' => $output,
        ));
    }
}
