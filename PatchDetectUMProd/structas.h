#pragma once

typedef struct _MMPFN
{
	ULONG64 pad0;
	ULONG64 pad1;
	ULONG64 pad2;
	ULONG64 ShareCount;
	ULONG64 pad3;
	ULONG64 Info;
} MMPFN, * PMMPFN;

typedef union _PTE
{
	unsigned __int64 value;
	struct
	{
		unsigned __int64 valid : 1;          // Must be 1, region invalid if 0.
		unsigned __int64 rw : 1;               // If 0, writes not allowed.
		unsigned __int64 user_supervisor : 1;  // If 0, user-mode accesses not allowed. 
		unsigned __int64 page_write : 1;        // Determines the memory type used to access the memory.
		unsigned __int64 page_cache : 1;       // Determines the memory type used to access the memory.
		unsigned __int64 accessed : 1;         // If 0, this entry has not been used for translation.
		unsigned __int64 dirty : 1;             // If 0, the memory backing this page has not been written to.
		unsigned __int64 page_access_type : 1;  // Determines the memory type used to access the memory.
		unsigned __int64 global : 1;            // If 1 and the PGE bit of CR4 is set, translations are global.
		unsigned __int64 ignored2 : 3;
		unsigned __int64 pfn : 36;             // The page frame number of the backing physical page.
		unsigned __int64 reserved : 4;
		unsigned __int64 ignored3 : 7;
		unsigned __int64 protect_key : 4;       // If the PKE bit of CR4 is set, determines the protection key.
		unsigned __int64 nx : 1;               // If 1, instruction fetches not allowed.
	};
} PTE, * PPTE;