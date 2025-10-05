#pragma once


class options_print;

class DlgPrintDrawArea : public CDialog
{
	// Construction
public:
	DlgPrintDrawArea(CWnd* p_parent = nullptr); // standard constructor

	// Dialog Data
	enum { IDD = IDD_PRINTDRAWAREA };

	int m_height_doc{ 0 };
	int m_height_separator{ 0 };
	int m_width_doc{ 0 };
	int m_spk_height{ 0 };
	int m_spk_width{ 0 };
	options_print* p_print_parameters{ nullptr };
	BOOL m_b_filter_dat{ false };

	// Implementation
protected:
	void DoDataExchange(CDataExchange* p_dx) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	void OnOK() override;

	DECLARE_MESSAGE_MAP()
};
