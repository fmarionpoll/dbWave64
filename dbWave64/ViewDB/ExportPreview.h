#pragma once

#include <afxwin.h>

// Lightweight EMF preview window with Copy and Save buttons
class ExportPreviewWindow : public CWnd
{
public:
	ExportPreviewWindow();
	~ExportPreviewWindow() override;

	// Create and show the preview window. Takes ownership of h_emf for the lifetime of the window.
	BOOL Create(CWnd* p_parent, HENHMETAFILE h_emf, const CRect& pixel_bounds, const CString& title);

	static BOOL Show(CWnd* p_parent, HENHMETAFILE h_emf, const CRect& pixel_bounds, const CString& title);

protected:
	HENHMETAFILE h_emf_ { nullptr };
	CRect pixel_bounds_ {};
	CButton btn_copy_ {};
	CButton btn_save_ {};
	CButton btn_close_ {};
	CButton btn_print_ {};
	CButton btn_png_ {};
	CButton btn_settings_ {};

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnCopy();
	afx_msg void OnSave();
	afx_msg void OnPrint();
	afx_msg void OnSavePng();
	afx_msg void OnSettings();
	afx_msg void OnCloseClicked();

	DECLARE_MESSAGE_MAP()
};
