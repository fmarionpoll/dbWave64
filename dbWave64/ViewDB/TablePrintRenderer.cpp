#include "StdAfx.h"
#include "TablePrintRenderer.h"
#include "ViewDbTable.h"
#include "dbWaveDoc.h"

BOOL DefaultTablePrintRenderer::prepare_printing(ViewDbTable* view, CPrintInfo* p_info)
{
	if (!view->DoPreparePrinting(p_info))
		return FALSE;
	if (!COleDocObjectItem::OnPreparePrinting(view, p_info))
		return FALSE;
	return TRUE;
}

void DefaultTablePrintRenderer::print_page(ViewDbTable* view, CDC* p_dc, CPrintInfo* p_info)
{
	// Call through the view to respect access protection
	view->framework_default_print(p_dc, p_info);
}


