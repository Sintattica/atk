<?php namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Node;

/**
 * Page builder. Provides a fluent interface to create standardized ATK pages.
 *
 * $node->createPageBuilder()
 *      ->title('...')
 *      ->beginActionBox()
 *        ->formStart('...')
 *        ->content('...')
 *      ->endActionBox()
 *      ->box('...')
 *      ->render();
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage ui
 */
class PageBuilder
{
    protected $m_node = null;
    protected $m_action = null;
    protected $m_record = null;
    protected $m_title = null;
    protected $m_boxes = array();

    /**
     * Constructor.
     *
     * @param Node $node
     */
    public function __construct(Node $node)
    {
        $this->m_node = $node;
        $this->m_action = $node->m_action;
    }

    /**
     * Returns the node.
     *
     * @return Node
     */
    public function getNode()
    {
        return $this->m_node;
    }

    /**
     * Sets the action.
     *
     * @param string $action
     *
     * @return PageBuilder
     */
    public function action($action)
    {
        $this->m_action = $action;
        return $this;
    }

    /**
     * Sets the record (if applicable) for this action.
     *
     * @param array $record
     *
     * @return PageBuilder
     */
    public function record($record)
    {
        $this->m_record = $record;
        return $this;
    }

    /**
     * Sets the page title to the given string.
     *
     * @param string $title
     *
     * @return Page
     */
    public function title($title)
    {
        $this->m_title = $title;
        return $this;
    }

    /**
     * Add box.
     *
     * @param string $content
     * @param string $title
     * @param string $template
     *
     * @return PageBuilder
     */
    public function box($content, $title = null, $template = null)
    {
        $this->m_boxes[] = array('type' => 'box', 'title' => $title, 'content' => $content, 'template' => $template);
        return $this;
    }

    /**
     * Add action box.
     *
     * @param array $params
     * @param string $title
     * @param string $template
     *
     * @return PageBuilder
     */
    public function actionBox($params, $title = null, $template = null)
    {
        $this->m_boxes[] = array('type' => 'action', 'title' => $title, 'params' => $params, 'template' => $template);
        return $this;
    }

    /**
     * Begins building a new action box.
     *
     * @return ActionBoxBuilder
     */
    public function beginActionBox()
    {
        return new ActionBoxBuilder($this);
    }

    /**
     * Renders the page.
     */
    public function render()
    {
        if ($this->m_title == null) {
            $this->m_title = $this->getNode()->actionTitle($this->m_action, $this->m_record);
        }

        $boxes = array();
        foreach ($this->m_boxes as $box) {
            $title = $box['title'];
            if ($title == null) {
                $title = $this->m_title;
            }

            if ($box['type'] == 'action') {
                $params = array_merge(array('title' => $title), $box['params']);
                $content = $this->getNode()->getUi()->renderAction($this->m_action, $params,
                    $this->getNode()->getModule());
            } else {
                $content = $box['content'];
            }

            $boxes[] = $this->getNode()->getUi()->renderBox(array('title' => $title, 'content' => $content),
                $box['template']);
        }

        $this->getNode()->getPage()->setTitle(Tools::atktext('app_shorttitle') . " - " . $this->m_title);

        $content = $this->getNode()->renderActionPage($this->m_title, $boxes);

        $this->getNode()->addStyle('style.css');
        $this->getNode()->getPage()->addContent($content);
        return null;
    }

}
