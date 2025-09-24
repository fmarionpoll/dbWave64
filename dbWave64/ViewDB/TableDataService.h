#pragma once

#include <afxdao.h>

class CdbWaveDoc;

// Thin data-access abstraction for views. Default implementation delegates to CdbWaveDoc (DAO).
class ITableDataService
{
public:
	virtual ~ITableDataService() = default;
	virtual CDaoRecordset* get_recordset() = 0;
};

class DaoTableDataService : public ITableDataService
{
public:
	explicit DaoTableDataService(CdbWaveDoc* document) : document_(document) {}
	CDaoRecordset* get_recordset() override;

private:
	CdbWaveDoc* document_ { nullptr };
};


