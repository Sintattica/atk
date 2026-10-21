# Data access

[← Documentation index](../README.md)

ATK gives you two levels of data access:

1. **Through a node.** `select()`, `addDb()`, `updateDb()`, `deleteDb()` and the other methods on this page. They apply the node's attributes, relations, filters, triggers and value conversion (`db2value`/`value2db`). Prefer this level in application code.
2. **Directly on the database.** `Db\Db`, the `Db\Query` builder and prepared statements, for raw SQL, reporting and migrations.

Driver internals are covered in [Database layer](../internals/database-layer.md).

## Getting a node instance

```php
use Sintattica\Atk\Core\Atk;

$customers = Atk::getInstance()->atkGetNode('app.customer');
```

Inside a node, `$this` already is the node, and `$this->getDb()` returns its database connection.

## Selecting records: `Node::select()` and `Utils\Selector`

`Node::select(string $condition = null, array $params = [])` returns a `Utils\Selector`, a fluent query object. Nothing is executed until you ask for rows.

```php
$rows = $customers->select('customer.country = ? AND customer.active = ?', ['IT', 1])
    ->orderBy('customer.name')
    ->limit(20, 0)
    ->getAllRows();

$first = $customers->select('customer.id = ?', [$id])->getFirstRow();
$count = $customers->select('customer.active = 1')->getRowCount();
```

`Selector` methods:

| Method | Purpose |
|---|---|
| `where($condition, $params = [])` | Add a condition, combined with `AND`. Can be called several times. |
| `orderBy($order)` | SQL `ORDER BY` expression. |
| `limit($limit, $offset = 0)` | Paging. |
| `includes($attrs)` / `excludes($attrs)` | Load only some attributes, or skip some. |
| `mode($mode)` | Select mode passed to attributes, e.g. `'view'`, `'edit'`, `'export'`. Some attributes load differently per mode. |
| `distinct($distinct)` | `SELECT DISTINCT`. |
| `ignoreDefaultFilters()` | Skip filters added with `Node::addFilter()`. |
| `ignorePostvars()` | Ignore search and filter values coming from the request. |
| `ignoreForceLoad()` | Do not load `AF_FORCE_LOAD` attributes that are not included. |
| `ignorePrimaryKey()` | Do not force-load the primary key. |
| `getAllRows()` | Execute and return all records (cached after the first call). |
| `getFirstRow()` | Execute and return the first record, or `null`. |
| `getRowCount()` | `COUNT` query with the same conditions. |
| `getTotals($fields)` | Sums for the given fields (used for `AF_TOTAL`). |
| `getIndices()` | Values for the alphabetical index in lists. |

The selector also implements `ArrayAccess`, `IteratorAggregate` and `Countable`, so you can write `foreach ($node->select(...) as $row)`.

**Bind parameters.** Queries are executed as prepared statements. `$params` supports both anonymous `?` parameters and named `:name` parameters (`Db\Statement\Statement`). Always bind user input instead of concatenating it.

**Records** are associative arrays keyed by attribute name, with values already converted by `db2value()`:
- dates are arrays;
- a many-to-one relation value is an array with the destination's primary key and descriptor fields.

Every record also contains `atkprimkey`, the primary key as an SQL condition string, e.g. `customer.id='5'`.

`Node::fetchByPk(int $pk): array` is a shortcut for `select("<table>.<pk> = ?", [$pk])->getFirstRow()`. It works only for nodes with a single integer primary key.

Two other node helpers:
- `Node::primaryKey($record)` returns the primary key condition string for a record.
- `Node::primaryKeyField()` returns the name of the primary key attribute.

## Writing records

| Method | Purpose |
|---|---|
| `addDb(array &$record, bool $exectrigger = true, string $mode = 'add', array $excludelist = []): bool` | Insert. The new primary key is written back into `$record`. |
| `updateDb(array &$record, bool $exectrigger = true, $excludes = '', $includes = ''): bool` | Update the record identified by `$record['atkprimkey']`. |
| `deleteDb(string $selector, bool $exectrigger = true, bool $failwhenempty = false): bool` | Delete the records matching the SQL condition. |
| `copyDb(array &$record, string $mode = 'copy'): bool` | Copy a record (runs `preCopy`). |
| `searchDb(string $expression, string $searchmethod = 'OR'): array` | Search all searchable attributes for an expression. |

- With `$exectrigger = true`, the node triggers run: `preAdd`/`postAdd`, `preUpdate`/`postUpdate`, `preDelete`/`postDelete`. See [Node hooks](node-hooks.md).
- These methods do **not** validate the record. Validation runs in the save and update handlers. Call `$node->validate($record, 'add')` yourself if you need it.

> **Transactions:** `addDb()`, `updateDb()` and `deleteDb()` **do not commit**. The MySQLi driver turns autocommit off when it connects (`MySqliDb::doConnect()`). The built-in handlers (save, update, delete, copy, import) call `$db->commit()` themselves.
>
> When you write data from your own code, for example a custom action, a CLI script or a trigger outside a handler, call `$node->getDb()->commit()`, or `rollback()` on failure. Otherwise the changes are lost at the end of the request.

```php
$record = ['name' => 'ACME', 'country' => 'IT'];
if ($customers->addDb($record)) {
    $customers->getDb()->commit();
    $newId = $record['id'];
} else {
    $customers->getDb()->rollback();
}
```

## The `Db` API

`Db\Db::getInstance($conn = 'default', $reset = false, $mode = 'rw')` returns the connection configured in the `db` config section; see [Configuration basics](../getting-started/configuration-basics.md). There is one instance per connection name.

| Method | Purpose |
|---|---|
| `getRows($query, $offset = -1, $limit = -1)` | All rows as associative arrays. |
| `getRow($query)` | First row. |
| `getRowsAssoc($query, $keyColumn = 0)` | Rows keyed by a column. |
| `getValue($query, $default = null)` | First column of the first row. |
| `getValues($query)`, `getValuesAssoc($query)` | First column of all rows, or a key/value map. |
| `query($sql)` | Run a statement without a result set. |
| `prepare($sql)` | Prepared statement: `execute($params)`, `getAllRows()`, `getFirstRow()`, `getFirstValue()`, `getAllValues()`, `getAffectedRowCount()`, `close()`. |
| `commit()`, `rollback()`, `savepoint($name)` | Transactions. |
| `nextid($sequence)`, `getInsertId()` | Generated keys. |
| `escapeSQL($value, $wildcards = false)`, `quoteIdentifier($name)` | Escaping, for when you cannot bind. |
| `tableExists($table)`, `tableMeta($table)`, `metadata($table)`, `table_names()` | Schema inspection. |
| `func_now()`, `func_concat()`, `func_concat_ws()`, `func_substring()`, `func_datetochar()`, `func_datetimetochar()` | Driver-portable SQL fragments. |

```php
$db = $this->getDb();
$stmt = $db->prepare('SELECT id, name FROM customer WHERE country = :country');
$stmt->execute(['country' => 'IT']);
foreach ($stmt->getAllRows() as $row) { /* ... */ }
$stmt->close();
```

## Query builder

`$db->createQuery()` returns a driver-specific `Db\Query` (`MySqliQuery` or `PgSqlQuery`):

```php
$query = $db->createQuery();
$query->addTable('invoice');
$query->addJoin('customer', 'c', 'c.id = invoice.customer_id', true);   // outer join
$query->addField('id', '', 'invoice');
$query->addExpression('total', 'SUM(invoice.amount)');
$query->addCondition($query->exactCondition('c.country', 'IT'));
$query->addGroupBy('invoice.id');
$query->addOrderBy('total DESC');
$query->setLimit(0, 10);
$rows = $query->executeSelect();
```

| Group | Methods |
|---|---|
| Building | `addTable`, `addJoin`, `addField`/`addFields` (also used for insert and update values), `addExpression`, `addCondition`, `addGroupBy`, `addOrderBy`, `setDistinct`, `setLimit($offset, $limit)` |
| Condition helpers | `exactCondition`, `substringCondition`, `betweenCondition`, `nullCondition`, … |
| Execution | `executeSelect`, `executeInsert`, `executeUpdate`, `executeDelete`, or `buildSelect()` to get the SQL string |

For schema changes, `$db->createDdl()` returns a `Db\Ddl`. See [Database layer](../internals/database-layer.md).

## Using another connection

`Node::setTable($tablename, $seq = '', $db = null)` accepts a connection name or a `Db` instance as the third argument. All node operations then run on that connection. Add the connection to the `db` config array under its own key.

## DataNode: nodes without a table

`Core\DataNode` is a node backed by a PHP array instead of a table. The constructor adds `NF_NO_ADD | NF_NO_EDIT`. `addDb()`, `updateDb()` and `deleteDb()` return `false`.

```php
class CountryList extends DataNode
{
    public function __construct($nodeUri)
    {
        parent::__construct($nodeUri);
        $this->add(new Attribute('code', Attribute::AF_PRIMARY));
        $this->add(new Attribute('name'));
        $this->setData([
            ['code' => 'IT', 'name' => 'Italy'],
            ['code' => 'NL', 'name' => 'Netherlands'],
        ]);
    }
}
```

`DataNode::select()` filters, sorts and limits the array in PHP. You can override `getData()` (protected) to compute the data lazily.

> **Warning:** `DataNode::select()` returns a plain `array`, not a `Selector`.
> - `DataGrid` and the built-in handlers chain selector methods on the result, e.g. `select()->excludes(...)`. Those calls fail on a `DataNode`. Do not use a `DataNode` as an admin page on `main`, only through your own code.
> - `DataNode::select()` also reads an undefined `$limit` variable, so offset and limit are never applied. See [Known issues](../internals/known-issues.md#datanode-cannot-back-an-admin-page).

## See also
- [Nodes](nodes.md)
- [Node hooks and overrides](node-hooks.md)
- [Database layer](../internals/database-layer.md)
