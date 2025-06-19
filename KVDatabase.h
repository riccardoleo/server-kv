//---------------------------------------------------------------------------
#ifndef KeyValueDBH
#define KeyValueDBH
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
#include "CppSQLite3.h"
#include "MTList.h"
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
class KVDatabase
{
public:
    enum SortType
    {
        stChronological,
        stAlphabetical,
        stValue,
        stNumericValue
    };

    typedef struct
    {
        uint64_t ID;
        string sKey;
        string sValue;
    }
    KVInfo;

    KVDatabase(const string &sDatabaseName, const bool bUniqueID, const SortType ST = KVDatabase::stChronological, const uint64_t MaxKeys = 0);
    ~KVDatabase();

    void Begin();
    void Commit();
    void Rollback();

    uint64_t Count();
    uint64_t MaxID();
    void RangeID(uint64_t &MinID, uint64_t &MaxID);

    void Clear();

    void Set(const string &sKey, const string &sValue);
    void Set(const string &sKey, const char *cValue);
    bool Get(const string &sKey, string &sValue);
    bool Rename(const string &sOldKey, const string &sNewKey);
    void Delete(const string &sKey);
    bool Exists(const string &sKey);

    bool GetFirst(string &sKey);
    bool GetLast(string &sKey);

    void GetList(MTList <string> *Keys, const uint64_t MaxValues = 0);
    void GetFilteredList(const string &sFilter, MTList <string> *Keys, const uint64_t MaxValues = 0);
    void GetReverseList(MTList <string> *Keys, const uint64_t MaxValues = 0);
    void GetReverseFilteredList(const string &sFilter, MTList <string> *Keys, const uint64_t MaxValues = 0);

    void GetKVInfoSet(const uint64_t &FromID, const uint64_t &MaxValues, MTList <KVInfo> *KVInfoList);

    void SetMaxKeys(uint64_t MaxKeys);

    void SetParameter(const string &sParameter, const string &sValue);
    void SetParameter(const string &sParameter, const char *cValue);
    bool GetParameter(const string &sParameter, string &sValue);
    void DeleteParameter(const string &sParameter);

    void Pack();
private:
    uint64_t m_MaxKeys;

    CppSQLite3DB m_DB;

    CppSQLite3Statement m_BeginStatement;
    CppSQLite3Statement m_CommitStatement;
    CppSQLite3Statement m_RollbackStatement;

    CppSQLite3Statement m_CountStatement;
    CppSQLite3Statement m_MaxIDStatement;
    CppSQLite3Statement m_RangeIDStatement;
    CppSQLite3Statement m_ClearStatement;
    CppSQLite3Statement m_InsertStatement;
    CppSQLite3Statement m_GetStatement;
    CppSQLite3Statement m_RenameStatement;
    CppSQLite3Statement m_DeleteStatement;
    CppSQLite3Statement m_ListStatement;
    CppSQLite3Statement m_ListStatementMax;
    CppSQLite3Statement m_ListStatementByFilter;
    CppSQLite3Statement m_ListStatementByFilterMax;
    CppSQLite3Statement m_ReverseListStatement;
    CppSQLite3Statement m_ReverseListStatementMax;
    CppSQLite3Statement m_ReverseListStatementByFilter;
    CppSQLite3Statement m_ReverseListStatementByFilterMax;

    CppSQLite3Statement m_GetKVInfoSetStatement;

    CppSQLite3Statement m_SetMaxKeysStatement;

    CppSQLite3Statement m_InsertParameterStatement;
    CppSQLite3Statement m_GetParameterStatement;
    CppSQLite3Statement m_DeleteParameterStatement;

    CppSQLite3Statement m_VacuumStatement;

    CppSQLite3Query m_Query;
};
//---------------------------------------------------------------------------
#endif
