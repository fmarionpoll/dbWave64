#include "StdAfx.h"
#include "DlgAbout.h"

#include <afxdao.h>
#include "Fileversion.h"

DlgAbout::DlgAbout() : CDialogEx(IDD)
{
}

void DlgAbout::DoDataExchange(CDataExchange* p_dx)
{
	CDialogEx::DoDataExchange(p_dx);
}

BEGIN_MESSAGE_MAP(DlgAbout, CDialogEx)
END_MESSAGE_MAP()

#pragma warning(disable : 4995)
BOOL DlgAbout::OnInitDialog()
{
	CDialog::OnInitDialog();

	try
	{
		CDaoWorkspace dao_workspace;
		dao_workspace.Create(_T("VersionWorkspace"), _T("Admin"), _T(""));
		GetDlgItem(IDC_VERSION)->SetWindowText(CDaoWorkspace::GetVersion());
	}
	catch (CDaoException* e)
	{
		e->ReportError();
		e->Delete();
	}

	CFileVersion c_fv;
	TCHAR module_path[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, module_path, MAX_PATH);
	c_fv.open(module_path);
	GetDlgItem(IDC_DBWAVEVERSION)->SetWindowText(c_fv.get_product_version());
	GetDlgItem(IDC_STATIC7)->SetWindowText(c_fv.get_legal_copyright());

	return TRUE;
}

