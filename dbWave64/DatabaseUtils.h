#pragma once

#include <afxdao.h>
#include <afxole.h>

// Utility class for safe database operations
class CDatabaseUtils
{
public:
    // Safely extract string from COleVariant
    static CString safe_get_string_from_variant(const COleVariant& var);
    
    // Check if variant contains valid string data
    static BOOL is_valid_string_variant(const COleVariant& var);
    
    // Safely extract integer from COleVariant
    static __int64 safe_get_int64_from_variant(const COleVariant& var);
    
    // Safely extract double from COleVariant
    static double safe_get_double_from_variant(const COleVariant& var);
    
    // Get variant type as string for debugging
    static CString get_variant_type_string(const COleVariant& var);
    
    // Analyze BSTR content for debugging corruption issues
    static CString analyze_bstr_content(const COleVariant& var);
    
    // String validation for writing to database
    static CString validate_string_for_writing(const CString& input);
};

// Enhanced database access wrapper
class CDatabaseAccessWrapper
{
private:
    CDaoRecordset* m_pRecordset;
    
public:
    CDatabaseAccessWrapper(CDaoRecordset* pRecordset);
    ~CDatabaseAccessWrapper();
    
    // Get field value as string by index
    CString get_field_value_as_string(int fieldIndex);
    
    // Get field value as string by name
    CString get_field_value_as_string(const CString& fieldName);
    
    // Get field value as 64-bit integer by index
    __int64 get_field_value_as_int64(int fieldIndex);
    
    // Get field value as 64-bit integer by name
    __int64 get_field_value_as_int64(const CString& fieldName);
    
    // Get field value as double by index
    double get_field_value_as_double(int fieldIndex);
    
    // Get field value as double by name
    double get_field_value_as_double(const CString& fieldName);
    
    // Check if recordset is valid and open
    BOOL is_valid() const;
    
    // Get error information
    CString get_last_error() const;
    
private:
    CString m_lastError;
};
