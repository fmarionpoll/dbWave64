

#include "stdafx.h"
#include "ViewAcqDat.h"

#include "dbWave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




IMPLEMENT_DYNCREATE(CViewAcqDat, ViewDbTable)

BEGIN_MESSAGE_MAP(CViewAcqDat, ViewDbTable)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &ViewDbTable::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &ViewDbTable::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CViewAcqDat::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CViewAcqDatView construction/destruction

CViewAcqDat::CViewAcqDat() noexcept
	: ViewDbTable(IDD)
{
}

CViewAcqDat::~CViewAcqDat()
{
}

void CViewAcqDat::DoDataExchange(CDataExchange* pDX)
{
	ViewDbTable::DoDataExchange(pDX);
}

BOOL CViewAcqDat::PreCreateWindow(CREATESTRUCT& cs)
{
	return ViewDbTable::PreCreateWindow(cs);
}

void CViewAcqDat::OnInitialUpdate()
{
	ViewDbTable::OnInitialUpdate();
	ResizeParentToFit();

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
