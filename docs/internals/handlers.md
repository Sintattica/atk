# Action handlers

[← Documentation index](../README.md)

Every request carries an **action** (`atkaction`) for a node (`atknodeuri`). An **action handler** turns that pair into page content or a redirect. This page covers four things: how ATK picks a handler, how a handler delegates to the node, how partial (AJAX) requests and CSRF tokens work, and how to add or replace actions.

Source: [`src/Handlers/`](../../src/Handlers), [`Core\Node::callHandler()` / `getHandler()`](../../src/Core/Node.php), [`Core\Atk::atkRegisterNodeHandler()` / `atkGetNodeHandler()`](../../src/Core/Atk.php).

## Built-in handlers

`ActionHandler::getDefaultHandler($action)` builds the class name `Sintattica\Atk\Handlers\` + `ucfirst($action)` + `Handler`. If that class does not exist, it returns a plain `ActionHandler`.

| Action | Handler | Notes |
|---|---|---|
| `admin` | `AdminHandler` | List page (DataGrid) plus the inline add form, unless `NF_ADD_LINK` is set. Partials: `datagrid`, `attribute`. |
| `add` | `AddHandler` | Add form. Return behaviour `ATK_ACTION_BACK`. Partials: `attribute`, `sectionstate`. |
| `save` | `SaveHandler` | Stores a new record (`preAdd` → validate → `addDb` → commit). |
| `edit` | `EditHandler` (extends `ViewEditBase`) | Edit form. Partials from `ViewEditBase`: `attribute`, `sectionstate`. |
| `update` | `UpdateHandler` | Stores an edited record (`preUpdate` → validate → `updateDb` → commit). |
| `view` | `ViewHandler` (extends `ViewEditBase`) | Read-only view. |
| `delete` | `DeleteHandler` | Confirmation page, then delete. Checks CSRF on confirm and on cancel. |
| `copy`, `editcopy` | `CopyHandler`, `EditcopyHandler` | Copy a record (and then edit it). |
| `search`, `smartsearch` | `SearchHandler`, `SmartSearchHandler` (extend `AbstractSearchHandler`) | Extended search forms. |
| `select`, `multiselect` | `SelectHandler`, `MultiselectHandler` | Record pickers used by relations. |
| `multiupdate` | `MultiupdateHandler` | Saves inline-edited list rows. |
| `export`, `import` | `ExportHandler`, `ImportHandler` | CSV export and import. Gated by `NF_EXPORT` / `NF_IMPORT`. |
| `feedback` | `FeedbackHandler` | Result page after an action (`setFeedback()` on the node). |
| `xml` | `XmlHandler` | XML dump of records. |

Any other action name, for example `approve`, gets a plain `ActionHandler`. Its `handle()` calls `action_approve()` on the node.

## Resolution: which handler runs?

`Node::callHandler($action)` does the following, in order:

1. `Atk::atkGetNodeHandler($nodeUri, $action)` looks in `g_nodeHandlers[$nodeUri][$action]`, then in `g_nodeHandlers['*'][$action]`. A class-name string is instantiated on first use and cached as an object.
2. If the result is an `ActionHandler` instance, its `handle($node, $action, $postvars)` is called.
3. Otherwise, `Node::getHandler($action)` → `ActionHandler::getDefaultHandler($action)` is used, and the result is stored in `$node->m_handler`.

To register a handler, call this from `Module::register()` or `boot()`:

```php
// Replace the "edit" handler for one node…
$this->atk->atkRegisterNodeHandler('app.employee', 'edit', App\Handlers\EmployeeEditHandler::class);
// …or the "export" handler for every node
$this->atk->atkRegisterNodeHandler('*', 'export', App\Handlers\XlsxExportHandler::class);
```

`atkRegisterNodeHandler()` returns `false` and does nothing if a handler is already registered for that URI and action.

Pitfalls (see [Known issues](known-issues.md#registered-action-handlers-are-ignored-by-internal-gethandler-lookups)):
- `callHandler()` uses an `instanceof ActionHandler` check, so registered handlers **are** used for the request's main action.
- `Node::getHandler()` is also called internally:
  - `AdminHandler::addPage()` calls `getHandler('add')`;
  - `AddHandler` calls `getHandler('edit')`;
  - `Node` itself calls it when confirming actions.

  It validates registrations with an unqualified `is_subclass_of($h, 'ActionHandler')`, which always fails. Those internal lookups therefore **ignore** registered handlers and use the default handler.
- `getHandler()` re-registers the default handler under `$this->m_type`, the type without the module. That key is never looked up by URI.
- The docblock says a *function name* can be registered as a handler. `atkGetNodeHandler()` would call `new $name()` on it and fail, so this does not work.

The most reliable way to customise an action is therefore a **node override**, described next.

## Delegation: `ActionHandler::invoke()`

`handle()` stores the postvars, node, action and partial on the handler, then calls `$this->invoke('action_'.$action)`. When it finishes, it clears the reject info (`atkreject` on the session stack) unless the handler set it itself.

`invoke($methodName, ...$args)` is the extension mechanism used throughout the handlers:

1. If the **node** has a method `$methodName`, it calls `$node->$methodName($handler, ...$args)`. The handler is passed first.
2. Otherwise, if the **handler** has it, it calls `$handler->$methodName(...$args)`.
3. Otherwise, it reports `atkerror("Undefined method …")` and returns `null`.

Handlers call their page-building steps through `invoke()`, so a node can override them without subclassing the handler. Examples: `adminPage`, `addPage`, `editPage`, `viewPage`, `searchPage`, `selectPage`, `getSuccessReturnURL`, `getAddNodeType` and `getExportAttributes`. Grep for `->invoke(` in [`src/Handlers`](../../src/Handlers) to see the full list for a given handler.

```php
class Employee extends Node
{
    // Custom action, reached with atkaction=approve
    public function action_approve(ActionHandler $handler): void
    {
        $record = $this->select($this->m_postvars['atkselector'])->getFirstRow();
        // … change state, then go back
        $this->redirect();
    }

    // Override a step of the default edit handler
    public function editPage(EditHandler $handler, array $record): string
    {
        return '<div class="alert alert-info">Read the policy first.</div>'
            .$handler->editPage($record);
    }
}
```

The node's `allowed($action)` is checked in `IndexPage::loadDispatchPage()` before the handler runs. A custom action must therefore be:
- registered in the node's actions with `registerNode(..., $actions)` or `addAllowedAction()`;
- and granted to the user.

See [Security](../guide/security.md).

## Partial requests

A request with `atkpartial=<name>[.<detail>]` renders without page chrome. `IndexPage` uses `Page::HTML_PARTIAL`, and `Page::renderPartial()` returns only the content plus the scripts and styles registered during this request. URLs are built with `Tools::partial_url($nodeUri, $action, $partial, $params)`, which uses `SessionManager::SESSION_PARTIAL`.

On the handler side, `action_*` methods start with:

```php
if (!empty($this->m_partial)) {
    $this->partial($this->m_partial);
    return;
}
```

`ActionHandler::partial()` splits the name on `.` and calls `partial_<first part>($partial)` **on the handler**. This dispatch does not go through `invoke()`, so nodes cannot override partials directly. If the method does not exist, an inline error span is returned. The built-in partials are listed in the table above. `attribute` forwards to an attribute's own partial method, which is how relations and dependent fields refresh themselves.

## CSRF protection

- `ActionHandler::getCSRFToken()` returns a per-stack token. It is stored with `SessionManager::globalStackVar('ATK_CSRF_TOKEN')` and generated with `md5(uniqid(rand(), true))`.
- The add and edit forms embed it as `<fieldprefix>atkcsrftoken`. `DeleteHandler::confirmAction()` passes it to the confirmation page.
- `isValidCSRFToken()` compares the token loosely. It is checked by `SaveHandler::doSave()`, `UpdateHandler` and `DeleteHandler`. A mismatch renders the access-denied page.
- Custom actions that change data should do the same: add `getCSRFToken()` to the form and check `isValidCSRFToken()` on submit.

## Return behaviour and feedback

- `ATK_ACTION_STAY` (0) / `ATK_ACTION_BACK` (1) is set with `setReturnBehaviour()`. It is posted as `atkreturnbehaviour` by `SessionManager::formState()`, and decides where `SaveHandler` goes after a successful save.
- The result constants `ACTION_CANCELLED`, `ACTION_FAILED` and `ACTION_SUCCESS` are passed to `Node::feedbackUrl($action, $status, …)`. That URL leads to the `feedback` action, replacing the current stack level, in two cases: when the status matches the flags set with `Node::setFeedback($action, $statusmask)`, and always for `ACTION_FAILED`. Otherwise it goes back one level of the session stack.
- `ActionHandler::notify($action, $record)` forwards to `Node::notify()` for action listeners. Listeners are currently never registered: see [Known issues](known-issues.md#action-and-trigger-listeners-are-never-registered).

## Writing a new handler class

1. Extend `Sintattica\Atk\Handlers\ActionHandler`, or an existing handler.
2. Implement `action_<name>()`. Use `$this->m_node`, `$this->m_postvars`, `$this->getPage()->addContent(...)`, `$this->getUi()->renderBox(...)` and `$this->m_node->renderActionPage(...)`.
3. Call overridable steps through `$this->invoke('stepName', ...)`, so nodes can still customise them.
4. Register the class with `atkRegisterNodeHandler()`. If it should be the global default for an action name, put it in `Handlers\`.

## Related pages

- [Request lifecycle](request-lifecycle.md)
- [Node hooks and overrides](../guide/node-hooks.md)
- [Session and state](session-and-state.md)
