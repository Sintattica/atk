# Node flags reference

[← Documentation index](../README.md)

Node flags are bit flags defined as constants on [`Core\Node`](../../src/Core/Node.php). There are two ways to set them:

- pass them as the second constructor argument;
- change them later with `setFlags()`, `addFlag()` or `removeFlag()`. Read them back with `hasFlag()` and `getFlags()`.

Combine flags with `|`:

```php
use Sintattica\Atk\Core\Node;

class Customer extends Node
{
    public function __construct($nodeUri)
    {
        parent::__construct($nodeUri, Node::NF_ADD_LINK | Node::NF_EXPORT | Node::NF_MRA);
        $this->setTable('customer');
        // ...
    }
}
```

For how the flags change node behaviour in practice, see [Nodes](../guide/nodes.md).

## Flags

| Constant | Value | Effect |
|---|---|---|
| `NF_NO_ADD` | 1 | No new records may be added. |
| `NF_NO_EDIT` | 2 | Records may not be edited. |
| `NF_NO_DELETE` | 4 | Records may not be deleted. |
| `NF_EDITAFTERADD` | 8 | After adding a record, open its edit page. |
| `NF_NO_SEARCH` | 16 | Records may not be searched. |
| `NF_NO_FILTER` | 32 | Ignore filters added with `addFilter()`. |
| `NF_ADD_LINK` | 64 | The admin page shows a link to the add form instead of an embedded add form. |
| `NF_NO_VIEW` | 128 | Records may not be viewed. |
| `NF_COPY` | 256 | Records (or trees) may be copied. |
| `NF_AUTOSELECT` | 512 | On a select page with exactly one record, select it automatically and continue. |
| `NF_TRACK_CHANGES` | 1024 | The original record is stored in `$rec['atkorgrec']` and passed to `postUpdate()`. |
| `NF_NO_SECURITY` | 2048 | Disable access-right checks for the whole node, so everybody may access it. |
| `NF_NO_EXTENDED_SEARCH` | 4096 | Turn off the extended search page. |
| `NF_MULTI_RECORD_ACTIONS` | 8192 | Enable multi-record selection and actions (MRA) in record lists. |
| `NF_MRA` | 8192 | Alias of `NF_MULTI_RECORD_ACTIONS`. |
| `NF_MRPA` | 16384 | Enable multi-record *priority* actions (records selected with a priority). |
| `NF_IMPORT` | 131072 | Enable CSV import. See [Lists, export and import](../guide/lists-export-import.md). |
| `NF_EXPORT` | 262144 | Enable CSV export. |
| `NF_EXT_SORT` | 524288 | Enable extended (multi-column) sorting. |
| `NF_CACHE_RECORDLIST` | 1048576 | Cache the rendered record list (checked by `Handlers\ActionHandler`). |
| `NF_ADDAFTERADD` | 2097152 | After adding a record, open a new add form immediately. |
| `NF_NO_SORT` | 4194304 | Disable sorting. |
| `NF_SPECIFIC_1` … `NF_SPECIFIC_5` | 33554432 … 536870912 | Reserved for subclasses (see below). |
| `NF_EDITAFTERCOPY` | 1073741824 | After copying a record, open the copy for editing. |
| `NF_READONLY` | 7 | Aggregate of `NF_NO_ADD \| NF_NO_EDIT \| NF_NO_DELETE`. |

Bits 32768 and 65536 are unused.

## Subclass-specific flags

`NF_SPECIFIC_1`…`NF_SPECIFIC_5` are free bits that subclasses can give their own meaning. [`Core\TreeNode`](../../src/Core/TreeNode.php) uses four of them:

| Constant | Maps to | Effect |
|---|---|---|
| `TreeNode::NF_TREE_NO_ROOT_DELETE` | `NF_SPECIFIC_1` | Root elements cannot be deleted. |
| `TreeNode::NF_TREE_NO_ROOT_COPY` | `NF_SPECIFIC_2` | Root elements cannot be copied. |
| `TreeNode::NF_TREE_NO_ROOT_ADD` | `NF_SPECIFIC_3` | Root elements cannot be added. |
| `TreeNode::NF_TREE_AUTO_EXPAND` | `NF_SPECIFIC_4` | The tree is initially fully expanded. |

`TreeNode` is not fully ported. See [Known issues](../internals/known-issues.md#treenode-is-not-fully-ported).

## Multi-record action selection modes

When `NF_MRA` is set, `setMRASelectionMode()` controls how records are selected:

| Constant | Value | Selection |
|---|---|---|
| `MRA_MULTI_SELECT` | 1 | Checkboxes; any number of records (default). |
| `MRA_SINGLE_SELECT` | 2 | Radio buttons; exactly one record. |
| `MRA_NO_SELECT` | 3 | No selection widget; the action applies without choosing records. |

## Other node constants

| Constant | Value | Meaning |
|---|---|---|
| `ACTION_DOWNLOAD_FILE_ATTRIBUTE` | `'download_file_attribute'` | Action name used to download files stored by file attributes. |
