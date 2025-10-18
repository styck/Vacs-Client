//=================================================
// DLList C++ RAII Wrapper
// Copyright 2025
//=================================================
//
// Modern C++ wrapper for DLList C API
// Provides RAII memory management and type safety
//
//=================================================

#ifndef DLLIST_WRAPPER_H
#define DLLIST_WRAPPER_H

#include <memory>
#include <vector>
#include <stdexcept>
#include <windows.h>

// Forward declare C API
extern "C" {
    struct DLROOTPTRtag;
    typedef struct DLROOTPTRtag* LPDLROOTPTR;
    
    // C API functions
    LPDLROOTPTR InitDoubleLinkedList(long lEntryDataSize, long lEntriesNum,
        BOOL bSyncEnabled, BOOL bGrow, HANDLE hFile, LPSTR lpstrFName);
    void FreeDLListRootAll(LPDLROOTPTR* pRootPtr);
    long AddEntry(LPDLROOTPTR lpdlrPtr, LPSTR lpstrData, long lEntrySize);
    long GetFirstEntry(LPDLROOTPTR pRootPtrObj);
    long GetLastEntry(LPDLROOTPTR pRootPtrObj);
    long GetNextEntry(LPDLROOTPTR pRootPtrObj, long lCur);
    long GetPrevEntry(LPDLROOTPTR pRootPtrObj, long lCur);
    LPVOID GetEntryData(LPDLROOTPTR pRootPtrObj, long lEntryPos);
    int DelEntryPtr(LPDLROOTPTR lpdlrPtr, long lItemPos);
    long InsertEntry(LPDLROOTPTR lpdlrPtr, long lAfter, LPSTR lpstrData, 
        long lEntrySize, long lRelation);
    long GetEntryLinkState(LPDLROOTPTR pRootPtrObj, long lCur);
} // extern "C"

namespace DLList {

//=================================================
// Custom deleter for LPDLROOTPTR
//=================================================
struct RootDeleter {
    void operator()(LPDLROOTPTR ptr) const {
        if (ptr) {
            FreeDLListRootAll(&ptr);
        }
    }
};

//=================================================
// RAII Wrapper Class for DLList
//=================================================
class Wrapper {
private:
    std::unique_ptr<LPDLROOTPTR, RootDeleter> root_;
    LPDLROOTPTR rootPtr_;  // Cache for faster access

public:
    // Constructor
    Wrapper(long entryDataSize, long entriesNum, bool syncEnabled, 
            bool grow, HANDLE hFile = nullptr, LPSTR fname = nullptr)
        : rootPtr_(nullptr)
    {
        LPDLROOTPTR ptr = InitDoubleLinkedList(entryDataSize, entriesNum, 
            syncEnabled ? TRUE : FALSE, grow ? TRUE : FALSE, hFile, fname);
        
        if (!ptr) {
            throw std::runtime_error("Failed to initialize DLList");
        }
        
        rootPtr_ = ptr;
        root_ = std::unique_ptr<LPDLROOTPTR, RootDeleter>(
            new LPDLROOTPTR(ptr), RootDeleter());
    }

    // Disable copying
    Wrapper(const Wrapper&) = delete;
    Wrapper& operator=(const Wrapper&) = delete;

    // Enable moving
    Wrapper(Wrapper&&) noexcept = default;
    Wrapper& operator=(Wrapper&&) noexcept = default;

    // Get raw pointer for C API calls
    LPDLROOTPTR get() const { return rootPtr_; }
    
    // Check if valid
    explicit operator bool() const { return rootPtr_ != nullptr; }

    // Add entry with std::vector
    long addEntry(const std::vector<char>& data) {
        if (!rootPtr_) {
            throw std::runtime_error("Invalid DLList root");
        }
        return AddEntry(rootPtr_, const_cast<LPSTR>(data.data()), 
            static_cast<long>(data.size()));
    }

    // Add entry with raw pointer (for compatibility)
    long addEntry(LPSTR data, long size) {
        if (!rootPtr_) {
            throw std::runtime_error("Invalid DLList root");
        }
        return AddEntry(rootPtr_, data, size);
    }

    // Navigation methods
    long getFirst() const {
        if (!rootPtr_) return -1;
        return GetFirstEntry(rootPtr_);
    }

    long getLast() const {
        if (!rootPtr_) return -1;
        return GetLastEntry(rootPtr_);
    }

    long getNext(long current) const {
        if (!rootPtr_) return -1;
        return GetNextEntry(rootPtr_, current);
    }

    long getPrev(long current) const {
        if (!rootPtr_) return -1;
        return GetPrevEntry(rootPtr_, current);
    }

    // Get entry data
    void* getEntryData(long position) const {
        if (!rootPtr_) return nullptr;
        return GetEntryData(rootPtr_, position);
    }

    // Delete entry
    int deleteEntry(long position) {
        if (!rootPtr_) return -1;
        return DelEntryPtr(rootPtr_, position);
    }

    // Insert entry
    long insertEntry(long after, LPSTR data, long size, long relation) {
        if (!rootPtr_) return -1;
        return InsertEntry(rootPtr_, after, data, size, relation);
    }

    // Get link state
    long getLinkState(long position) const {
        if (!rootPtr_) return -1;
        return GetEntryLinkState(rootPtr_, position);
    }
};

} // namespace DLList

#endif // DLLIST_WRAPPER_H
