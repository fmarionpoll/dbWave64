#include "StdAfx.h"
#include "DlgAcquisitionStub.h"
#include "resource.h"
#include "dbWaveDoc.h"
#include "DlgImportFiles.h"
#include <shellapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DlgAcquisitionStub* DlgAcquisitionStub::s_instance_ = nullptr;

IMPLEMENT_DYNAMIC(DlgAcquisitionStub, CDialogEx)

DlgAcquisitionStub::DlgAcquisitionStub()
	: CDialogEx(IDD_VIEWADCONTINUOUS)
{
	m_bAutoDelete = TRUE;
}

void DlgAcquisitionStub::Show(CWnd* p_parent)
{
	if (s_instance_ != nullptr)
	{
		if (::IsWindow(s_instance_->GetSafeHwnd()))
		{
			s_instance_->SetForegroundWindow();
			return;
		}
		s_instance_ = nullptr;
	}
	auto* dlg = new DlgAcquisitionStub();
	if (dlg->Create(IDD_VIEWADCONTINUOUS, p_parent))
	{
		s_instance_ = dlg;
		dlg->ShowWindow(SW_SHOW);
	}
	else
	{
		delete dlg;
	}
}

BEGIN_MESSAGE_MAP(DlgAcquisitionStub, CDialogEx)
	ON_BN_CLICKED(IDC_ACQ_STUB_LAUNCH, &DlgAcquisitionStub::OnBnClickedLaunch)
	ON_BN_CLICKED(IDC_ACQ_STUB_IMPORT, &DlgAcquisitionStub::OnBnClickedImport)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL DlgAcquisitionStub::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CenterWindow();
	return TRUE;
}

void DlgAcquisitionStub::OnBnClickedLaunch()
{
	CString exe = AfxGetApp()->GetProfileString(_T("dataAcq"), _T("exePath"));
	if (exe.IsEmpty() || !CdbWaveDoc::file_exists(exe))
	{
		CFileDialog dlg(TRUE, _T("exe"), nullptr, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
			_T("Executables (*.exe)|*.exe|All Files (*.*)|*.*||"), this);
		dlg.m_ofn.lpstrTitle = _T("Select data acquisition program");
		if (dlg.DoModal() != IDOK)
			return;
		exe = dlg.GetPathName();
		AfxGetApp()->WriteProfileString(_T("dataAcq"), _T("exePath"), exe);
	}
	SHELLEXECUTEINFO sei{};
	sei.cbSize = sizeof(sei);
	sei.fMask = SEE_MASK_NOASYNC;
	sei.hwnd = GetSafeHwnd();
	sei.lpVerb = _T("open");
	sei.lpFile = exe;
	sei.nShow = SW_SHOWNORMAL;
	if (!ShellExecuteEx(&sei))
	{
		AfxMessageBox(_T("Unable to launch the selected acquisition program."), MB_ICONERROR);
	}
}

void DlgAcquisitionStub::OnBnClickedImport()
{
	CdbWaveDoc* doc = CdbWaveDoc::get_active_mdi_document();
	if (doc == nullptr)
	{
		AfxMessageBox(_T("Open a project before importing files."), MB_ICONINFORMATION);
		return;
	}
	DlgImportFiles dlg(this);
	dlg.DoModal();
}

void DlgAcquisitionStub::OnDestroy()
{
	CDialogEx::OnDestroy();
	s_instance_ = nullptr;
}
