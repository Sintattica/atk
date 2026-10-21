# Database layer

[← Documentation index](../README.md)

ATK has its own small database abstraction, with no PDO and no ORM. This page covers the following:
- connections;
- the `Db` API used by nodes and applications;
- the query builder (`Query`) and the schema builder (`Ddl`);
- prepared statements;
- what a new driver needs to implement.

For the application-level view (`Node::select()`, `Selector`, `addDb()`/`updateDb()`), see [Data access](../guide/data-access.md).

Source: [`src/Db/`](../../src/Db).

| Class | Role |
|---|---|
| `Db\Db` | Base class: connection registry, convenience fetch methods, error handling, SQL portability helpers. |
| `Db\MySqliDb`, `Db\PgSqlDb` | Drivers. `MySqli` is the default and the only driver whose extension is required by `composer.json` (`ext-mysqli`). `PgSql` needs `ext-pgsql`, which you install yourself. |
| `Db\Query` → `MySqliQuery`, `PgSqlQuery` | Fluent SELECT, INSERT, UPDATE and DELETE builder. |
| `Db\Ddl` → `MySqliDdl`, `PgSqlDdl` | CREATE, ALTER and DROP builder for tables, views, indexes and sequences. |
| `Db\Statement\Statement` → `MySqliStatement`, `CompatStatement` | Prepared statements with `?` or `:name` parameters. |

## Connections

Connections are named entries of the `db` config key. The default entry reads `DB_*` environment variables, but see [Known issues](known-issues.md#environment-variables-from-env-are-not-visible-to-configenv):

```php
'db' => [
    'default' => [
        'host'     => 'localhost',
        'db'       => 'atk',
        'user'     => 'root',
        'password' => '',
        'port'     => 3306,          // optional
        'charset'  => 'utf8',        // optional: SET NAMES (MySQL) / client_encoding (PgSQL)
        'collate'  => '...',         // optional
        'force_ci' => false,         // optional, case-insensitive search helper
        'mode'     => 'rw',          // optional: 'r', 'w' or 'rw'
        'driver'   => 'MySqli',      // class prefix: Sintattica\Atk\Db\<driver>Db
    ],
    'reporting' => [ /* a second connection */ ],
],
```

- **`Db::getInstance($conn = 'default', $reset = false, $mode = 'rw')`** returns one cached instance per connection name. It creates a new one when `$reset` is true or when the cached instance lacks `$mode`. `Db::newInstance()` always creates a new instance.
- **Driver selection.** The driver class is `__NAMESPACE__.'\\'.$config['driver'].'Db'`, so `driver` must match a class name exactly. `init()` copies the config onto the instance. The physical connection is opened lazily by `connect()` on the first query.
- **Aliases.** `$db->useMapping(['default' => 'test'])` redirects connection names, for example in tests. `getTranslatedDatabaseName()` resolves them, and `clearMapping()` resets them.
- **Nodes.** A node uses the connection given as the third argument of `Node::setTable($table, $sequence, $db)`. `Node::getDb()` returns `Db::getInstance()` when that argument is empty, `Db::getInstance($name)` for a string, or the object itself.

## Transactions

ATK does **not** commit automatically at the end of a request. Each driver handles this differently:

- **MySqli.** `doConnect()` calls `mysqli_autocommit($link, false)`. Everything runs in a transaction until `commit()`. Uncommitted work is discarded when the connection closes.
- **PgSql.** `doConnect()` sends `BEGIN`. `commit()` sends `COMMIT` and `rollback()` sends `ROLLBACK`.

> **To verify:** no new `BEGIN` is issued after `commit()`. Statements after the first commit in a request therefore appear to run in PostgreSQL autocommit mode, and a later `rollback()` would have no effect.

The handlers commit explicitly after a successful write (`SaveHandler::storeRecordInDb()`, `UpdateHandler`, `DeleteHandler`) and roll back on validation or storage errors. Code that writes through `Node::addDb()`, `updateDb()` or `deleteDb()`, or through `Db::query()`, outside these handlers must call `$db->commit()` itself.

`savepoint($name)` and `rollback($savepoint)` support nested work.

## Running queries

```php
$db = Db::getInstance();

$rows  = $db->getRows("SELECT * FROM employee WHERE active = 1");        // list of assoc arrays
$row   = $db->getRow("SELECT * FROM employee WHERE id = 3");             // first row or null
$name  = $db->getValue("SELECT name FROM employee WHERE id = 3");        // first column of first row
$ids   = $db->getValues("SELECT id FROM employee");                      // first column of all rows
$byId  = $db->getRowsAssoc("SELECT * FROM employee", 'id');              // keyed by a column
$pairs = $db->getValuesAssoc("SELECT id, name FROM employee");           // key => value

$db->query("UPDATE employee SET active = 0 WHERE id = ".(int) $id);
$db->commit();
```

- **Paging.** `getRows()`, `getValues()` and `getRowsAssoc()` accept `$offset` and `$limit`. `query()` appends the driver-specific `LIMIT` clause itself.
- **Escaping.** Use `escapeSQL($string, $wildcard = false)` for values you concatenate. `quoteIdentifier()` quotes names.
- **Sequences.** `nextid($sequence)` returns the next id.
  - *MySqli* emulates sequences with a locked table `db_sequence (seq_name, nextid)`.
  - *PgSql* calls `nextval('<database_sequenceprefix><sequence><database_sequencesuffix>')`, and falls back to `nextval('<sequence>')`.

  `getInsertId()` returns the last auto-increment id (MySqli).
- **Metadata.** `metadata($table)`, `tableMeta($table)`, `table_names()` and `tableExists()`. `MF_*` constants describe column flags. Metadata is cached when `meta_caching` is on.
- **Portability helpers.** `func_now()`, `func_substring()`, `func_datetochar()`, `func_datetimetochar()`, `func_concat()`, `func_concat_ws()` and `vendorDateFormat()` return SQL fragments for the current vendor. `getSearchModes()` lists the supported search modes.
- **Errors.**
  - `hasError()`, `getErrorType()` (`'user'` or `'system'`), `getErrorMsg()`, `getAtkDbErrno()` (`DB_*` constants) and `getDbError()`.
  - `halt()` reports through `Tools::atkerror()`, controlled by `setHaltOnError()`.
  - MySQL error 1451 (foreign key restriction) is classed as a *user* error, so it is shown to the user instead of failing the request.
- **Debugging.** With `debug >= 0`, every statement is recorded through `Debugger::addQuery()` and appears in the debug console.

## Prepared statements

```php
$stmt = Db::getInstance()->prepare('SELECT * FROM employee WHERE dept_id = :dept AND active = :active');
$stmt->execute(['dept' => 5, 'active' => 1]);
foreach ($stmt as $row) { /* … */ }
$first = $stmt->getFirstRow();
$stmt->close();
```

- **Parameters.** Use either named (`:name`) or anonymous (`?`) parameters. You cannot mix the two in one query. `StatementParser` handles both.
- **Fetch helpers.** `getFirstRow()`, `getAllRows()`, `getAllRowsAssoc()`, `getFirstValue()`, `getAllValues()`, `getAllValuesAssoc()` and `getAffectedRowCount()`.
- **Implementation.** `Db::prepare()` instantiates `Statement\<type>Statement`, otherwise `Statement\CompatStatement`.
  - `MySqliStatement` uses native mysqli prepared statements.
  - PgSql has no native class. It uses `CompatStatement`, which escapes each value with `escapeSQL()` and interpolates it into the SQL.

## Query builder

`$db->createQuery()` returns a `<type>Query` bound to the connection. All `add*` methods return `$this`.

```php
$q = Db::getInstance()->createQuery();
$q->addTable('employee', 'e')
  ->addJoin('department', 'd', 'd.id = e.dept_id', true)   // LEFT JOIN
  ->addField('name', '', 'e')
  ->addField('name', '', 'd', 'dept_')                     // selected as a generated alias (al_N)
  ->addCondition($q->exactCondition('e.active', 1))
  ->addOrderBy('e.name')
  ->setLimit(0, 20);
$rows = $q->executeSelect();
foreach ($rows as &$row) {
    $q->deAlias($row);                                     // al_N → dept_name
}
```

`executeSelect()` does not translate aliases. When you use an alias prefix, call `deAlias($row)` on each row yourself.

- **Reading.** `buildSelect()` or `executeSelect()`, `buildCount()`, `setDistinct()`, `addGroupBy()`, `addExpression()`.
- **Writing.** `buildInsert()`, `buildUpdate()` and `buildDelete()`, or their `execute*()` forms. Values come from `addField($name, $value, …)`, and `addSequenceField()` fills a key from `nextid()`.
- **Condition builders.** `exactCondition`, `substringCondition`, `wildcardCondition`, `greaterthanCondition` (and the `equal` and `lessthan` variants), `betweenCondition`, `nullCondition`/`notNullCondition`. Attributes use these through their search methods. `addSearchCondition()` together with `setSearchMethod()` combines search conditions with `AND` or `OR`.
- **Escaping.** `addField()` wraps the value in quotes but does **not** escape it. Pass values that have already gone through `escapeSQL()`, as attributes do in `Attribute::value2db()`.

## Schema builder (DDL)

`$db->createDdl()` returns a `<type>Ddl` bound to the connection. `Ddl::create($driver)` does the same from the config of the `default` connection.

```php
$ddl = Db::getInstance()->createDdl();
$ddl->setTable('note');
$ddl->addField('id', 'number', 10, Ddl::DDL_PRIMARY);
$ddl->addField('title', 'string', 100, Ddl::DDL_NOTNULL);
$ddl->addField('text', 'text');
$ddl->executeCreate();
```

- **Types.** Field types are generic strings, mapped to vendor types by `getType()`: `number`, `decimal`, `string`, `date`, `text`, `datetime`, `time` and `boolean`.
- **Flags.** The available flags are `Ddl::DDL_PRIMARY`, `DDL_UNIQUE` and `DDL_NOTNULL`. A primary key is always `NOT NULL`.
- **MySQL booleans.** `MySqliDdl::getType('boolean')` returns `NUMBER(1,0)`, which is not a valid MySQL type. Use `number` with size 1 instead.

The class also offers:
- table methods: `buildCreate`/`executeCreate`, `buildAlter`/`executeAlter`, `buildDrop`/`executeDrop`, `loadMetaData()` to copy an existing table, and `renameTable()`;
- views: `executeCreateView()`, `executeDropView()`;
- indexes: `createIndex()`, `dropIndex()`;
- sequences: `renameSequence()`, `dropSequence()`.

`Db::cloneAll($otherDb)`, `dropAll()` and `deleteAll()` are built on these, mainly for test databases.

## Adding a driver

To add a vendor `Foo`:

1. **`Db\FooDb extends Db`.**
   - In the constructor, set `m_type = 'Foo'` and `m_vendor`.
   - Implement `doConnect()`, `query()`, `next_record()`, `affected_rows()`, `nextid()`, `metadata()`, `table_names()`, `tableExists()`, `commit()`, `rollback()`, `savepoint()`, `lock()`/`unlock()`, `escapeSQL()` and `_translateError()`.
   - Override the `func_*` helpers and `getSearchModes()` where the SQL differs.
2. **`Db\FooQuery extends Query`.** Implement at least `_addLimiter()`, plus any vendor-specific build methods.
3. **`Db\FooDdl extends Ddl`.** Implement type mapping (`getType()`/`getGenericType()`) and any syntax differences.
4. **Optionally, `Db\Statement\FooStatement extends Statement`** for native prepared statements. Without it, `CompatStatement` is used.
5. Set `'driver' => 'Foo'` on a connection.

Use `MySqliDb`/`MySqliQuery`/`MySqliDdl` as the reference implementation. It is the most complete and the most exercised.

## Related pages

- [Data access](../guide/data-access.md)
- [Architecture](architecture.md)
- [Configuration reference](../reference/configuration.md)
