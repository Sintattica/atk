<?php

namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Session\SessionManager;

/**
 * Action box builder. Provides a fluent interface to create standardized
 * ATK action boxes.
 *
 * This class is used/exposed by the PageBuilder class.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @see PageBuilder
 */
class ActionBoxBuilder
{
    /**
     * Page builder.
     *
     * @var PageBuilder
     */
    protected $m_pageBuilder;

    /**
     * Box title.
     *
     * @var string
     */
    protected $m_title = null;

    /**
     * Box template.
     *
     * @var string
     */
    protected $m_template = null;

    /**
     * Action box parameters.
     *
     * @var array
     */
    protected $m_params = [];

    /**
     * Session status.
     *
     * @var int
     */
    protected $m_sessionStatus = SessionManager::SESSION_DEFAULT;

    /**
     * Constructor.
     *
     * @param PageBuilder $pageBuilder page builder
     */
    public function __construct(PageBuilder $pageBuilder)
    {
        $this->m_pageBuilder = $pageBuilder;
        $this->m_params = $pageBuilder->getNode()->getDefaultActionParams(false);

        $this->formStart('<form id="entryform" name="entryform" enctype="multipart/form-data" action="'.Config::getGlobal('dispatcher').'" method="post" onsubmit="return globalSubmit(this,true)">');
    }

    /**
     * Sets the box title.
     *
     * @param string $title title
     *
     * @return ActionBoxBuilder
     */
    public function title($title)
    {
        $this->m_title = $title;

        return $this;
    }

    /**
     * Set form start.
     *
     * @param string $formStart form start
     *
     * @return ActionBoxBuilder
     */
    public function formStart($formStart)
    {
        $this->m_params['formstart'] = $formStart;

        return $this;
    }

    /**
     * Sets the session status.
     *
     * The default session status is SessionManager::SESSION_DEFAULT. If you don't want an
     * automatically appended session form set the session status
     * explicitly to null!
     *
     * @param int $status session status
     *
     * @return ActionBoxBuilder
     */
    public function sessionStatus($status)
    {
        $this->m_sessionStatus = $status;

        return $this;
    }

    /**
     * Set form end.
     *
     * @param string $formEnd form end
     *
     * @return ActionBoxBuilder
     */
    public function formEnd($formEnd)
    {
        $this->m_params['formend'] = $formEnd;

        return $this;
    }

    /**
     * Template.
     *
     * @param string $template template name
     *
     * @return ActionBoxBuilder
     */
    public function template($template)
    {
        $this->m_template = $template;

        return $this;
    }

    /**
     * Set content.
     *
     * @param string $content content
     *
     * @return ActionBoxBuilder
     */
    public function content($content)
    {
        $this->m_params['content'] = $content;

        return $this;
    }

    /**
     * Set form buttons.
     *
     * @param string $buttons form buttons
     *
     * @return ActionBoxBuilder
     */
    public function buttons($buttons)
    {
        $this->m_params['buttons'] = $buttons;

        return $this;
    }

    /**
     * Stops building the action box and returns the page builder.
     *
     * @return PageBuilder
     */
    public function endActionBox()
    {
        if ($this->m_sessionStatus !== null) {
            $sm = SessionManager::getInstance();
            $this->m_params['formend'] = $sm->formState($this->m_sessionStatus).$this->m_params['formend'];
        }

        $this->m_pageBuilder->actionBox($this->m_params, $this->m_title, $this->m_template);

        return $this->m_pageBuilder;
    }
}
