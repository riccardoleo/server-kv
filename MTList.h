//---------------------------------------------------------------------------
#ifndef MTListH
#define MTListH
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma warn -8091
#endif
//---------------------------------------------------------------------------
#include <vector>
#include <algorithm>
//---------------------------------------------------------------------------
typedef bool (*SortCompare)(void *Object1, void *Object2);
//---------------------------------------------------------------------------
template <typename T>
//---------------------------------------------------------------------------
class MTList
{
public:
    MTList();
    MTList(MTList<T> &Source);
    MTList(bool bAutoDeleteObjects);
    ~MTList();

    MTList & operator = (MTList<T> &Source);
    const T* operator [] (size_t Index) const;
    T* operator [] (size_t Index);

    void SetAutoDeleteObjects(bool bAutoDeleteObjects) {m_bAutoDeleteObjects = bAutoDeleteObjects;}

    void Clear();
    void ClearSavingPointers();
    void ClearDestroyingPointers();

    void Add(T *Object);
    T * Add(T Object);

    void Delete(size_t Index);
    void DeleteSavingObject(size_t Index);
    void DeleteDestroyingObject(size_t Index);

    void Exchange(size_t Index1, size_t Index2);

    T * First();
    T * Last();

    const T * First() const;
    const T * Last() const;

    T * Search(T Object);
    const T * Search(T Object) const;

    void CloneTo(MTList <T> *Destination) const;

    size_t IndexOf(T *Object) const;
    size_t IndexOf(T Object) const;

    void Insert(size_t Index, T *Object);
    T * Insert(size_t Index, T Object);

    void Replace(size_t Index, T *Object, bool bDeleteCurrentObject);
    void Replace(size_t Index, T Object, bool bDeleteCurrentObject);

    void Sort(SortCompare SortCompareFunction);

    size_t GetSize() const;
    #ifdef __BORLANDC__
    ssize_t GetCount() const;
    #else
    size_t GetCount() const;
    #endif

    T * GetItem(size_t Index);
    const T * GetItem(size_t Index) const;

    std::vector <T *> *GetPointersList() {return(&m_PointersList);}
private:
    bool m_bAutoDeleteObjects;

    std::vector <T *> m_PointersList;
};
//---------------------------------------------------------------------------
template <typename T>
MTList <T>::MTList()
{
    m_bAutoDeleteObjects = true;
}
//---------------------------------------------------------------------------
template <typename T>
MTList <T>::MTList(MTList<T> &Source)
{
    m_bAutoDeleteObjects = Source.m_bAutoDeleteObjects;

    Source.CloneTo(this);
}
//---------------------------------------------------------------------------
template <typename T>
MTList <T>::MTList(bool bAutoDeleteObjects)
{
    m_bAutoDeleteObjects = bAutoDeleteObjects;
}
//---------------------------------------------------------------------------
template <typename T>
MTList <T>::~MTList()
{
    Clear();
}
//---------------------------------------------------------------------------
template <typename T>
MTList<T> & MTList <T>::operator = (MTList<T> &Source)
{
    if (this == &Source)
    {
        return(*this);
    }

    m_bAutoDeleteObjects = Source.m_bAutoDeleteObjects;

    Clear();

    Source.CloneTo(this);

    return(*this);
}
//---------------------------------------------------------------------------
template <typename T>
const T* MTList<T>::operator [] (size_t Index) const
{
    return(GetItem(Index));
}
//---------------------------------------------------------------------------
template <typename T>
T* MTList<T>::operator [] (size_t Index)
{
    return(GetItem(Index));
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::Clear()
{
    size_t i;

    if (m_bAutoDeleteObjects)
    {
        for (i = 0; i < m_PointersList.size(); ++i)
        {
            delete static_cast<T *>(m_PointersList[i]);
        }
    }

    m_PointersList.clear();
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::ClearSavingPointers()
{
    m_PointersList.clear();
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::ClearDestroyingPointers()
{
    size_t i;

    for (i = 0; i < m_PointersList.size(); ++i)
    {
        delete static_cast<T *>(m_PointersList[i]);
    }

    m_PointersList.clear();
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::Add(T *Object)
{
    m_PointersList.push_back(Object);
}
//---------------------------------------------------------------------------
template <typename T>
T * MTList <T>::Add(T Object)
{
    T *NewObject = new T(Object);

    m_PointersList.push_back(NewObject);

    return(NewObject);
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::Delete(size_t Index)
{
    if (Index < 1 || Index > m_PointersList.size())
    {
        return;
    }

    if (m_bAutoDeleteObjects)
    {
        delete static_cast<T *>(m_PointersList[Index - 1]);
    }

    m_PointersList.erase(m_PointersList.begin() + Index - 1, m_PointersList.begin() + Index);
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::DeleteSavingObject(size_t Index)
{
    if (Index < 1 || Index > m_PointersList.size())
    {
        return;
    }

    m_PointersList.erase(m_PointersList.begin() + Index - 1, m_PointersList.begin() + Index);
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::DeleteDestroyingObject(size_t Index)
{
    if (Index < 1 || Index > m_PointersList.size())
    {
        return;
    }

    delete static_cast<T *>(m_PointersList[Index - 1]);

    m_PointersList.erase(m_PointersList.begin() + Index - 1, m_PointersList.begin() + Index);
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::Exchange(size_t Index1, size_t Index2)
{
    if (Index1 < 1 || Index1 > m_PointersList.size() || Index2 < 1 || Index2 > m_PointersList.size()) return;

    std::swap(m_PointersList[Index1 - 1], m_PointersList[Index2 - 1]);
}
//---------------------------------------------------------------------------
template <typename T>
T * MTList <T>::First()
{
    if (m_PointersList.size() == 0)
    {
        return(static_cast<T *>(0));
    }

    return(m_PointersList[0]);
}
//---------------------------------------------------------------------------
template <typename T>
T * MTList <T>::Last()
{
    if (m_PointersList.size() == 0)
    {
        return(static_cast<T *>(0));
    }

    return(m_PointersList[m_PointersList.size() - 1]);
}
//---------------------------------------------------------------------------
template <typename T>
const T * MTList <T>::First() const
{
    if (m_PointersList.size() == 0)
    {
        return(static_cast<T *>(0));
    }

    return(m_PointersList[0]);
}
//---------------------------------------------------------------------------
template <typename T>
const T * MTList <T>::Last() const
{
    if (m_PointersList.size() == 0)
    {
        return(static_cast<T *>(0));
    }

    return(m_PointersList[m_PointersList.size() - 1]);
}
//---------------------------------------------------------------------------
template <typename T>
T * MTList <T>::Search(T Object)
{
    size_t i;

    for (i = 0; i < m_PointersList.size(); ++i)
    {
        if (Object == *m_PointersList[i])
        {
            return(m_PointersList[i]);
        }
    }

    return(0);
}
//---------------------------------------------------------------------------
template <typename T>
const T * MTList <T>::Search(T Object) const
{
    size_t i;

    for (i = 0; i < m_PointersList.size(); ++i)
    {
        if (Object == *m_PointersList[i])
        {
            return(m_PointersList[i]);
        }
    }

    return(0);
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::CloneTo(MTList <T> *Destination) const
{
    T *Object, *NewObject;
    size_t i;

    Destination->Clear();

    for (i = 0; i < m_PointersList.size(); ++i)
    {
        Object = m_PointersList[i];

        NewObject = new T(*Object);

        Destination->Add(NewObject);
    }
}
//---------------------------------------------------------------------------
template <typename T>
size_t MTList <T>::IndexOf(T *Object) const
{
    size_t i;

    for (i = 0; i < m_PointersList.size(); ++i)
    {
        if (Object == m_PointersList[i])
        {
            return(i + 1);
        }
    }

    return(0);
}
//---------------------------------------------------------------------------
template <typename T>
size_t MTList <T>::IndexOf(T Object) const
{
    size_t i;

    for (i = 0; i < m_PointersList.size(); ++i)
    {
        if (Object == *m_PointersList[i])
        {
            return(i + 1);
        }
    }

    return(0);
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::Insert(size_t Index, T *Object)
{
    if (Index < 1)
    {
        return;
    }

    if (Index > m_PointersList.size())
    {
        m_PointersList.push_back(Object);
    }
    else
    {
        m_PointersList.insert(m_PointersList.begin() + Index - 1, Object);
    }
}
//---------------------------------------------------------------------------
template <typename T>
T * MTList <T>::Insert(size_t Index, T Object)
{
    T *NewObject = new T(Object);

    Insert(Index, NewObject);

    return(NewObject);
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::Replace(size_t Index, T *Object, bool bDeleteCurrentObject)
{
    if (Index < 1 || Index > m_PointersList.size())
    {
        return;
    }

    if (bDeleteCurrentObject)
    {
        delete static_cast<T *>(m_PointersList[Index - 1]);
    }

    m_PointersList[Index - 1] = Object;
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::Replace(size_t Index, T Object, bool bDeleteCurrentObject)
{
    Replace(Index, new T(Object), bDeleteCurrentObject);
}
//---------------------------------------------------------------------------
template <typename T>
void MTList <T>::Sort(SortCompare SortCompareFunction)
{
    std::sort(m_PointersList.begin(), m_PointersList.end(), SortCompareFunction);
}
//---------------------------------------------------------------------------
template <typename T>
size_t MTList <T>::GetSize() const
{
    return(m_PointersList.size());
}
//---------------------------------------------------------------------------
template <typename T>
#ifdef __BORLANDC__
ssize_t MTList <T>::GetCount() const
#else
size_t MTList <T>::GetCount() const
#endif
{
    return(m_PointersList.size());
}
//---------------------------------------------------------------------------
template <typename T>
T * MTList <T>::GetItem(size_t Index)
{
    if (Index < 1 || Index > m_PointersList.size())
    {
        return(0);
    }

    return(m_PointersList[Index - 1]);
}
//---------------------------------------------------------------------------
template <typename T>
const T * MTList <T>::GetItem(size_t Index) const
{
    if (Index < 1 || Index > m_PointersList.size())
    {
        return(0);
    }

    return(m_PointersList[Index - 1]);
}
//---------------------------------------------------------------------------
#endif
