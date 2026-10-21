# Node hooks and overrides

[← Documentation index](../README.md)

ATK nodes are customised mostly by **overriding methods**. ATK calls these methods at well-defined moments. Some are ordinary methods of `Core\Node` that you override, such as `preAdd()` and `validate()`. Others are discovered by **name**: ATK checks with `method_exists()` whether your node defines a method like `email_display()` or `adminHeader()`, and calls it instead of the default.

All signatures below are taken from [`src/Core/Node.php`](../../src/Core/Node.php) and the handlers in [`src/Handlers/`](../../src/Handlers). Because PHP checks signature compatibility, an override must keep the parent's parameter types and return type.

## Initialisation hooks

`Node::init()` calls these once, after the constructor, when the node is created through `Atk::atkGetNode()`.

| Method | Use it to |
|---|---|
| `protected function setFilters(): void` | Add filters with `addFilter()` |
| `protected function setRowColors(): void` | Add `addColorCondition()` rules for list rows |
| `public function setLegendItems(): void` | Add `addLegendItem()` entries |
| `public function setAdminHeaderFilter(): void` | Add header button or input filters to the list page |
| `public function buildAdminPageNodeHelp(): string` | Return HTML help shown in the list header |

Before these hooks run, `init()` calls `postInit()` on every attribute. At this point the attributes know their owner node and their tabs.

## Record life-cycle triggers

Triggers are called by `addDb()`, `updateDb()`, `deleteDb()` and `copyDb()`, and by the save, update and delete handlers. They all receive the record. Return `true` to continue. Return `false` to report a failure, and the handler then rolls back the transaction.

| Trigger | Called | Can modify the record | `false` aborts |
|---|---|---|---|
| `preAdd(array &$record, string $mode = 'add'): bool` | Before validation (save handler) and before the INSERT (`addDb`). `$mode` is `add` or `copy`. | Yes | Yes |
| `postAdd(array $record, string $mode = 'add'): bool` | After the INSERT. The record contains the generated id. | No | Yes. `addDb()` returns false. |
| `preUpdate(array &$record): bool` | Before validation (update handler) and before the UPDATE (`updateDb`) | Yes | **No**, see the warning below |
| `postUpdate(array $record): bool` | After the UPDATE. With `NF_TRACK_CHANGES`, the old values are in `$record['atkorgrec']`. | No | Yes |
| `preDelete(array $record): bool` | Once per record matched by the delete selector | No | Yes |
| `postDel(array $record): bool` (deprecated) / `postDelete(array $record): bool` | After the DELETE, per record | No | Yes |
| `preCopy(array &$record): void` | Before a copy is stored, e.g. to prefix the title with "Copy of" | Yes | n/a |

Rules worth knowing:

- **Each trigger runs once per record.** `Node::executeTrigger()` marks the record with `__executed<trigger>`. The save handler calls `preAdd()` before validating, and `addDb()` does not call it a second time.
- **Call the parent in `preAdd()` and `preUpdate()`.** The base implementations serialise nested (JSON) attributes.
- **Return a boolean.** A `null` return is accepted with a debug warning and treated as success.
- **Store methods do not commit.** `addDb()`, `updateDb()` and `deleteDb()` do not commit; the handlers do. If you call them from your own code, call `$this->getDb()->commit()` yourself.
- **Listeners don't help here.** Attaching trigger listeners (`Utils\TriggerListener`, `Listeners\RecycleBin`) currently has no effect; see [Known issues](../internals/known-issues.md#action-and-trigger-listeners-are-never-registered). Put the logic in the trigger methods instead.

> **Warning:** the return value of `preUpdate()` is ignored. `UpdateHandler` calls `executeTrigger('preUpdate', ...)` without checking the result, and `updateDb()` does the same. Returning `false` therefore does **not** stop an update. To block an update, add an error to the record (see below) or throw an exception. See [Known issues](../internals/known-issues.md#returning-false-from-preupdate-does-not-stop-an-update).

```php
public function preAdd(array &$record, string $mode = 'add'): bool
{
    $record['code'] = strtoupper($record['code']);
    return parent::preAdd($record, $mode);
}

public function postUpdate(array $record): bool
{
    $old = $record['atkorgrec'] ?? null;    // requires NF_TRACK_CHANGES
    if ($old && $old['status'] !== $record['status']) {
        // ... notify somebody
    }
    return true;
}
```

## Validation

The save and update handlers call `Node::validate(array &$record, string $mode, array $ignoreList = []): bool`. `$mode` is `'add'` or `'update'`. The default implementation delegates to the class named in `$m_validate_class`, which is `Core\NodeValidator` ([source](../../src/Core/NodeValidator.php)). For every attribute not in `$ignoreList`, it does the following:

1. **Per-attribute validation.** It runs only if the value is not empty:
   - if the node has a method `<attribute>_validate(array &$record, string $mode)`, ATK calls it;
   - otherwise ATK calls the attribute's own `validate()`.
2. **Primary key uniqueness** for non-auto-increment keys (`error_primarykey_exists`).
3. **`AF_OBLIGATORY`** fields must not be empty (`error_obligatoryfield`).
4. **`AF_UNIQUE`** values must not already exist (`error_uniquefield`).

After the loop, it checks the unique field combinations declared with `addUniqueFieldset(['name', 'parent_id'])`.

Errors are collected in `$record['atkerror']`. To add your own error, use `Tools::triggerError()`:

```php
use Sintattica\Atk\Core\Tools;

// Called only when 'end_date' is not empty
public function end_date_validate(array &$record, string $mode): void
{
    if ($record['end_date'] < $record['start_date']) {
        Tools::triggerError($record, 'end_date', 'error_end_before_start');
    }
}

// Whole-record validation
public function validate(array &$record, string $mode, array $ignoreList = []): bool
{
    $ok = parent::validate($record, $mode, $ignoreList);
    if ($mode === 'add' && $record['budget'] > 10000 && empty($record['approved_by'])) {
        Tools::triggerError($record, 'approved_by', 'error_approval_required');
        $ok = false;
    }
    return $ok;
}
```

`Tools::triggerError(&$rec, $attrib, $err, $msg = '', $tab = '', $label = '', $module = 'atk')` translates `$err` with `atktext()` when `$msg` is empty. Put the key in your module's language file. `Tools::atkTriggerError(&$record, $attribute, $error, $message = '')` does the same but takes the attribute **object** (or an array of them), so it can fill in the label and tab.

When the record has errors, the handler rolls back and shows the form again with the messages.

## Default values

| Method | Purpose |
|---|---|
| `initial_values(): array` | Values for a new record (add form). The default collects each attribute's `initialValue()` (see `Attribute::setInitialValue()`), plus values passed in the request. Merge with the parent's result when you override. |
| `edit_values(array $record): array` | Change a loaded record just before the edit form is shown. |

```php
public function initial_values(): array
{
    $values = parent::initial_values();
    $values['status'] = 'prospect';
    $values['customer_since'] = date('Y-m-d');
    return $values;
}
```

## Per-attribute overrides (by name)

Define these methods on the node to change how one attribute behaves without subclassing it. `<attr>` is the attribute name.

| Method | Replaces | Called from |
|---|---|---|
| `<attr>_display(array $record, string $mode, Attribute $attr): string` | `Attribute::display()` in list, view, export and plain modes | `Attribute::getView()` |
| `<attr>_edit(array $record, string $fieldprefix, string $mode, Attribute $attr): string` | `Attribute::edit()` in add and edit forms. Not supported for an attribute named `action`. | `Attribute::getEdit()` |
| `<attr>_hide(array $record, string $fieldprefix, string $mode): string` | The hidden-field rendering | `Attribute::hide()` |
| `<attr>_search(...)` | The search widget. In the list header the signature is `($atksearch, false, $fieldprefix, DataGrid $grid)`; in the extended search form it is `($record, $extended, $fieldprefix)`. | `Attribute::addToListArrayHeader()`, `Attribute::addToSearchformFields()` |
| `<attr>_searchcondition(Query $query, string $table, $value, string $searchmode): string` | The SQL search condition | `Node::getSearchCondition()` |
| `<attr>_validate(array &$record, string $mode)` | `Attribute::validate()` | `NodeValidator::validateAttributeValue()` |

The attribute's own callbacks take precedence over the `_display` and `_edit` methods:
- `Attribute::setViewCallback(callable)` receives `($record, $mode, $attribute)`;
- `Attribute::setEditCallback(callable)` receives `($record, $fieldprefix, $mode, $attribute)`.

```php
public function email_display(array $record, string $mode, $attr): string
{
    $email = htmlspecialchars($record['email'] ?? '');
    return $mode === 'list' ? "<a href=\"mailto:$email\">$email</a>" : $email;
}
```

To change attribute flags depending on the record or the mode, override `setAttributesFlags(array $record = null, string $mode = ''): void`. It is called before the admin, add, edit, view and search pages are built, with `$mode` set to `admin`, `add`, `edit`, `view` or `search`.

```php
public function setAttributesFlags(array $record = null, string $mode = ''): void
{
    if ($mode === 'edit' && ($record['status'] ?? '') === 'lost') {
        $this->getAttribute('status')->addFlag(A::AF_READONLY);
    }
}
```

## Record actions and row display

| Method | Purpose |
|---|---|
| `recordActions(array $record, array &$actions, array &$mraactions): void` | Called for each row of the list. `$actions` maps action names to URLs. Add entries (`$actions['archive'] = Tools::dispatch_url(...)`) or remove them (`unset($actions['delete'])`). `$mraactions` holds the multi-record actions available for the row. |
| `registerRecordActionsCallback(callable $cb)` | The same, but from outside the node. The callback gets `($record, &$actions, &$mraactions)`. |
| `rowColor(array $record): string` | Background colour of a row (legacy; prefer `addColorCondition()` in `setRowColors()`) |
| `rowClass(array $record, int $nr): string` / `setRowClassCallback($cb)` | CSS classes of a row |
| `getFormButtons(string $mode, ?array $record = []): array` | Buttons shown under add, edit and view forms |
| `descriptor(array $record): string` / `descriptor_def(): string` | The record's display name; see [Nodes](nodes.md#primary-key-and-descriptor) |

```php
public function recordActions(array $record, array &$actions, array &$mraactions): void
{
    if ($record['status'] === 'lost') {
        unset($actions['edit']);
    }
    $actions['archive'] = Tools::dispatch_url($this->atkNodeUri(), 'archive', ['atkselector' => $record['atkprimkey']]);
    parent::recordActions($record, $actions, $mraactions);
}
```

## Custom actions

Any action without a handler class is handled by the generic `Handlers\ActionHandler`. It calls `action_<name>(ActionHandler $handler)` on the node. The action must also be allowed:
1. list it in the module's `registerNode()` actions;
2. grant it to users, or call `addAllowedAction('archive')`;
3. optionally map it onto an existing privilege with `addSecurityMap('archive', 'edit')`.

See [Security](security.md).

```php
public function action_archive(ActionHandler $handler): void
{
    // Ask for confirmation first; checkConfirmAction() renders the yes/no page
    if (!$this->checkConfirmAction($handler)) {
        return;
    }
    $this->getDb()->query("UPDATE crm_customer SET archived = 1 WHERE " . $this->m_postvars['atkselector']);
    $this->getDb()->commit();
    $this->redirect();
}
```

`Node::checkConfirmAction(ActionHandler $handler, ?array $atkSelectors = null, ?string $cancelUrl = null): bool` is a protected helper. It works like this:
- On the first request it renders the confirmation page (yes/no) and returns `false`.
- If the user cancels, it redirects back, or to `$cancelUrl`, and returns `false`.
- It returns `true` only once the user has confirmed.

The confirmation page of an action can be customised by defining:
- `confirm<action>($atkselector)`, which returns the whole page;
- `confirm<action>text($atkselector)`, which returns only the question.

The default question is the translation of `confirm_<action>`, or of `confirm_<action>_multi` when several records are selected.

## Page and handler overrides

Handlers build pages through `ActionHandler::invoke($method, ...)`. If your node has a method with the same name, ATK calls it with the **handler as the first argument**, followed by the handler's own arguments. Override them to change a page partially or completely.

| Node method | Default implementation |
|---|---|
| `action_admin(AdminHandler $h)`, `adminPage(AdminHandler $h, array $actions = [])` | `Node` (calls `setAttributesFlags()` and then the handler) |
| `adminHeader(AdminHandler $h): string`, `adminFooter(AdminHandler $h): string` | `AdminHandler`. HTML above and below the list. |
| `addPage(AddHandler $h, array $record = null)`, `addHeader(AddHandler $h, $record = null)` | `Node` / `AddHandler` |
| `editPage(EditHandler $h, array $record)`, `editHeader(EditHandler $h, $record)` | `Node` / `EditHandler` |
| `viewPage(ViewHandler $h, array $record, Node $node, bool $renderbox = true)`, `viewHeader(ViewHandler $h)` | `Node` / `ViewHandler` |
| `searchPage(SearchHandler $h, array $record = null)` | `Node` |
| `getSuccessReturnURL(SaveHandler $h, $record)` | `SaveHandler`. Where to go after a successful save. |
| `getExportAttributes(...)`, `importPage(...)`, `selectPage(...)`, `feedbackPage(...)` | Export, import, select and feedback handlers |
| `modifyDataGrid(DataGrid $grid, int $mode)` | Called by handlers after creating or resuming a data grid. See [Lists](lists-export-import.md). |

```php
use Sintattica\Atk\Handlers\AdminHandler;

public function adminHeader(AdminHandler $handler): string
{
    return '<div class="alert alert-info">' . $this->text('customers_admin_intro') . '</div>';
}
```

You can also register a custom handler with `Atk::atkRegisterNodeHandler($nodeUri, $action, $handler)`. It is used for the request's main action, but internal lookups through `Node::getHandler()` ignore it; see [Known issues](../internals/known-issues.md#registered-action-handlers-are-ignored-by-internal-gethandler-lookups). Overriding node methods is the more reliable way. How `invoke()` works internally is described in [Action handlers](../internals/handlers.md).

## Related pages

- [Nodes](nodes.md)
- [Attributes](attributes.md)
- [Security](security.md)
- [Action handlers (internals)](../internals/handlers.md)
