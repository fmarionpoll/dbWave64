#include "StdAfx.h"
#include "TableDataService.h"
#include "dbWaveDoc.h"

CDaoRecordset* DaoTableDataService::get_recordset()
{
	return document_ ? document_->db_get_recordset() : nullptr;
}


