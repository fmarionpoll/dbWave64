#pragma once

#include <afxwin.h>
#include "ViewDB/options_print.h"

// Simple, programmatic export options dialog window bound to options_print
class ExportOptionsWindow : public CWnd
{
public:
	static BOOL Show(CWnd* p_parent, options_print* p_opts);

protected:
	ExportOptionsWindow();
	~ExportOptionsWindow() override;

	BOOL Create(CWnd* p_parent, options_print* p_opts);

	options_print* p_opts_ { nullptr };

	// Controls
	CEdit edit_w_ {};
	CEdit edit_h_ {};
	CButton chk_frame_ {};
	CButton chk_clip_ {};
	CButton chk_timebar_ {};
	CButton chk_voltbar_ {};
	CButton chk_preview_ {};
	CButton btn_ok_ {};
	CButton btn_cancel_ {};

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnOk();
	afx_msg void OnCancelClicked();

	DECLARE_MESSAGE_MAP()
};



