//====================================================
// DLList Software Audio Console - Double linked List
// Header File
//
// Copyright 1998 CorTek Software, Inc.
//====================================================

// Relation definitions
//---------------------
#define DL_RELATION_CHILD       0x00000001
#define DL_RELATION_PEER        0x00000002

// Link state definitions
//-----------------------
#define DLENTRY_NEXT            0x00000001
#define DLENTRY_PREV            0x00000002
#define DLENTRY_CHILD           0x00000004
#define DLENTRY_PARENT          0x00000008

// important ?!
#ifdef DLLIST_MAIN

// This header is obsolete. Use ../inc/DLLIST.H instead.
#define IQS_DLLIST_VERSION      MAKELONG(1,0)

#define DEFAULT_IDX_PAGE_SIZE   8192 // Bytes
