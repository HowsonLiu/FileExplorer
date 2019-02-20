#ifndef FRELEASER_H
#define FRELEASER_H

#include <Windows.h>
#include <ShlObj_core.h>
#include <vector>

//RAII机制

class FItemIdListReleaser {
public:
    explicit FItemIdListReleaser(ITEMIDLIST* idList) : m_idList(idList) {}
    ~FItemIdListReleaser() {if(m_idList) CoTaskMemFree(m_idList);}
private:
    ITEMIDLIST* m_idList;
};

class FComInterfaceReleaser {
public:
    explicit FComInterfaceReleaser(IUnknown* i) : m_ptr(i) {}
    ~FComInterfaceReleaser() {if(m_ptr) m_ptr->Release();}
private:
    IUnknown* m_ptr;
};

class FItemIdListVectorReleaser {
public:
    explicit FItemIdListVectorReleaser(const std::vector<ITEMIDLIST*>& idArray) : m_array(idArray) {}
    ~FItemIdListVectorReleaser() {
        for (ITEMIDLIST* item: m_array)
            CoTaskMemFree(item);
    }
    FItemIdListVectorReleaser& operator=(const FItemIdListVectorReleaser&) = delete;
private:
    const std::vector<ITEMIDLIST*>& m_array;
};

#endif // FRELEASER_H
