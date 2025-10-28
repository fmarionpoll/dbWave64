#pragma once

#include <afxext.h>
#include "ViewDbTable.h"
#include "dbWave.h"

class ViewADcontinuous : public ViewDbTable
{
protected:
    ViewADcontinuous();
    ~ViewADcontinuous() override;
    DECLARE_DYNCREATE(ViewADcontinuous)

    enum { IDD = IDD_VIEWADCONTINUOUS };

public:
    void DoDataExchange(CDataExchange* dx) override;
    void OnInitialUpdate() override;
    afx_msg void OnPaint();

    DECLARE_MESSAGE_MAP()
};
