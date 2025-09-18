## ON-THE-FLY COMMENT TABLE MIGRATION (DAO / MFC)

### Goal
- Add a new associated table `comment(commentID, comment)` and replace the legacy free-text column ("more") with a keyed LONG column (`comment`) in the main table (`table`).
- Support both database versions transparently: when opening an `.mdb`, detect missing parts, create what’s missing, and migrate data from legacy text to the keyed table.

### Approach
1) Detect main table and field layout in `CdbTable::open_tables()` after existing version checks and before opening associated tables.
2) Ensure a LONG key column exists in the main table at `CH_COMMENT_KEY` header name (currently `"comment"`). Create it if missing.
3) Ensure associated `comment` table exists (create if missing), then ensure the relation `table_comment` exists.
4) Migrate legacy free-text from column `"more"` (if present) into the `comment` table and set the new key in the main table, without removing the legacy field.

This avoids introducing a dummy version-bump column while providing robust on-the-fly migration.

---

### Edits

1) Add a migration helper declaration

File: `dbWave64/dbTable.h`
```cpp
// in class CdbTable
BOOL ensure_comment_schema_and_migrate();
```

2) Call the migration helper during open

File: `dbWave64/dbTable.cpp` (in `CdbTable::open_tables()`, after the existing `field_count` upgrade switch and before opening associated tables)
```cpp
// After validating main table schema and possibly adding missing columns
try
{
    if (!ensure_comment_schema_and_migrate())
        return FALSE;
}
catch (CDaoException* e)
{
    AfxMessageBox(e->m_pErrorInfo->m_strDescription);
    e->Delete();
    return FALSE;
}
```

3) Implement the migration helper

File: `dbWave64/dbTable.cpp`
```cpp
BOOL CdbTable::ensure_comment_schema_and_migrate()
{
    const CString cs_table = _T("table");

    // Ensure main table has LONG column for comment key
    bool hasCommentLong = false;
    try
    {
        CDaoRecordset rs(this);
        rs.Open(dbOpenTable, cs_table);
        CDaoFieldInfo fi;
        rs.GetFieldInfo(m_main_table_set.m_desc[CH_COMMENT_KEY].header_name, fi); // throws if missing
        hasCommentLong = (fi.m_nType == dbLong);
        rs.Close();
    }
    catch (CDaoException* e)
    {
        e->Delete();
        hasCommentLong = false;
    }

    if (!hasCommentLong)
    {
        CDaoTableDef table_def(this);
        table_def.Open(cs_table);
        table_def.CreateField(m_main_table_set.m_desc[CH_COMMENT_KEY].header_name, dbLong, 4, 0);
        table_def.Close();
    }

    // Ensure associated table exists
    try
    {
        open_associated_table(&m_comment_set);
    }
    catch (CDaoException* e)
    {
        e->Delete();
        m_comment_set.create_index_table(_T("comment"), _T("comment"), _T("commentID"), 100, this);
        open_associated_table(&m_comment_set);
    }

    // Ensure relation exists
    try
    {
        const CString cs_rel = _T("table_comment");
        const long l_attr = dbRelationDontEnforce; // consistent with other relations in this project
        CreateRelation(cs_rel, _T("comment"), cs_table, l_attr, _T("commentID"),
                       m_main_table_set.m_desc[CH_COMMENT_KEY].header_name);
    }
    catch (CDaoException* e)
    {
        // Relation may already exist; ignore
        e->Delete();
    }

    // Migrate legacy free-text from column "more" if present
    bool hasLegacyMore = false;
    try
    {
        CDaoRecordset rs(this);
        rs.Open(dbOpenTable, cs_table);
        CDaoFieldInfo fiMore;
        rs.GetFieldInfo(_T("more"), fiMore); // throws if not present
        hasLegacyMore = (fiMore.m_nType == dbMemo || fiMore.m_nType == dbText);
        rs.Close();
    }
    catch (CDaoException* e)
    {
        e->Delete();
        hasLegacyMore = false;
    }

    if (hasLegacyMore)
    {
        // Iterate records and fill the new key from legacy text, if key not set
        m_main_table_set.Open(dbOpenDynaset, cs_table);
        if (!m_main_table_set.IsEOF()) m_main_table_set.MoveFirst();
        while (!m_main_table_set.IsEOF())
        {
            // Read legacy text
            COleVariant v_text;
            m_main_table_set.GetFieldValue(_T("more"), v_text);
            CString legacyText;
            if (v_text.vt == VT_BSTR)
                legacyText = V_BSTRT(&v_text);

            // Read current key value
            COleVariant v_key;
            m_main_table_set.GetFieldValue(m_main_table_set.m_desc[CH_COMMENT_KEY].header_name, v_key);
            const long existingKey = (v_key.vt == VT_I4 || v_key.vt == VT_I2) ? v_key.lVal : 0;

            if (!legacyText.IsEmpty() && existingKey == 0)
            {
                const long id = m_comment_set.get_string_in_linked_table(legacyText);
                m_main_table_set.Edit();
                m_main_table_set.SetFieldValue(m_main_table_set.m_desc[CH_COMMENT_KEY].header_name, COleVariant(id));
                m_main_table_set.Update();
            }

            m_main_table_set.MoveNext();
        }
        m_main_table_set.Close();
    }

    return TRUE;
}
```

4) New database creation: make `comment` a LONG key, not MEMO

File: `dbWave64/dbTable.cpp` (`CdbTable::create_main_table`)
```cpp
// Replace the legacy memo creation for index 5 with a LONG key field
int i = CH_COMMENT_KEY; // was hardcoded i = 5 with dbMemo
table_def.CreateField(m_main_table_set.m_desc[i].header_name, dbLong, 4, 0);
```

5) Fix descriptor mapping for comment vs sex

File: `dbWave64/dbTable.cpp` (`CdbTable::get_record_item_descriptor`)
```cpp
case CH_SEX_KEY:
    p_desc->pdata_item = &m_main_table_set.m_sex_key;
    p_desc->p_linked_set = &m_sex_set;
    ASSERT(p_desc->data_code_number == FIELD_IND_TEXT);
    break;

case CH_COMMENT_KEY:
    p_desc->pdata_item = &m_main_table_set.m_comment_key;
    p_desc->p_linked_set = &m_comment_set;
    ASSERT(p_desc->data_code_number == FIELD_IND_TEXT);
    break;
```

6) Store the keyed comment from wave format using the correct associated table

File: `dbWave64/dbTable.cpp` (`CdbTable::transfer_wave_format_data_to_record`)
```cpp
// Set keyed comment from p_wave_format->cs_more_comment
m_main_table_set.m_comment_key = m_comment_set.get_string_in_linked_table(p_wave_format->cs_more_comment);
```

---

### Notes
- The migration keeps the legacy `more` column for backward compatibility; no data is deleted or renamed.
- The on-the-fly helper is idempotent: it will quietly succeed if the column/table/relation already exist.
- We intentionally use MFC/DAO exception handling (`catch (CDaoException* e) { ... e->Delete(); }`) to match the existing project style.

### Test Plan
- Open a pre-change database (with `more` memo, without `comment` table/column):
  - `open_tables()` should succeed; `comment` table should be created; `comment` LONG column should be added; relation should exist; keys should be populated from `more`.
- Open a fully migrated database: no changes should occur.
- Create a brand-new database: `create_main_table()` should create `comment` as LONG and `create_associated_tables()` already creates the `comment` index table.


