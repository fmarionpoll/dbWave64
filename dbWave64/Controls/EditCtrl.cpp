// CEditCtrl.cpp : implementation file
//
// CEditCtrl is a control derived from the CEdit control. It accepts
// characters from the keyboard and store them as text within the window.
//
// CEditCtrl is used to subclass edit controls and check the incoming keys
// when the user press CR, the field is considered as validated and
// a message is sent to the parent to signal that.
//
// CEditCtrl traps up/down arrow keys and signals it to the parent
// so that the programmer can implement an automatic increment/decrement at the
// parent level
//
// lastly but not least, if the programmer wants that the edit control to be displayed
// with a short vertical scrollbar, the original edit control only needs to be declared
// (within the rc file or from within App studio's menu) as multiline and with a vertical
// scrollbar. CEditCtrl will trap scroll messages and forward them to the parent so
// that the value of the edit control can be changed. Vertical scroll messages are sent to
// the parent as VK_UP or VK_DOWN keys
//
// communication with parent window is done via messages posted and public variables.
// 	(::GetParent(m_hWnd), WM_COMMAND, ::GetDlgCtrlID(m_hWnd), MAKELONG(m_hWnd, EN_CHANGE));
// posted message is EN_CHANGE
// the state of the control is governed by 3 parameters.
// 	BOOL	m_bEntryDone;  TRUE only if CR, or up/down arrows = input is valid
//	WORD	m_nChar;		number of characters within windows text buffer
//
// the parent window should therefore trap EN_CHANGE messages from the sub-classed edit
// control and take action oly when m_bEntryDone is TRUE. It is to the responsibility
// of this parent to reset m_bEntryDone to FALSE when input is processed so that a new
// value can be validated.
//

#include "StdAfx.h"
#include "Editctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEditCtrl::CEditCtrl()
= default;

CEditCtrl::~CEditCtrl()
= default;

BEGIN_MESSAGE_MAP(CEditCtrl, CEdit)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


UINT CEditCtrl::OnGetDlgCode()
{
	return CEdit::OnGetDlgCode() | DLGC_WANTALLKEYS;
}

void CEditCtrl::OnKeyDown(const UINT n_char, const UINT n_rep_cnt, const UINT n_flags)
{
	// VK_SPACE (20), _PRIOR, _NEXT, _END, _HOME, _LEFT, _UP, _RIGHT, _DOWN, _SELECT(28)
	bool handled = false;
	if (n_char > VK_SPACE && n_char < VK_SELECT)
		handled = process_keys(n_char) != FALSE;
	if (!handled)
		CEdit::OnKeyDown(n_char, n_rep_cnt, n_flags);
}

void CEditCtrl::OnChar(const UINT n_char, const UINT n_rep_cnt, const UINT n_flags)
{
	if (!process_keys(n_char))
		CEdit::OnChar(n_char, n_rep_cnt, n_flags);
}

//--------------------------------------------------------------------------
// ProcessKeys
// send EN_CHANGE when control receives CR or arrow & page UP/DOWN
// TAB or lef/righ arrow de-select and select next control 
//--------------------------------------------------------------------------

BOOL CEditCtrl::process_keys(UINT n_char)
{
	switch (n_char)
	{
	case VK_TAB: 
		{
			const BOOL bShiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			const auto hParent = ::GetParent(m_hWnd);
			const auto hNext = ::GetNextDlgTabItem(hParent, m_hWnd, bShiftDown);
			if (hParent && hNext)
				::SendMessage(hParent, WM_NEXTDLGCTL, reinterpret_cast<WPARAM>(hNext), TRUE);
		}
		break;

	case VK_RETURN: 
	case VK_UP: 
	case VK_DOWN: 
	case VK_PRIOR:
	case VK_NEXT: 
		m_b_entry_done = TRUE;
		m_n_char = n_char;
		GetParent()->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), EN_CHANGE), reinterpret_cast<LPARAM>(m_hWnd));
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

void CEditCtrl::on_en_change(CWnd* parent_wnd, float& parameter, const float delta_up, const float delta_down)
{
	switch (m_n_char)
	{
	case VK_RETURN:
		//parent_wnd->UpdateData(TRUE);
		{
			CString cs;
			GetWindowText(cs);
			parameter = static_cast<float>(_ttof(cs));
		}
		break;
	case VK_UP:
	case VK_PRIOR:
		parameter += delta_up;
		break;
	case VK_DOWN:
	case VK_NEXT:
		parameter += delta_down;
		break;
	default:;
	}
	m_b_entry_done = FALSE;
	m_n_char = 0;
	SetSel(0, -1);
}

void CEditCtrl::on_en_change(CWnd* parent_wnd, int& parameter, const int delta_up, const int delta_down)
{
	switch (m_n_char)
	{
	case VK_RETURN:
		//parent_wnd->UpdateData(TRUE);
		{
			CString cs;
			GetWindowText(cs);
			parameter = static_cast<int>(_ttoi(cs));
		}
		break;
	case VK_UP:
	case VK_PRIOR:
		parameter += delta_up;
		break;
	case VK_DOWN:
	case VK_NEXT:
		parameter += delta_down;
		break;
	default:;
	}
	m_b_entry_done = FALSE;
	m_n_char = 0;
	SetSel(0, -1);
}

void CEditCtrl::on_en_change(CWnd* parent_wnd, UINT& parameter, const int delta_up, const int delta_down)
{
	switch (m_n_char)
	{
	case VK_RETURN:
		//parent_wnd->UpdateData(TRUE);
		{
			CString cs;
			GetWindowText(cs);
			LPTSTR endPtr = nullptr;
			unsigned long val = _tcstoul(cs, &endPtr, 10);
			parameter = static_cast<UINT>(val);
		}
		break;
	case VK_UP:
	case VK_PRIOR:
		parameter = static_cast<UINT>(static_cast<int>(parameter) + delta_up);
		break;
	case VK_DOWN:
	case VK_NEXT:
		parameter = static_cast<UINT>(static_cast<int>(parameter) + delta_down);
		break;
	default:;
	}
	m_b_entry_done = FALSE;
	m_n_char = 0;
	SetSel(0, -1);
}

void CEditCtrl::OnSetFocus(CWnd* p_old_wnd)
{
	CEdit::OnSetFocus(p_old_wnd);
	SetSel(0, -1);
}

void CEditCtrl::OnKillFocus(CWnd* p_new_wnd)
{
	if (!m_b_entry_done)
	{
		m_b_entry_done = TRUE;
		m_n_char = VK_RETURN;
		GetParent()->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), EN_CHANGE), reinterpret_cast<LPARAM>(m_hWnd));
	}
	CEdit::OnKillFocus(p_new_wnd);
}

void CEditCtrl::OnVScroll(const UINT n_sb_code, UINT n_pos, CScrollBar* p_scroll_bar)
{
	if (n_sb_code == SB_LINEDOWN)
		m_n_char = VK_DOWN;
	else if (n_sb_code == SB_LINEUP)
		m_n_char = VK_UP;
	else {
		CEdit::OnVScroll(n_sb_code, n_pos, p_scroll_bar);
		return;
	}
	m_b_entry_done = TRUE;
	GetParent()->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), EN_CHANGE), reinterpret_cast<LPARAM>(m_hWnd));
}
