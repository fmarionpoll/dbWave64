#include "stdafx.h"
#include "DatabaseUtils.h"
#include <afxdao.h>

// CDatabaseUtils implementation
CString CDatabaseUtils::safe_get_string_from_variant(const COleVariant& var)
{
    CString result;
    
    try
    {
        // Check if the variant contains a string
        if (var.vt == VT_BSTR && var.bstrVal != nullptr)
        {
            // Get the BSTR length and content
            UINT bstrLen = SysStringLen(var.bstrVal);
            
            // Check if the BSTR contains valid data
            if (bstrLen > 0)
            {
                // Method 1: Try direct BSTR to CString conversion
                result = CString(var.bstrVal);
                
                // Method 2: If the result looks corrupted (contains non-ASCII chars), try alternative approaches
                BOOL looksCorrupted = FALSE;
                for (int i = 0; i < result.GetLength(); i++)
                {
                    TCHAR ch = result[i];
                    if (ch > 127) // Non-ASCII characters
                    {
                        looksCorrupted = TRUE;
                        break;
                    }
                }
                
                if (looksCorrupted)
                {
                    // Method 3: Try using COleVariant's built-in string conversion
                    try
                    {
                        COleVariant varCopy = var;
                        varCopy.ChangeType(VT_BSTR);
                        if (varCopy.vt == VT_BSTR && varCopy.bstrVal != nullptr)
                        {
                            CString convertedResult = CString(varCopy.bstrVal);
                            // Check if this conversion worked better
                            BOOL convertedLooksBetter = TRUE;
                            for (int i = 0; i < convertedResult.GetLength(); i++)
                            {
                                TCHAR ch = convertedResult[i];
                                if (ch > 127) // Still has non-ASCII
                                {
                                    convertedLooksBetter = FALSE;
                                    break;
                                }
                            }
                            if (convertedLooksBetter)
                            {
                                result = convertedResult;
                            }
                        }
                    }
                    catch (...)
                    {
                        // Conversion failed, keep original result
                    }
                    
                    // Method 4: If still corrupted, try to extract only ASCII characters
                    if (looksCorrupted)
                    {
                        CString asciiOnly;
                        for (int i = 0; i < result.GetLength(); i++)
                        {
                            TCHAR ch = result[i];
                            if (ch >= 32 && ch <= 127) // Printable ASCII
                            {
                                asciiOnly += ch;
                            }
                        }
                        
                        if (!asciiOnly.IsEmpty())
                        {
                            result = asciiOnly;
                        }
                        else
                        {
                            // Method 5: Last resort - try to interpret as raw bytes
                            // This is a fallback for cases where the BSTR contains valid data
                            // but is being misinterpreted due to 32/64-bit differences
                            result = _T("[BSTR Interpretation Error]");
                        }
                    }
                }
            }
            else
            {
                result = _T("[Empty BSTR]");
            }
        }
        else if (var.vt == VT_I4 || var.vt == VT_I2)
        {
            // Handle numeric types that might contain string data
            result.Format(_T("%ld"), var.lVal);
        }
        else if (var.vt == VT_R8 || var.vt == VT_R4)
        {
            // Handle floating point types
            result.Format(_T("%f"), var.dblVal);
        }
        else if (var.vt == VT_DATE)
        {
            // Handle date types
            COleDateTime date(var.date);
            result = date.Format(_T("%Y-%m-%d %H:%M:%S"));
        }
        else if (var.vt == VT_BOOL)
        {
            // Handle boolean types
            result = var.boolVal ? _T("True") : _T("False");
        }
        else if (var.vt == VT_NULL)
        {
            // Handle null values
            result = _T("[NULL]");
        }
        else
        {
            // Try to convert using COleVariant's built-in conversion
            try
            {
                COleVariant varCopy = var;
                varCopy.ChangeType(VT_BSTR);
                if (varCopy.vt == VT_BSTR && varCopy.bstrVal != nullptr)
                {
                    result = CString(varCopy.bstrVal);
                }
                else
                {
                    result = _T("[Invalid String Data]");
                }
            }
            catch (...)
            {
                result = _T("[Conversion Failed]");
            }
        }
    }
    catch (...)
    {
        result = _T("[String Read Error]");
    }
    
    return result;
}

// Enhanced debugging function to analyze BSTR content
CString CDatabaseUtils::analyze_bstr_content(const COleVariant& var)
{
    CString analysis;
    
    if (var.vt == VT_BSTR && var.bstrVal != nullptr)
    {
        UINT bstrLen = SysStringLen(var.bstrVal);
        analysis.Format(_T("BSTR Length: %u, Pointer: 0x%p\n"), bstrLen, var.bstrVal);
        
        if (bstrLen > 0)
        {
            analysis += _T("Content: ");
            for (UINT i = 0; i < bstrLen && i < 50; i++) // Limit to first 50 chars
            {
                TCHAR ch = var.bstrVal[i];
                if (ch >= 32 && ch <= 127) // Printable ASCII
                {
                    analysis += ch;
                }
                else
                {
                    analysis += _T("?");
                }
            }
            
            if (bstrLen > 50)
                analysis += _T("...");
                
            // Add hex dump for debugging
            analysis += _T("\nHex dump: ");
            for (UINT i = 0; i < bstrLen && i < 20; i++) // Limit to first 20 bytes
            {
                TCHAR ch = var.bstrVal[i];
                CString hexByte;
                hexByte.Format(_T("%02X "), (unsigned char)ch);
                analysis += hexByte;
            }
        }
    }
    else
    {
        analysis = _T("Not a valid BSTR");
    }
    
    return analysis;
}

// String validation for writing to database
CString CDatabaseUtils::validate_string_for_writing(const CString& input)
{
    CString validated = input;
    
    // Remove null characters
    validated.Remove('\0');
    
    // Limit length to reasonable database field size (255 characters)
    if (validated.GetLength() > 255)
    {
        validated = validated.Left(255);
    }
    
    // Replace problematic characters that might cause encoding issues
    // Replace non-printable ASCII characters with spaces
    for (int i = 0; i < validated.GetLength(); i++)
    {
        TCHAR ch = validated[i];
        if (ch < 32 && ch != '\t' && ch != '\n' && ch != '\r')
        {
            validated.SetAt(i, ' ');
        }
    }
    
    return validated;
}

BOOL CDatabaseUtils::is_valid_string_variant(const COleVariant& var)
{
    return (var.vt == VT_BSTR && var.bstrVal != nullptr);
}

__int64 CDatabaseUtils::safe_get_int64_from_variant(const COleVariant& var)
{
    try
    {
        if (var.vt == VT_I4)
            return var.lVal;
        else if (var.vt == VT_I2)
            return var.iVal;
        else if (var.vt == VT_R8)
            return static_cast<__int64>(var.dblVal);
        else if (var.vt == VT_R4)
            return static_cast<__int64>(var.fltVal);
        else if (var.vt == VT_BSTR && var.bstrVal != nullptr)
        {
            // Try to convert string to integer
            CString str(var.bstrVal);
            return _ttoi64(str);
        }
        else
            return 0;
    }
    catch (...)
    {
        return 0;
    }
}

double CDatabaseUtils::safe_get_double_from_variant(const COleVariant& var)
{
    try
    {
        if (var.vt == VT_R8)
            return var.dblVal;
        else if (var.vt == VT_R4)
            return var.fltVal;
        else if (var.vt == VT_I4)
            return static_cast<double>(var.lVal);
        else if (var.vt == VT_I2)
            return static_cast<double>(var.iVal);
        else if (var.vt == VT_BSTR && var.bstrVal != nullptr)
        {
            // Try to convert string to double
            CString str(var.bstrVal);
            return _ttof(str);
        }
        else
            return 0.0;
    }
    catch (...)
    {
        return 0.0;
    }
}

CString CDatabaseUtils::get_variant_type_string(const COleVariant& var)
{
    switch (var.vt)
    {
    case VT_EMPTY: return _T("VT_EMPTY");
    case VT_NULL: return _T("VT_NULL");
    case VT_I2: return _T("VT_I2");
    case VT_I4: return _T("VT_I4");
    case VT_R4: return _T("VT_R4");
    case VT_R8: return _T("VT_R8");
    case VT_CY: return _T("VT_CY");
    case VT_DATE: return _T("VT_DATE");
    case VT_BSTR: return _T("VT_BSTR");
    case VT_BOOL: return _T("VT_BOOL");
    case VT_VARIANT: return _T("VT_VARIANT");
    case VT_DECIMAL: return _T("VT_DECIMAL");
    case VT_I1: return _T("VT_I1");
    case VT_UI1: return _T("VT_UI1");
    case VT_UI2: return _T("VT_UI2");
    case VT_UI4: return _T("VT_UI4");
    case VT_I8: return _T("VT_I8");
    case VT_UI8: return _T("VT_UI8");
    default: return _T("VT_UNKNOWN");
    }
}

// CDatabaseAccessWrapper implementation
CDatabaseAccessWrapper::CDatabaseAccessWrapper(CDaoRecordset* pRecordset)
    : m_pRecordset(pRecordset)
{
}

CDatabaseAccessWrapper::~CDatabaseAccessWrapper()
{
}

CString CDatabaseAccessWrapper::get_field_value_as_string(int fieldIndex)
{
    if (!is_valid())
        return _T("");
        
    try
    {
        COleVariant var;
        m_pRecordset->GetFieldValue(fieldIndex, var);
        return CDatabaseUtils::safe_get_string_from_variant(var);
    }
    catch (CDaoException* e)
    {
        m_lastError.Format(_T("DAO Exception: %s"), e->m_pErrorInfo->m_strDescription);
        e->Delete();
        return _T("[Field Read Error]");
    }
    catch (...)
    {
        m_lastError = _T("Unknown error reading field");
        return _T("[Field Read Error]");
    }
}

CString CDatabaseAccessWrapper::get_field_value_as_string(const CString& fieldName)
{
    if (!is_valid())
        return _T("");
        
    try
    {
        COleVariant var;
        m_pRecordset->GetFieldValue(fieldName, var);
        return CDatabaseUtils::safe_get_string_from_variant(var);
    }
    catch (CDaoException* e)
    {
        m_lastError.Format(_T("DAO Exception: %s"), e->m_pErrorInfo->m_strDescription);
        e->Delete();
        return _T("[Field Read Error]");
    }
    catch (...)
    {
        m_lastError = _T("Unknown error reading field");
        return _T("[Field Read Error]");
    }
}

__int64 CDatabaseAccessWrapper::get_field_value_as_int64(int fieldIndex)
{
    if (!is_valid())
        return 0;
        
    try
    {
        COleVariant var;
        m_pRecordset->GetFieldValue(fieldIndex, var);
        return CDatabaseUtils::safe_get_int64_from_variant(var);
    }
    catch (CDaoException* e)
    {
        m_lastError.Format(_T("DAO Exception: %s"), e->m_pErrorInfo->m_strDescription);
        e->Delete();
        return 0;
    }
    catch (...)
    {
        m_lastError = _T("Unknown error reading field");
        return 0;
    }
}

__int64 CDatabaseAccessWrapper::get_field_value_as_int64(const CString& fieldName)
{
    if (!is_valid())
        return 0;
        
    try
    {
        COleVariant var;
        m_pRecordset->GetFieldValue(fieldName, var);
        return CDatabaseUtils::safe_get_int64_from_variant(var);
    }
    catch (CDaoException* e)
    {
        m_lastError.Format(_T("DAO Exception: %s"), e->m_pErrorInfo->m_strDescription);
        e->Delete();
        return 0;
    }
    catch (...)
    {
        m_lastError = _T("Unknown error reading field");
        return 0;
    }
}

double CDatabaseAccessWrapper::get_field_value_as_double(int fieldIndex)
{
    if (!is_valid())
        return 0.0;
        
    try
    {
        COleVariant var;
        m_pRecordset->GetFieldValue(fieldIndex, var);
        return CDatabaseUtils::safe_get_double_from_variant(var);
    }
    catch (CDaoException* e)
    {
        m_lastError.Format(_T("DAO Exception: %s"), e->m_pErrorInfo->m_strDescription);
        e->Delete();
        return 0.0;
    }
    catch (...)
    {
        m_lastError = _T("Unknown error reading field");
        return 0.0;
    }
}

double CDatabaseAccessWrapper::get_field_value_as_double(const CString& fieldName)
{
    if (!is_valid())
        return 0.0;
        
    try
    {
        COleVariant var;
        m_pRecordset->GetFieldValue(fieldName, var);
        return CDatabaseUtils::safe_get_double_from_variant(var);
    }
    catch (CDaoException* e)
    {
        m_lastError.Format(_T("DAO Exception: %s"), e->m_pErrorInfo->m_strDescription);
        e->Delete();
        return 0.0;
    }
    catch (...)
    {
        m_lastError = _T("Unknown error reading field");
        return 0.0;
    }
}

BOOL CDatabaseAccessWrapper::is_valid() const
{
    return (m_pRecordset != nullptr && m_pRecordset->IsOpen());
}

CString CDatabaseAccessWrapper::get_last_error() const
{
    return m_lastError;
}
