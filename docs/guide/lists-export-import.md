# Lists, export and import

[← Documentation index](../README.md)

This page covers the parts of a node's admin page that show and move many records at once:

- the **DataGrid** (the paginated list);
- **record actions** and **multi-record actions**;
- totals;
- **CSV export** and **CSV import**.

## The admin list (DataGrid)

The `admin` action is handled by `Handlers\AdminHandler`. It builds a `DataGrid\DataGrid` ([source](../../src/DataGrid/DataGrid.php)) with `DataGrid::create($node, ...)`. AJAX refreshes (paging, sorting, searching) restore it with `DataGrid::resume($node)`.

A grid is assembled from **components**. The default ones are:

| Component | Class | Renders |
|---|---|---|
| `list` | `DataGridList` | The table. Rows are built through each attribute's list methods. |
| `summary` | `DataGridSummary` | "x–y of z records". |
| `limit` | `DataGridLimit` | Records-per-page selector (config `recordsperpage`, `recordsperpage_options`, `enable_showall`). |
| `norecordsfound` | `DataGridNoRecordsFound` | Empty-state message. |
| `paginator` | `DataGridPaginator` | Page links (config `pagelinks*`). |
| `index` | `DataGridIndex` | Alphabetical index, when the node has an index set with `Node::setIndex()`. |
| `editcontrol` | `DataGridEditControl` | Save/cancel buttons for in-list editing. |

The grid class can be replaced globally with the config key `datagrid_class`.

### Customising the grid

There are two hooks. Both receive the grid and a mode, `DataGrid::CREATE` or `DataGrid::RESUME`.

- **On the node:** define `modifyDataGrid(DataGrid $grid, $mode)`. `ActionHandler::modifyDataGrid()` calls it from the admin and select handlers.
- **Globally:** `DataGrid::registerModifier($nodeUri, $callback)`. The callback is called as `$callback($grid, $mode)` for grids of that node, or for every grid when `$nodeUri` is `null` or `'*'`. Remove it with `DataGrid::unregisterModifier()`.

```php
public function modifyDataGrid(DataGrid $grid, $mode)
{
    if ($mode == DataGrid::CREATE) {
        $grid->setDefaultLimit(50);
        $grid->setDefaultOrderBy('invoice.date DESC');
        $grid->addFilter('invoice.cancelled = 0');
    }
}
```

The grid API also includes:
- `setExcludes()`;
- `setDefaultActions()`;
- `setMRADefaultAction()`, `setMRASelectionMode()`;
- `setDisplayTopInfo()`, `setDisplayBottomInfo()`, `setTemplate()`;
- `addComponent($name, $class, $options)`, `setComponentOption()`;
- `setCountHandler()`, `setSelectHandler()`, to supply your own data.

### Sorting, searching and columns

- Columns are sortable unless the attribute has `AF_NO_SORT` or the node has `NF_NO_SORT`.
- Attributes with `AF_SEARCHABLE` get a search field in the list header. The extended search form is disabled with `NF_NO_EXTENDED_SEARCH`.
- The per-user column state (sort order, totals) is kept in the session by `RecordList\ColumnConfig`.
- Default order: `Node::setOrder()`.
- Permanent node filters: `Node::addFilter()`.
- Header filter widgets: the node hooks `setAdminHeaderFilter*`. See [Nodes](nodes.md).

### Totals

Attributes with `AF_TOTAL` get a totals row. It is computed with `Selector::getTotals()` and rendered by `RecordList\Totalizer`. A typical use is a `NumberAttribute`, `CurrencyAttribute` or `CalculatorAttribute`.

### Editing in the list

`Node::setEditableListAttributes(...)` makes some attributes editable directly in the grid. The `multiupdate` action saves them through `Handlers\MultiupdateHandler`, which calls `updateDb()` with only those attributes.

## Record actions

Every row has action icons: view, edit, delete, plus copy with `NF_COPY` and editcopy with `NF_EDITAFTERCOPY`.
- `Node::defaultActions($mode)` builds the defaults.
- Each action is shown only if `allowed($action, $record)` returns true.
- Icons are configured with the `icon_*` and `recordlist_icons` config keys.
- `recordlist_onclick` makes a click on the row trigger the first action.

To add, remove or change actions per record, override `recordActions()` on the node:

```php
public function recordActions(array $record, array &$actions, array &$mraactions): void
{
    $actions['print'] = Tools::dispatch_url($this->atkNodeUri(), 'print', ['atkselector' => $record['atkprimkey']]);
    if ($record['locked']) {
        unset($actions['edit'], $actions['delete']);
    }
    parent::recordActions($record, $actions, $mraactions);
}
```

The action needs a matching `action_print($handler)` method on the node (see [Node hooks](node-hooks.md)), and the user needs the privilege for it (see [Security](security.md)). Callbacks registered with `Node::registerRecordActionsCallback()` run after `recordActions()`.

## Multi-record actions (MRA)

With `NF_MRA`, the grid shows a checkbox per row and an action selector under the list. The selector's position is set with the config key `mra_position`.
- If the node does not have `NF_NO_DELETE`, `delete` is the default multi-record action.
- Add actions per record through the `$mraactions` argument of `recordActions()`: `$mraactions[] = 'archive';`.
- Actions the user is not allowed to perform are removed.

When submitted, the action receives the selected primary keys as an **array** in `$this->m_postvars['atkselector']`:

```php
public function action_archive(ActionHandler $handler)
{
    foreach ((array)$this->m_postvars['atkselector'] as $selector) {
        $this->getDb()->query("UPDATE invoice SET archived = 1 WHERE $selector");
    }
    $this->getDb()->commit();
    $this->redirect();
}
```

The selection mode is set with `Node::setMRASelectionMode()`:
- `Node::MRA_MULTI_SELECT` (default): checkboxes.
- `Node::MRA_SINGLE_SELECT`: radio buttons.
- `Node::MRA_NO_SELECT`: all rows, no choice.

`NF_MRPA` (multi-record **priority** actions) shows a priority dropdown per row instead. The range is set with `Node::setPriorityRange($min, $max)`.

> **Warning:** `DataGrid\DataGridList` (the MRA checkbox script) and `Handlers\DeleteHandler::_checkAllowed()` (deleting several records at once) still use the pre-PHP 8 argument order `implode($array, $glue)`. That order raises a `TypeError` on PHP 8 whenever these lines run. See [Known issues](../internals/known-issues.md#legacy-implode-argument-order).

## CSV export

With `NF_EXPORT`, the admin page shows an **Export** button, provided the user has the `view` and `export` privileges. A one-to-many relation whose destination has `NF_EXPORT` also gets an export button. Register the `export` action with the node; see [Modules and menu](modules-and-menu.md).

`Handlers\ExportHandler` shows a form where the user can:
- choose the attributes;
- set the delimiter and enclosure;
- include a title row.

The defaults for these come from config `export_enclosure` and `export_titlerow_checked`. The delimiter is read from `export_delimiter`, but see [Export delimiter config key is misspelled](../internals/known-issues.md#export-delimiter-config-key-is-misspelled).

The file is written by `RecordList\CustomRecordList` in CSV mode, with the records selected in `'export'` mode. Its name comes from `Node::exportFileName()`.

Which attributes are exported:

- By default, all attributes are exported except:
  - `DummyAttribute` and `TabbedPane` subclasses, unless `setForceExport(true)` is set on the attribute;
  - attributes with `AF_AUTOKEY` or `AF_HIDE_VIEW`.
- Node hooks, all optional:
  - `getExportAttributes(ExportHandler $handler)`: define it on the node to replace the list of exportable attributes. It is called through `ActionHandler::invoke()`, so the handler is passed as the first argument. Return an array of `Attribute` objects in the same shape as `Node::getAttributes()`, which is the default.
  - `getExportAttributeGroup($attrName)`: group attributes in the export form. The default group is the attribute's first tab.
  - `assignExportData(array $includes, array &$recordSet)`: post-process the records before they are written.
  - `exportFileName()`: override to change the file name.
- Each attribute renders its value through `display($record, 'csv')`, so `<name>_display` overrides also apply to the export.

## CSV import

With `NF_IMPORT` (and without `NF_READONLY`), users with the `add` privilege get an **Import** button. `Handlers\ImportHandler` runs as a small wizard, using the request parameter `phase`:

1. **`init`**: upload form for the CSV file.
2. **`upload`**: the file is stored in `atktempdir`. ATK analyses it and shows a mapping screen. There the user:
   - maps each CSV column to an attribute (`col_map`);
   - sets the delimiter and enclosure;
   - chooses whether to skip the first row (`skipfirstrow`);
   - chooses whether to update existing records (`doupdate`, with a key to match on);
   - chooses what to do with unknown identifiers (`onfalseid`);
   - chooses whether to validate first (`novalidatefirst`).
3. **`process`**: each row is turned into a record. Relations are resolved through their destination, and string values are parsed with `parseStringValue()`. The node's `initial_values()` are applied.
   - Then the import calls `validate()`, followed by `preAdd()` + `addDb()` (or `preUpdate()` + `updateDb()`).
   - Everything is committed at the end.
   - At most `showmaximporterrors` errors (default 50) are shown.

## See also
- [Nodes](nodes.md)
- [Node hooks and overrides](node-hooks.md)
- [UI and templates](ui-and-templates.md)
- [Node flags reference](../reference/node-flags.md)
