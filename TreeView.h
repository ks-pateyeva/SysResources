#pragma once
#include <Windows.h>
#include <commctrl.h>
#include <vector>

#define MAX_HEADING_LEN 25
#define ID_TREEVIEW 0x8801

typedef struct
{
	TCHAR* tchHeading;
	int tchLevel;
} Heading;

class TreeView
{
public:
	TreeView();
	HWND CreateATreeView(HINSTANCE hInst, HWND hwndParent);
	HTREEITEM AddItemToTree(HWND hwndTV, LPTSTR lpszItem, int nLevel);
private:
	std::vector<Heading> g_rgDocHeadings;
	BOOL InitTreeViewItems(HWND hwndTV);
};
