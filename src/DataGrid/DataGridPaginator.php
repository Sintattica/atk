<?php

namespace Sintattica\Atk\DataGrid;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use SmartyException;

/**
 * The data grid paginator. Can be used to render pagination
 * links for an ATK data grid.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class DataGridPaginator extends DataGridComponent
{
    /**
     * Maximum number of pagination links (excluding
     * the previous and next links).
     *
     * @var int
     */
    protected $m_maxLinks = 10;

    /**
     * Show go to previous and next page links.
     *
     * @var bool
     */
    protected $m_goToPreviousNext = true;

    /**
     * Show go to first and last page links.
     *
     * @var bool
     */
    protected $m_goToFirstLast = false;

    private bool $iconizeLinks;

    /**
     * Constructor.
     *
     * @param DataGrid $grid grid
     * @param array $options component options
     */
    public function __construct($grid, $options = array())
    {
        parent::__construct($grid, $options);
        $this->m_maxLinks = Config::getGlobal('pagelinks', 10);
        $this->m_goToFirstLast = Config::getGlobal('pagelinks_first_last', false);
        $this->m_goToPreviousNext = Config::getGlobal('pagelinks_previous_next', true);
        $this->iconizeLinks = Config::getGlobal('datagrid_iconize_links', true);
    }

    /**
     * Returns an array with pagination links.
     */
    protected function getLinks(): array
    {
        $grid = $this->getGrid();
        $links = [];

        $count = $grid->getCount();
        $limit = $grid->getLimit();
        $offset = $grid->getOffset();

        // no navigation links needed
        if ($limit == 0 || $count <= $limit) {
            return $links;
        }

        // calculate pages, first and last page
        $pages = ceil($count / $limit);
        $current = floor($offset / $limit) + 1;
        $first = $current - floor(($this->m_maxLinks - 1) / 2);
        $last = $current + ceil(($this->m_maxLinks - 1) / 2);

        // fix invalid first page
        if ($first < 1) {
            $first = 1;
            $last = min($pages, $this->m_maxLinks);
        }

        // fix invalid last page
        if ($last > $pages) {
            $first = max(1, $pages - $this->m_maxLinks + 1);
            $last = $pages;
        }

        // go to first link
        if ($this->m_goToFirstLast) {
            if ($current > 1 && $last > $this->m_maxLinks) {
                $title = $grid->text('first');
                $url = $grid->getUpdateCall(array('atkstartat' => 0));
                $links[] = array('type' => 'first', 'call' => $url, 'title' => $title);
            }
        }

        // previous link
        if ($this->m_goToPreviousNext) {
            if ($current > 1) {
                $title = $this->iconizeLinks ? $grid->text('previous') : '';
                $url = $grid->getUpdateCall(array('atkstartat' => $offset - $limit));
                $links[] = array('type' => 'previous', 'call' => $url, 'title' => $title);
            }
        }

        // normal pagination links
        for ($i = $first; $i <= $last; ++$i) {
            if ($i == $current) {
                $links[] = array('type' => 'page', 'title' => $i, 'current' => true);
            } else {
                $title = $i;
                $url = $grid->getUpdateCall(array('atkstartat' => max(0, ($i - 1) * $limit)));
                $links[] = array('type' => 'page', 'call' => $url, 'title' => $title, 'current' => false);
            }
        }

        // next link
        if ($this->m_goToPreviousNext) {
            if ($current < $pages) {
                $title = $grid->text('next');
                $url = $grid->getUpdateCall(array('atkstartat' => $offset + $limit));
                $links[] = array('type' => 'next', 'call' => $url, 'title' => $title);
            }
        }

        // go to last link
        if ($this->m_goToFirstLast) {
            if ($current < $pages && $pages != $last) {
                $title = $grid->text('last');
                $url = $grid->getUpdateCall(array('atkstartat' => ($pages - 1) * $limit));
                $links[] = array('type' => 'last', 'call' => $url, 'title' => $title);
            }
        }

        return $links;
    }

    /**
     * Renders the paginator for the given data grid.
     *
     * @return null|string rendered HTML
     * @throws SmartyException
     */
    public function render(): ?string
    {
        if ($this->getGrid()->isEditing()) {
            return '';
        }

        $links = $this->getLinks();

        if(!Tools::count($links)){
            return '';
        }

        return $this->getUi()->render('dgpaginator.tpl', ['links' => $links, 'iconize_links' => $this->iconizeLinks]);
    }
}
