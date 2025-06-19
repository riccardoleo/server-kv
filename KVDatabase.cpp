//---------------------------------------------------------------------------
#include "KVDatabase.h"
//---------------------------------------------------------------------------
#define PARAMETERS_TABLE_NAME     "SETTINGS"
//---------------------------------------------------------------------------
KVDatabase::KVDatabase(const string &sDatabaseName, const bool bUniqueID, const SortType ST, const uint64_t MaxKeys)
{
    string sKVTableName = bUniqueID ? "KVU" : "KV";  // KVU = Key value unique (ID)
    string sIDField = bUniqueID ? "ID" : "ROWID";
    string sSortFields;
    string sSQL;

    switch (ST)
    {
        case stChronological:
        default:
            sSortFields = sIDField;
            break;

        case stAlphabetical:
            sSortFields = "KEY";
            break;

        case stValue:
            sSortFields = "VALUE, KEY";
            break;

        case stNumericValue:
            sSortFields = "CAST(VALUE AS INTEGER), KEY";
            break;
    }

    m_MaxKeys = MaxKeys;

    m_DB.open(sDatabaseName.c_str());
    m_DB.execQuery("PRAGMA journal_mode=WAL");

    if (! m_DB.tableExists(sKVTableName.c_str()))
    {
        if (bUniqueID)
        {
            sSQL = string("CREATE TABLE IF NOT EXISTS ") + sKVTableName + "(ID INTEGER PRIMARY KEY AUTOINCREMENT, KEY CHAR NOT NULL, VALUE CHAR NOT NULL)";
        }
        else
        {
            sSQL = string("CREATE TABLE IF NOT EXISTS ") + sKVTableName + "(ROWID INTEGER PRIMARY KEY, KEY CHAR NOT NULL, VALUE CHAR NOT NULL)";
        }

        m_DB.execQuery(sSQL.c_str());

        sSQL = string("CREATE UNIQUE INDEX ") + sKVTableName + "_INDEX ON " + sKVTableName + " (KEY)";
        m_DB.execQuery(sSQL.c_str());
    }

    if (! m_DB.tableExists(PARAMETERS_TABLE_NAME))
    {
        sSQL = string("CREATE TABLE IF NOT EXISTS ") + PARAMETERS_TABLE_NAME + "(PARAMETER CHAR NOT NULL COLLATE NOCASE PRIMARY KEY, VALUE CHAR NOT NULL)";
        m_DB.execQuery(sSQL.c_str());
    }

    sSQL = "BEGIN TRANSACTION";
    m_BeginStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = "COMMIT";
    m_CommitStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = "ROLLBACK";
    m_RollbackStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT COUNT(*) FROM ") + sKVTableName;
    m_CountStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT MAX(") + sIDField + ") FROM " + sKVTableName;
    m_MaxIDStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT MIN(") + sIDField + "), MAX(" + sIDField + ") FROM " + sKVTableName;
    m_RangeIDStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("DELETE FROM ") + sKVTableName;
    m_ClearStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("INSERT OR REPLACE INTO ") + sKVTableName + " (KEY, VALUE) VALUES (?,?)";
    m_InsertStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT VALUE FROM ") + sKVTableName + " WHERE KEY = ?";
    m_GetStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("UPDATE ") + sKVTableName + " SET KEY = ? WHERE KEY = ?";
    m_RenameStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("DELETE FROM ") + sKVTableName + " WHERE KEY = ?";
    m_DeleteStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT KEY FROM ") + sKVTableName + " ORDER BY " + sSortFields;
    m_ListStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT KEY FROM ") + sKVTableName + " ORDER BY " + sSortFields + " LIMIT ?";
    m_ListStatementMax = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT KEY FROM ") + sKVTableName + " WHERE KEY LIKE ? ORDER BY " + sSortFields;
    m_ListStatementByFilter = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT KEY FROM ") + sKVTableName + " WHERE KEY LIKE ? ORDER BY " + sSortFields + " LIMIT ?";
    m_ListStatementByFilterMax = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT KEY FROM ") + sKVTableName + " ORDER BY " + sSortFields + " DESC";
    m_ReverseListStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT KEY FROM ") + sKVTableName + " ORDER BY " + sSortFields + " DESC LIMIT ?";
    m_ReverseListStatementMax = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT KEY FROM ") + sKVTableName + " WHERE KEY LIKE ? ORDER BY " + sSortFields + " DESC";
    m_ReverseListStatementByFilter = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT KEY FROM ") + sKVTableName + " WHERE KEY LIKE ? ORDER BY " + sSortFields + " DESC LIMIT ?";
    m_ReverseListStatementByFilterMax = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT ") + sIDField + ", KEY, VALUE FROM " + sKVTableName + " WHERE " + sIDField + ">= ? ORDER BY " + sIDField + " LIMIT ?";
    m_GetKVInfoSetStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("DELETE FROM ") + sKVTableName + " WHERE " + sIDField + " IN (SELECT " + sIDField + " FROM " + sKVTableName + " ORDER BY " + sSortFields + " LIMIT ?)";
    m_SetMaxKeysStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("INSERT OR REPLACE INTO ") + PARAMETERS_TABLE_NAME + " (PARAMETER, VALUE) VALUES (?,?)";
    m_InsertParameterStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("SELECT VALUE FROM ") + PARAMETERS_TABLE_NAME + " WHERE PARAMETER = ?";
    m_GetParameterStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("DELETE FROM ") + PARAMETERS_TABLE_NAME + " WHERE PARAMETER = ?";
    m_DeleteParameterStatement = m_DB.compileStatement(sSQL.c_str());

    sSQL = string("VACUUM");
    m_VacuumStatement = m_DB.compileStatement(sSQL.c_str());
}
//---------------------------------------------------------------------------
KVDatabase::~KVDatabase()
{
    m_BeginStatement.finalize();
    m_CommitStatement.finalize();
    m_RollbackStatement.finalize();

    m_CountStatement.finalize();
    m_MaxIDStatement.finalize();
    m_RangeIDStatement.finalize();
    m_ClearStatement.finalize();
    m_InsertStatement.finalize();
    m_GetStatement.finalize();
    m_RenameStatement.finalize();
    m_DeleteStatement.finalize();
    m_ListStatement.finalize();
    m_ListStatementMax.finalize();
    m_ListStatementByFilter.finalize();
    m_ListStatementByFilterMax.finalize();
    m_ReverseListStatement.finalize();
    m_ReverseListStatementMax.finalize();
    m_ReverseListStatementByFilter.finalize();
    m_ReverseListStatementByFilterMax.finalize();

    m_SetMaxKeysStatement.finalize();

    m_InsertParameterStatement.finalize();
    m_GetParameterStatement.finalize();
    m_DeleteParameterStatement.finalize();

    m_VacuumStatement.finalize();

    m_Query.finalize();

    m_DB.close();
}
//---------------------------------------------------------------------------
void KVDatabase::Begin()
{
    m_BeginStatement.execQuery();
}
//---------------------------------------------------------------------------
void KVDatabase::Commit()
{
    m_CommitStatement.execQuery();
}
//---------------------------------------------------------------------------
void KVDatabase::Rollback()
{
    m_RollbackStatement.execQuery();
}
//---------------------------------------------------------------------------
uint64_t KVDatabase::Count()
{
    uint64_t Count;

    m_Query = m_CountStatement.execQuery();
    Count = m_Query.getInt64Field(0);

    m_CountStatement.reset();

    return(Count);
}
//---------------------------------------------------------------------------
uint64_t KVDatabase::MaxID()
{
    uint64_t MaxID;

    m_Query = m_MaxIDStatement.execQuery();
    MaxID = m_Query.getInt64Field(0);

    m_MaxIDStatement.reset();

    return(MaxID);
}
//---------------------------------------------------------------------------
void KVDatabase::RangeID(uint64_t &MinID, uint64_t &MaxID)
{
    m_Query = m_RangeIDStatement.execQuery();
    MinID = m_Query.getInt64Field(0);
    MaxID = m_Query.getInt64Field(1);

    m_RangeIDStatement.reset();
}
//---------------------------------------------------------------------------
void KVDatabase::Clear()
{
    m_ClearStatement.execQuery();
    m_ClearStatement.reset();
}
//---------------------------------------------------------------------------
void KVDatabase::Set(const string &sKey, const string &sValue)
{
    if (m_MaxKeys != 0)
    {
        SetMaxKeys(m_MaxKeys - 1);
    }

    m_InsertStatement.bind(1, sKey.c_str());
    m_InsertStatement.bind(2, sValue.c_str());
    m_InsertStatement.execQuery();
    m_InsertStatement.reset();
}
//---------------------------------------------------------------------------
void KVDatabase::Set(const string &sKey, const char *cValue)
{
    const string sValue = cValue;

    Set(sKey, sValue);
}
//---------------------------------------------------------------------------
bool KVDatabase::Get(const string &sKey, string &sValue)
{
    bool bFound = false;

    m_GetStatement.bind(1, sKey.c_str());
    m_Query = m_GetStatement.execQuery();

    if (! m_Query.eof())
    {
        bFound = true;
        sValue = m_Query.getStringField("VALUE");
    }
    else
    {
        sValue = "";
    }

    m_GetStatement.reset();

    return(bFound);
}
//---------------------------------------------------------------------------
bool KVDatabase::Rename(const string &sOldKey, const string &sNewKey)
{
    if (! Exists(sOldKey) || Exists(sNewKey))
    {
        return(false);
    }

    m_RenameStatement.bind(1, sNewKey.c_str());
    m_RenameStatement.bind(2, sOldKey.c_str());
    m_RenameStatement.execQuery();
    m_RenameStatement.reset();

    return(true);
}
//---------------------------------------------------------------------------
void KVDatabase::Delete(const string &sKey)
{
    m_DeleteStatement.bind(1, sKey.c_str());
    m_DeleteStatement.execQuery();
    m_DeleteStatement.reset();
}
//---------------------------------------------------------------------------
bool KVDatabase::Exists(const string &sKey)
{
    string sTempValue;

    return(Get(sKey, sTempValue));
}
//---------------------------------------------------------------------------
bool KVDatabase::GetFirst(string &sKey)
{
    MTList <string> Keys;

    sKey = "";

    GetList(&Keys, 1);

    if (Keys.GetCount() > 0)
    {
        sKey = *Keys.GetItem(1);
        return(true);
    }
    else
    {
        return(false);
    }
}
//---------------------------------------------------------------------------
bool KVDatabase::GetLast(string &sKey)
{
    MTList <string> Keys;

    sKey = "";

    GetReverseList(&Keys, 1);

    if (Keys.GetCount() > 0)
    {
        sKey = *Keys.GetItem(1);
        return(true);
    }
    else
    {
        return(false);
    }
}
//---------------------------------------------------------------------------
void KVDatabase::GetList(MTList <string> *Keys, const uint64_t MaxValues)
{
    if (MaxValues == 0)
    {
        m_Query = m_ListStatement.execQuery();
    }
    else
    {
        m_ListStatementMax.bind(1, (long long int)MaxValues);
        m_Query = m_ListStatementMax.execQuery();
    }

    Keys->Clear();

    while (! m_Query.eof())
    {
        Keys->Add(m_Query.getStringField("KEY"));
        m_Query.nextRow();
    }

    if (MaxValues == 0)
    {
        m_ListStatement.reset();
    }
    else
    {
        m_ListStatementMax.reset();
    }
}
//---------------------------------------------------------------------------
void KVDatabase::GetFilteredList(const string &sFilter, MTList <string> *Keys, const uint64_t MaxValues)
{
    if (MaxValues == 0)
    {
        m_ListStatementByFilter.bind(1, sFilter.c_str());
        m_Query = m_ListStatementByFilter.execQuery();
    }
    else
    {
        m_ListStatementByFilterMax.bind(1, sFilter.c_str());
        m_ListStatementByFilterMax.bind(2, (long long int)MaxValues);
        m_Query = m_ListStatementByFilterMax.execQuery();
    }

    Keys->Clear();

    while (! m_Query.eof())
    {
        Keys->Add(m_Query.getStringField("KEY"));
        m_Query.nextRow();
    }

    if (MaxValues == 0)
    {
        m_ListStatementByFilter.reset();
    }
    else
    {
        m_ListStatementByFilterMax.reset();
    }
}
//---------------------------------------------------------------------------
void KVDatabase::GetReverseList(MTList <string> *Keys, const uint64_t MaxValues)
{
    if (MaxValues == 0)
    {
        m_Query = m_ReverseListStatement.execQuery();
    }
    else
    {
        m_ReverseListStatementMax.bind(1, (long long int)MaxValues);
        m_Query = m_ReverseListStatementMax.execQuery();
    }

    Keys->Clear();

    while (! m_Query.eof())
    {
        Keys->Add(m_Query.getStringField("KEY"));
        m_Query.nextRow();
    }

    if (MaxValues == 0)
    {
        m_ReverseListStatement.reset();
    }
    else
    {
        m_ReverseListStatementMax.reset();
    }
}
//---------------------------------------------------------------------------
void KVDatabase::GetReverseFilteredList(const string &sFilter, MTList <string> *Keys, const uint64_t MaxValues)
{
    if (MaxValues == 0)
    {
        m_ReverseListStatementByFilter.bind(1, sFilter.c_str());
        m_Query = m_ReverseListStatementByFilter.execQuery();
    }
    else
    {
        m_ReverseListStatementByFilterMax.bind(1, sFilter.c_str());
        m_ReverseListStatementByFilterMax.bind(2, (long long int)MaxValues);
        m_Query = m_ReverseListStatementByFilterMax.execQuery();
    }

    Keys->Clear();

    while (! m_Query.eof())
    {
        Keys->Add(m_Query.getStringField("KEY"));
        m_Query.nextRow();
    }

    if (MaxValues == 0)
    {
        m_ReverseListStatementByFilter.reset();
    }
    else
    {
        m_ReverseListStatementByFilterMax.reset();
    }
}
//---------------------------------------------------------------------------
void KVDatabase::GetKVInfoSet(const uint64_t &FromID, const uint64_t &MaxValues, MTList <KVInfo> *KVInfoList)
{
    KVInfo *Info;

    m_GetKVInfoSetStatement.bind(1, (long long int)FromID);
    m_GetKVInfoSetStatement.bind(2, (long long int)MaxValues);
    m_Query = m_GetKVInfoSetStatement.execQuery();

    KVInfoList->Clear();

    while (! m_Query.eof())
    {
        Info = new KVInfo();

        Info->ID = (uint64_t)m_Query.getInt64Field(0);
        Info->sKey = m_Query.getStringField(1);
        Info->sValue = m_Query.getStringField(2);

        KVInfoList->Add(Info);

        m_Query.nextRow();
    }

    m_GetKVInfoSetStatement.reset();
}
//---------------------------------------------------------------------------
void KVDatabase::SetMaxKeys(const uint64_t MaxKeys)
{
    uint64_t NoKeys = Count();

    if (NoKeys <= MaxKeys)
    {
        return;
    }

    m_SetMaxKeysStatement.bind(1, (long long int)(NoKeys - MaxKeys));
    m_SetMaxKeysStatement.execQuery();
    m_SetMaxKeysStatement.reset();
}
//---------------------------------------------------------------------------
void KVDatabase::SetParameter(const string &sParameter, const string &sValue)
{
    m_InsertParameterStatement.bind(1, sParameter.c_str());
    m_InsertParameterStatement.bind(2, sValue.c_str());
    m_InsertParameterStatement.execQuery();
    m_InsertParameterStatement.reset();
}
//---------------------------------------------------------------------------
void KVDatabase::SetParameter(const string &sParameter, const char *cValue)
{
    const string sValue = cValue;

    SetParameter(sParameter, sValue);
}
//---------------------------------------------------------------------------
bool KVDatabase::GetParameter(const string &sParameter, string &sValue)
{
    bool bFound = false;

    m_GetParameterStatement.bind(1, sParameter.c_str());
    m_Query = m_GetParameterStatement.execQuery();

    if (! m_Query.eof())
    {
        bFound = true;
        sValue = m_Query.getStringField("VALUE");
    }
    else
    {
        sValue = "";
    }

    m_GetParameterStatement.reset();

    return(bFound);
}
//---------------------------------------------------------------------------
void KVDatabase::DeleteParameter(const string &sParameter)
{
    m_DeleteParameterStatement.bind(1, sParameter.c_str());
    m_DeleteParameterStatement.execQuery();
    m_DeleteParameterStatement.reset();
}
//---------------------------------------------------------------------------
void KVDatabase::Pack()
{
    m_VacuumStatement.execQuery();
    m_VacuumStatement.reset();
}
//---------------------------------------------------------------------------
