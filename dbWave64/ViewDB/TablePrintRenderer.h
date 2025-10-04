#pragma once

#include <afx.h>

class ViewDbTable;
class CDC;
struct CPrintInfo; // MFC declares CPrintInfo as struct in afxext.h
class CRect;

// Abstraction for printing and export rendering for table-based views
class ITablePrintRenderer
{
public:
	virtual ~ITablePrintRenderer() = default;
	virtual BOOL prepare_printing(ViewDbTable* view, CPrintInfo* p_info) = 0;
	virtual void begin_printing(ViewDbTable* view, CDC* p_dc, CPrintInfo* p_info) = 0;
	virtual void print_page(ViewDbTable* view, CDC* p_dc, CPrintInfo* p_info) = 0;
	virtual void end_printing(ViewDbTable* view, CDC* p_dc, CPrintInfo* p_info) = 0;
	virtual void render_for_export(ViewDbTable* view, CDC* p_dc, const CSize& pixel_rect) = 0;
};

// Default renderer preserves existing behavior (delegates to framework defaults)
class DefaultTablePrintRenderer : public ITablePrintRenderer
{
public:
	BOOL prepare_printing(ViewDbTable* view, CPrintInfo* p_info) override;
	void begin_printing(ViewDbTable* /*view*/, CDC* /*p_dc*/, CPrintInfo* /*p_info*/) override {}
	void print_page(ViewDbTable* view, CDC* p_dc, CPrintInfo* p_info) override;
	void end_printing(ViewDbTable* /*view*/, CDC* /*p_dc*/, CPrintInfo* /*p_info*/) override {}
	void render_for_export(ViewDbTable* /*view*/, CDC* /*p_dc*/, const CSize& /*resolution*/) override {}
};


