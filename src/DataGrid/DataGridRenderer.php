<?php

namespace Sintattica\Atk\DataGrid;

use Sintattica\Atk\Utils\Json;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

/**
 * The grid renderer is responsible for rendering the grid components and
 * ofcourse the grid itself.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class DataGridRenderer extends DataGridComponent
{
    /**
     * Surrounds the grid by a container if we are rendering the grid for the
     * first time (e.g. if this is not an update of the grid contents).
     *
     * @param string $result grid HTML
     *
     * @return string grid HTML
     */
    protected function renderContainer($result)
    {
        if (!$this->getGrid()->isUpdate()) {
            $result = '<div id="'.Tools::htmlName($this->getGrid()->getName()).'_container" class="atkdatagrid-container">'.$result.'</div>';
        }

        return $result;
    }

    /**
     * Surrounds the grid by a form if needed.
     *
     * @param string $result grid HTML
     *
     * @return string grid HTML
     */
    protected function renderForm($result)
    {
        if (!$this->getGrid()->isUpdate() && !$this->getGrid()->isEmbedded()) {
            $sm = SessionManager::getInstance();
            $result = '<form id="'.$this->getGrid()->getFormName().'" name="'.$this->getGrid()->getFormName().'" method="post" action="'.Config::getGlobal('dispatcher').'">'.$sm->formState().$result.'</form>';
        }

        return $result;
    }

    /**
     * Render the grid components and the grid itself.
     *
     * @return string grid HTML
     */
    protected function renderGrid()
    {
        $vars = [];

        // $this->getGrid() is an atkdatagrid instance
        foreach ($this->getGrid()->getComponentInstances() as $name => $comp) {
            $vars[$name] = $comp->render(); // when $name == "list", $comp->render() results in a call to DataGridList::render()
        }

        $vars['displayTopInfo'] = $this->getGrid()->getDisplayTopInfo();
        $vars['displayBottomInfo'] = $this->getGrid()->getDisplayBottomInfo();

        return $this->getUi()->render($this->getGrid()->getTemplate(), $vars);
    }

    /**
     * Register JavaScript code for the grid.
     */
    protected function registerScript()
    {
        if ($this->getGrid()->isUpdate()) {
            return;
        }
        

        $name = Json::encode($this->getGrid()->getName());
        $baseUrl = Json::encode($this->getGrid()->getBaseUrl());
        $embedded = $this->getGrid()->isEmbedded() ? 'true' : 'false';

        $this->getPage()->register_script(Config::getGlobal('assets_url').'javascript/datagrid.js');
        $this->getPage()->register_loadscript("
      ATK.DataGrid.register($name, $baseUrl, $embedded);
    ");
    }

    /**
     * Render the grid.
     *
     * @return string grid HTML
     */
    public function render()
    {
        $this->registerScript();
        $result = $this->renderGrid();
        $result = $this->renderContainer($result);
        $result = $this->renderForm($result);

        return $result;
    }
}
