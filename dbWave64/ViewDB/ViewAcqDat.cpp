#include "stdafx.h"
#include "ViewAcqDat.h"

#include "dbWave.h"
#include "dbWaveDoc.h"
#include "RegistryManager.h"

#include <algorithm>
#include <Shlwapi.h>
using namespace std;

#pragma comment(lib, "Shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


template<typename T>
static constexpr T clamp_ct(const T& v, const T& lo, const T& hi) noexcept
{
	return (v < lo) ? lo : ((v > hi) ? hi : v);
}

IMPLEMENT_DYNCREATE(CViewAcqDat, ViewDbTable)

BEGIN_MESSAGE_MAP(CViewAcqDat, ViewDbTable)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &ViewDbTable::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &ViewDbTable::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CViewAcqDat::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_MAILBOX_BROWSE_BUTTON, &CViewAcqDat::OnBrowseMailboxDirectory)
	ON_BN_CLICKED(IDC_MAILBOX_START_BUTTON, &CViewAcqDat::OnStartMailboxPolling)
	ON_BN_CLICKED(IDC_MAILBOX_STOP_BUTTON, &CViewAcqDat::OnStopMailboxPolling)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CViewAcqDatView construction/destruction

CViewAcqDat::CViewAcqDat() noexcept
	: ViewDbTable(IDD)
{
	mailbox_filename_ = _T("mailbox.txt");
	poll_interval_secs_ = k_min_poll_interval_;
	last_status_message_ = _T("Idle");
}

CViewAcqDat::~CViewAcqDat()
{
	stop_timer();
}

void CViewAcqDat::DoDataExchange(CDataExchange* pDX)
{
	ViewDbTable::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MAILBOX_DIR_EDIT, mailbox_directory_);
	DDX_Text(pDX, IDC_MAILBOX_FILE_EDIT, mailbox_filename_);
	DDX_Text(pDX, IDC_MAILBOX_INTERVAL_EDIT, poll_interval_secs_);
	DDV_MinMaxInt(pDX, poll_interval_secs_, k_min_poll_interval_, k_max_poll_interval_);
}

BOOL CViewAcqDat::PreCreateWindow(CREATESTRUCT& cs)
{
	return ViewDbTable::PreCreateWindow(cs);
}

void CViewAcqDat::OnInitialUpdate()
{
	ViewDbTable::OnInitialUpdate();
	ResizeParentToFit();

	load_settings();
	UpdateData(FALSE);

	if (auto spin = reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_MAILBOX_INTERVAL_SPIN)))
	{
		spin->SetRange32(k_min_poll_interval_, k_max_poll_interval_);
		spin->SetPos(poll_interval_secs_);
	}

	update_status(last_status_message_);
	update_controls();
}


// CViewAcqDatView printing


void CViewAcqDat::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CViewAcqDat::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CViewAcqDat::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CViewAcqDat::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CViewAcqDat::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

void CViewAcqDat::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CViewAcqDat::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	the_app.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

void CViewAcqDat::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == k_timer_id_)
	{
		if (!is_polling_)
		{
			stop_timer();
			update_controls();
		}
		else
		{
			poll_mailbox_once();
		}
		return;
	}

	ViewDbTable::OnTimer(nIDEvent);
}


void CViewAcqDat::OnDestroy()
{
	stop_timer();
	save_settings();
	ViewDbTable::OnDestroy();
}

void CViewAcqDat::OnBrowseMailboxDirectory()
{
	CFolderPickerDialog dlg(mailbox_directory_, OFN_FILEMUSTEXIST, this);
	if (dlg.DoModal() == IDOK)
	{
		mailbox_directory_ = dlg.GetPathName();
		mailbox_directory_.TrimLeft();
		mailbox_directory_.TrimRight(_T("\\/"));
		UpdateData(FALSE);
		update_controls();
		save_settings();
	}
}

void CViewAcqDat::OnStartMailboxPolling()
{
	if (is_polling_)
		return;

	if (!UpdateData(TRUE))
		return;

	mailbox_directory_.Trim();
	mailbox_directory_.TrimRight(_T("\\/"));
	mailbox_filename_.Trim();
	if (mailbox_filename_.IsEmpty())
		mailbox_filename_ = _T("mailbox.txt");

	poll_interval_secs_ = clamp_ct(poll_interval_secs_, k_min_poll_interval_, k_max_poll_interval_);
	UpdateData(FALSE);

	if (mailbox_directory_.IsEmpty())
	{
		AfxMessageBox(_T("Please select a mailbox directory."));
		return;
	}

	if (!PathFileExists(mailbox_directory_))
	{
		AfxMessageBox(_T("The selected mailbox directory does not exist."));
		return;
	}

	if (mailbox_filename_.IsEmpty())
	{
		AfxMessageBox(_T("Please specify the mailbox file name."));
		return;
	}

	save_settings();
	is_polling_ = true;
	if (!start_timer())
	{
		is_polling_ = false;
		AfxMessageBox(_T("Unable to start mailbox polling timer."));
		update_controls();
		return;
	}

	update_controls();
	CString message;
	message.Format(_T("Polling every %d second(s)."), poll_interval_secs_);
	update_status(message);
	poll_mailbox_once();
}

void CViewAcqDat::OnStopMailboxPolling()
{
	if (!is_polling_)
		return;

	is_polling_ = false;
	stop_timer();
	update_controls();
	update_status(_T("Polling stopped."));
	save_settings();
}

void CViewAcqDat::load_settings()
{
	auto& registry = RegistryManager::GetInstance();
	mailbox_directory_ = registry.GetProfileString(_T("Mailbox"), _T("Directory"));
	mailbox_directory_.Trim();
	mailbox_directory_.TrimRight(_T("\\/"));
	mailbox_filename_ = registry.GetProfileString(_T("Mailbox"), _T("File"), _T("mailbox.txt"));
	mailbox_filename_.Trim();
	poll_interval_secs_ = registry.GetProfileInt(_T("Mailbox"), _T("PollIntervalSeconds"), k_min_poll_interval_);
	poll_interval_secs_ = clamp_ct(poll_interval_secs_, k_min_poll_interval_, k_max_poll_interval_);
}

void CViewAcqDat::save_settings() const
{
	auto& registry = RegistryManager::GetInstance();
	registry.WriteProfileString(_T("Mailbox"), _T("Directory"), mailbox_directory_);
	registry.WriteProfileString(_T("Mailbox"), _T("File"), mailbox_filename_);
	registry.WriteProfileInt(_T("Mailbox"), _T("PollIntervalSeconds"), poll_interval_secs_);
}

void CViewAcqDat::update_controls()
{
	const BOOL inputs_enabled = !is_polling_;
	if (auto start_button = GetDlgItem(IDC_MAILBOX_START_BUTTON))
	{
		CString dir;
		GetDlgItemText(IDC_MAILBOX_DIR_EDIT, dir);
		dir.Trim();
		CString file;
		GetDlgItemText(IDC_MAILBOX_FILE_EDIT, file);
		file.Trim();
		const BOOL can_start = inputs_enabled && !dir.IsEmpty() && !file.IsEmpty();
		start_button->EnableWindow(can_start);
	}

	if (auto stop_button = GetDlgItem(IDC_MAILBOX_STOP_BUTTON))
		stop_button->EnableWindow(is_polling_);

	const UINT control_ids[] = {
		IDC_MAILBOX_DIR_EDIT,
		IDC_MAILBOX_BROWSE_BUTTON,
		IDC_MAILBOX_FILE_EDIT,
		IDC_MAILBOX_INTERVAL_EDIT,
		IDC_MAILBOX_INTERVAL_SPIN
	};

	for (auto id : control_ids)
	{
		if (auto ctrl = GetDlgItem(id))
			ctrl->EnableWindow(inputs_enabled);
	}
}

void CViewAcqDat::update_status(const CString& message)
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	CString display;
	display.Format(_T("%s - %s"), now.Format(_T("%H:%M:%S")), message);
	last_status_message_ = display;
	if (GetSafeHwnd() != nullptr)
		SetDlgItemText(IDC_MAILBOX_STATUS_STATIC, display);
}

bool CViewAcqDat::poll_mailbox_once()
{
	const CString mailbox_path = build_mailbox_file_path();
	if (mailbox_path.IsEmpty())
	{
		update_status(_T("Mailbox path is not configured."));
		return false;
	}

	if (!PathFileExists(mailbox_directory_))
	{
		update_status(_T("Mailbox directory not found."));
		return false;
	}

	if (!PathFileExists(mailbox_path))
	{
		update_status(_T("No new files detected."));
		return false;
	}

	CString processing_path = mailbox_path + _T(".processing");
	DeleteFile(processing_path);
	if (!MoveFile(mailbox_path, processing_path))
	{
		const DWORD error_code = GetLastError();
		if (error_code == ERROR_SHARING_VIOLATION || error_code == ERROR_ACCESS_DENIED)
		{
			update_status(_T("Mailbox file is busy; will retry."));
		}
		else
		{
			CString msg;
			msg.Format(_T("Unable to lock mailbox (error %lu)."), error_code);
			update_status(msg);
		}
		return false;
	}

	CStringArray files_to_import;
	bool restore_mailbox = false;
	try
	{
		CStdioFile file(processing_path, CFile::modeRead | CFile::typeText | CFile::shareDenyNone);
		CString line;
		while (file.ReadString(line))
		{
			line.Trim();
			if (line.IsEmpty())
				continue;

			CString normalized = line;
			if (PathIsRelative(normalized))
			{
				normalized = mailbox_directory_;
				if (!normalized.IsEmpty() && normalized.Right(1) != _T("\\") && normalized.Right(1) != _T("/"))
					normalized += _T("\\");
				normalized += line;
			}

			TCHAR canonical_buffer[MAX_PATH] = { 0 };
			if (PathCanonicalize(canonical_buffer, normalized))
				normalized = canonical_buffer;

			files_to_import.Add(normalized);
		}
		file.Close();
	}
	catch (CFileException* e)
	{
		restore_mailbox = true;
		e->Delete();
	}

	if (restore_mailbox)
	{
		MoveFileEx(processing_path, mailbox_path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
		update_status(_T("Failed to read mailbox; will retry."));
		return false;
	}

	if (files_to_import.IsEmpty())
	{
		DeleteFile(processing_path);
		update_status(_T("Mailbox file was empty."));
		return false;
	}

	CdbWaveDoc* doc = CdbWaveDoc::get_active_mdi_document();
	if (doc == nullptr)
	{
		MoveFileEx(processing_path, mailbox_path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
		CString msg;
		msg.Format(_T("%d file(s) pending but no active database."), files_to_import.GetSize());
		update_status(msg);
		return false;
	}

	doc->import_file_list(files_to_import);
	doc->UpdateAllViews(nullptr, HINT_REQUERY, nullptr);

	DeleteFile(processing_path);

	CString msg;
	msg.Format(_T("Imported %d file(s)."), files_to_import.GetSize());
	update_status(msg);
	return true;
}

CString CViewAcqDat::build_mailbox_file_path() const
{
	CString directory = mailbox_directory_;
	directory.Trim();
	directory.TrimRight(_T("\\/"));
	CString file = mailbox_filename_;
	file.Trim();
	if (directory.IsEmpty() || file.IsEmpty())
		return CString();
	if (directory.Right(1) != _T("\\"))
		directory += _T("\\");
	return directory + file;
}

void CViewAcqDat::stop_timer()
{
	if (timer_id_ != 0)
	{
		KillTimer(timer_id_);
		timer_id_ = 0;
	}
}

bool CViewAcqDat::start_timer()
{
	stop_timer();
	const UINT interval_ms = static_cast<UINT>(std::max(k_min_poll_interval_, poll_interval_secs_)) * 1000U;
	timer_id_ = SetTimer(k_timer_id_, interval_ms, nullptr);
	return timer_id_ != 0;
}


// CViewAcqDatView diagnostics

#ifdef _DEBUG
void CViewAcqDat::AssertValid() const
{
	ViewDbTable::AssertValid();
}

void CViewAcqDat::Dump(CDumpContext& dc) const
{
	ViewDbTable::Dump(dc);
}

#endif //_DEBUG


// CViewAcqDatView message handlers
