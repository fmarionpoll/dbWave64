

#pragma once
#include "StdAfx.h"
#include "ViewDbTable.h"


class CViewAcqDat : public ViewDbTable
{
protected: // create from serialization only
	CViewAcqDat() noexcept;
	DECLARE_DYNCREATE(CViewAcqDat)
	// Form Data
	enum { IDD = IDD_VIEWADCONTINUOUS};


// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnTimer(UINT_PTR nIDEvent);

// Implementation
public:
	virtual ~CViewAcqDat();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnBrowseMailboxDirectory();
	afx_msg void OnStartMailboxPolling();
	afx_msg void OnStopMailboxPolling();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:
	void load_settings();
	void save_settings() const;
	void update_controls();
	void update_status(const CString& message);
	bool poll_mailbox_once();
	CString build_mailbox_file_path() const;
	void stop_timer();
	bool start_timer();

private:
	CString mailbox_directory_;
	CString mailbox_filename_;
	int poll_interval_secs_ {1};
	bool is_polling_ {false};
	UINT_PTR timer_id_ {0};
	CString last_status_message_;
	static constexpr UINT_PTR k_timer_id_ = 1;
	static constexpr int k_min_poll_interval_ = 1;
	static constexpr int k_max_poll_interval_ = 3600;
};

#ifndef _DEBUG  // debug version in ViewAcqDatcpp
inline CMFCApplication1Doc* CViewAcqDat::GetDocument() const
   { return reinterpret_cast<CMFCApplication1Doc*>(m_pDocument); }
#endif
