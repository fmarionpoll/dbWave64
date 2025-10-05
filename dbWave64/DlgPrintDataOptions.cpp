
#include "StdAfx.h"
#include "resource.h"
#include "DlgPrintDataOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DlgPrintDataOptions::DlgPrintDataOptions(CWnd* p_parent /*=NULL*/)
	: CDialog(IDD, p_parent)
{
}

void DlgPrintDataOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CLIPRECT, m_b_clip_rect);
	DDX_Check(pDX, IDC_FRAMERECT, m_b_frame_rect);
	DDX_Check(pDX, IDC_TIMESCALEBAR, m_b_time_scale_bar);
	DDX_Check(pDX, IDC_VOLTAGESCALEBAR, m_b_voltage_scale_bar);
	DDX_Check(pDX, IDC_CHECK1, m_b_print_selection);
	DDX_Check(pDX, IDC_CHECK2, m_b_print_spk_bars);
}

BEGIN_MESSAGE_MAP(DlgPrintDataOptions, CDialog)

END_MESSAGE_MAP()


BOOL DlgPrintDataOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_b_frame_rect = p_print_parameters->b_frame_rect;
	m_b_clip_rect = p_print_parameters->b_clip_rect;
	m_b_time_scale_bar = p_print_parameters->b_timescale_bar;
	m_b_voltage_scale_bar = p_print_parameters->b_voltage_scale_bar;
	m_b_print_selection = p_print_parameters->b_print_selection;
	m_b_print_spk_bars = p_print_parameters->b_print_spk_bars;

	UpdateData(FALSE);
	return TRUE; // return TRUE  unless you set the focus to a control
}

void DlgPrintDataOptions::OnOK()
{
	UpdateData(TRUE);

	p_print_parameters->b_frame_rect = m_b_frame_rect;
	p_print_parameters->b_clip_rect = m_b_clip_rect;
	p_print_parameters->b_timescale_bar = m_b_time_scale_bar;
	p_print_parameters->b_voltage_scale_bar = m_b_voltage_scale_bar;
	p_print_parameters->b_print_selection = m_b_print_selection;
	p_print_parameters->b_print_spk_bars = m_b_print_spk_bars;

	CDialog::OnOK();
}
