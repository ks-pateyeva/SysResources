#include "TreeView.h"
#include "CPUInfo.h"
#include <commctrl.h>
#include <string>
#include <atlconv.h>

#ifndef _UNICODE
#define _UNICODE
#endif

TreeView::TreeView()
{
	Heading heading;
	heading.tchHeading = (TCHAR *)L"CPU";
	heading.tchLevel = 0;
	g_rgDocHeadings.push_back(heading);
	DWORD coreCount = CpuInfo::GetProcessorCount();
	heading.tchLevel = 1;
	//for (int i = 0; i < coreCount; i++)
	//{
	////	 TODO fix Unicode trouble
	//	std::wstring cpuStr(L"CPU ");
	//	cpuStr += std::wstring(std::to_wstring(static_cast<int>(i)));
	//	TCHAR* t = const_cast<wchar_t*>(cpuStr.c_str());
	//	heading.tchHeading = t;

	//	g_rgDocHeadings.push_back(heading);
	//}
}

HWND TreeView::CreateATreeView(HINSTANCE hInst, HWND hwndParent)
{
	RECT rcClient;

	GetClientRect(hwndParent, &rcClient);

	HWND hwndTV; // handle to tree-view control 

	hwndTV = CreateWindowEx(0,
	                        WC_TREEVIEW,
	                        TEXT("Tree View"),
	                        WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES,
	                        10,
	                        10,
	                        100,
	                        250,
	                        hwndParent,
	                        (HMENU)ID_TREEVIEW,
	                        hInst,
	                        NULL);

	if (!InitTreeViewItems(hwndTV))
	{
		DestroyWindow(hwndTV);
		return FALSE;
	}
	return hwndTV;
}

HTREEITEM TreeView::AddItemToTree(HWND hwndTV, LPTSTR lpszItem, int nLevel)
{
	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST;
	static HTREEITEM hPrevRootItem = NULL;
	static HTREEITEM hPrevLev2Item = NULL;
	HTREEITEM hti;

	tvi.mask = TVIF_TEXT | TVIF_IMAGE
		| TVIF_SELECTEDIMAGE | TVIF_PARAM;

	// Set the text of the item. 
	tvi.pszText = lpszItem;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);

	// Save the heading level in the item's application-defined 
	// data area. 
	tvi.lParam = (LPARAM)nLevel;
	tvins.item = tvi;
	tvins.hInsertAfter = hPrev;

	// Set the parent item based on the specified level. 
	if (nLevel == 1)
		tvins.hParent = TVI_ROOT;
	else if (nLevel == 2)
		tvins.hParent = hPrevRootItem;
	else
		tvins.hParent = hPrevLev2Item;

	// Add the item to the tree-view control. 
	hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM,
	                               0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	if (hPrev == NULL)
		return NULL;

	// Save the handle to the item. 
	if (nLevel == 1)
		hPrevRootItem = hPrev;
	else if (nLevel == 2)
		hPrevLev2Item = hPrev;

	// The new item is a child item. Give the parent item a 
	// closed folder bitmap to indicate it now has child items. 
	if (nLevel > 1)
	{
		hti = TreeView_GetParent(hwndTV, hPrev);
		tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvi.hItem = hti;
		TreeView_SetItem(hwndTV, &tvi);
	}

	return hPrev;
}

BOOL TreeView::InitTreeViewItems(HWND hwndTV)
{
	HTREEITEM hti;

	for (int i = 0; i < g_rgDocHeadings.size(); i++)
	{
		// Add the item to the tree-view control. 
		hti = AddItemToTree(hwndTV, (LPTSTR)g_rgDocHeadings[i].tchHeading,
		                    g_rgDocHeadings[i].tchLevel);
		if (hti == NULL)
			return FALSE;
	}

	return TRUE;
}
