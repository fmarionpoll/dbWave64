#include "StdAfx.h"
#include "resource.h"
#include "dbTableMain.h"
#include "dbWave.h"
#include "MainFrm.h"
#include "StretchControls.h"
#include "ViewADcontinuous.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(ViewADcontinuous, ViewDbTable)

BEGIN_MESSAGE_MAP(ViewADcontinuous, CFormView)
END_MESSAGE_MAP()

ViewADcontinuous::ViewADcontinuous()
	: ViewDbTable(IDD)
{
	m_bEnableActiveAccessibility = FALSE;
}

ViewADcontinuous::~ViewADcontinuous()
= default;

void ViewADcontinuous::DoDataExchange(CDataExchange * p_dx)
{
	CFormView::DoDataExchange(p_dx);

}

void ViewADcontinuous::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

}




