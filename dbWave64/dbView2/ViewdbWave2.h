#pragma once

#include "ViewDbTable.h"
#include "DataListCtrl2.h"

class ViewdbWave2 : public ViewDbTable
{
protected:
	DECLARE_DYNCREATE(ViewdbWave2)
	ViewdbWave2();
	~ViewdbWave2() override;

public:
	enum { IDD = IDD_VIEWDBWAVE };

protected:
	void DoDataExchange(CDataExchange* p_dx) override;
	void OnInitialUpdate() override;
	void OnSize(UINT n_type, int cx, int cy);

protected:
	DECLARE_MESSAGE_MAP()

private:
	DataListCtrl2 m_data_list_ctrl_;
};


