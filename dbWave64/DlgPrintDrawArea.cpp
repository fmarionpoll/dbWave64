// printdra.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "./DlgPrintDrawArea.h"
#include "ViewDB/options_print.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DlgPrintDrawArea::DlgPrintDrawArea(CWnd* p_parent /*=NULL*/)
	: CDialog(IDD, p_parent)
{

}

void DlgPrintDrawArea::DoDataExchange(CDataExchange* p_dx)
{
	CDialog::DoDataExchange(p_dx);
	DDX_Text(p_dx, IDC_HEIGHTDOC, m_height_doc);
	DDX_Text(p_dx, IDC_SEPARATORHEIGHT, m_height_separator);
	DDX_Text(p_dx, IDC_WIDTHDOC, m_width_doc);
	DDX_Text(p_dx, IDC_EDIT1, m_spk_height);
	DDX_Text(p_dx, IDC_EDIT2, m_spk_width);
	DDX_Check(p_dx, IDC_CHECKFILTERDATA, m_b_filter_dat);
}

BEGIN_MESSAGE_MAP(DlgPrintDrawArea, CDialog)

END_MESSAGE_MAP()

BOOL DlgPrintDrawArea::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_height_doc = p_print_parameters->height_doc;
	m_width_doc = p_print_parameters->width_doc;
	m_spk_height = p_print_parameters->spike_height;
	m_spk_width = p_print_parameters->spike_width;
	m_height_separator = p_print_parameters->height_separator;
	m_b_filter_dat = p_print_parameters->b_filter_data_source;
	UpdateData(FALSE);

	return TRUE; // return TRUE  unless you set the focus to a control
}

void DlgPrintDrawArea::OnOK()
{
	UpdateData(TRUE);

	p_print_parameters->height_doc = m_height_doc;
	p_print_parameters->width_doc = m_width_doc;
	p_print_parameters->height_separator = m_height_separator;
	p_print_parameters->spike_height = m_spk_height;
	p_print_parameters->spike_width = m_spk_width;
	p_print_parameters->b_filter_data_source = m_b_filter_dat;
	CDialog::OnOK();
}
