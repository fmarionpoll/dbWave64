// GridCellCheck.cpp : implementation file
//
// MFC Grid Control - Main grid cell class
//
// Provides the implementation for a combobox cell type of the
// grid control.
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
//
// Parts of the code contained in this file are based on the original
// CInPlaceList from http://www.codeguru.com/listview
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name and all copyright
// notices remains intact.
//
// An email letting me know how you are using it would be nice as well.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with GridCtrl v2.22+
//
// History:
// 23 Jul 2001 - Complete rewrite
// 13 Mar 2004 - GetCellExtent and GetCheckPlacement fixed by Yogurt
//             - Read-only now honoured - Yogurt
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCell.h"
#include "GridCtrl.h"

#include "GridCellCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(GridCellCheck, GridCell)

GridCellCheck::GridCellCheck() : GridCell()
{
	m_bChecked = FALSE;
	//m_Rect.IsRectNull();
}

CSize GridCellCheck::GetCellExtent(CDC* p_dc)
{
	// Using SM_CXHSCROLL as a guide to the size of the checkbox
	const int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2 * GetMargin();
	CSize cellSize = GridCell::GetCellExtent(p_dc);
	cellSize.cx += nWidth;
	cellSize.cy = max(cellSize.cy, nWidth);
	return cellSize;
}

// i/o:  i=dims of cell rect; o=dims of text rect
BOOL GridCellCheck::GetTextRect(LPRECT pRect)
{
	const BOOL bResult = GridCell::GetTextRect(pRect);
	if (bResult)
	{
		const int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2 * GetMargin();
		pRect->left += nWidth;
		if (pRect->left > pRect->right)
			pRect->left = pRect->right;
	}
	return bResult;
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL GridCellCheck::Draw(CDC* p_dc, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd /*=TRUE*/)
{
	const BOOL bResult = GridCell::Draw(p_dc, nRow, nCol, rect, bEraseBkgnd);

#ifndef _WIN32_WCE
	// Store the cell's dimensions for later
	m_Rect = rect;

	const CRect CheckRect = GetCheckPlacement();
	rect.left = CheckRect.right;

	// enough room to draw?
	// if (CheckRect.GetRectWidth() < rect.GetRectWidth() && CheckRect.GetRectHeight() < rect.GetRectHeight()) {
	// Do the draw
	p_dc->DrawFrameControl(GetCheckPlacement(), DFC_BUTTON,
	                       (m_bChecked) ? DFCS_BUTTONCHECK | DFCS_CHECKED : DFCS_BUTTONCHECK);
	// }
#endif
	return bResult;
}

void GridCellCheck::OnClick(CPoint PointCellRelative)
{
	// PointCellRelative is relative to the topleft of the cell. Convert to client coords
	PointCellRelative += m_Rect.TopLeft();

	// Bail if cell is read-only
	GridCellID cell = GetGrid()->GetCellFromPt(PointCellRelative);
	if (!GetGrid()->IsCellEditable(cell))
		return;

	// GetCheckPlacement returns the checkbox dimensions in client coords. Only check/
	// uncheck if the user clicked in the box
	if (GetCheckPlacement().PtInRect(PointCellRelative))
	{
		m_bChecked = !m_bChecked;
		GetGrid()->InvalidateRect(m_Rect);
	}
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL GridCellCheck::SetCheck(BOOL bChecked /*=TRUE*/)
{
	const BOOL bTemp = m_bChecked;
	m_bChecked = bChecked;
	if (!m_Rect.IsRectEmpty())
		GetGrid()->InvalidateRect(m_Rect);

	return bTemp;
}

BOOL GridCellCheck::GetCheck()
{
	return m_bChecked;
}

//////////////////////////////////////////////////////////////////////
// Protected implementation
//////////////////////////////////////////////////////////////////////

// Returns the dimensions and placement of the checkbox in client coords.
CRect GridCellCheck::GetCheckPlacement()
{
	const int nWidth = GetSystemMetrics(SM_CXHSCROLL);
	CRect place = m_Rect + CSize(GetMargin(), GetMargin());
	place.right = place.left + nWidth;
	place.bottom = place.top + nWidth;

	/* for centering
	int nDiff = (place.GetRectWidth() - nWidth)/2;
	if (nDiff > 0)
	{
		place.left += nDiff;
		place.right = place.left + nWidth;
	}
	nDiff = (place.GetRectHeight() - nWidth)/2;
	if (nDiff > 0)
	{
		place.top += nDiff;
		place.bottom = place.top + nWidth;
	}
	*/

	if (m_Rect.Height() < nWidth + 2 * static_cast<int>(GetMargin()))
	{
		place.top = m_Rect.top + (m_Rect.Height() - nWidth) / 2;
		place.bottom = place.top + nWidth;
	}

	return place;
}
