#pragma once

#include <afxwin.h>

class DlgAcquisitionStub : public CDialogEx
{
	DECLARE_DYNAMIC(DlgAcquisitionStub)
public:
	DlgAcquisitionStub();
	static void Show(CWnd* p_parent);

protected:
	BOOL OnInitDialog() override;
	afx_msg void OnBnClickedLaunch();
	afx_msg void OnBnClickedImport();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:
	static DlgAcquisitionStub* s_instance_;
};
