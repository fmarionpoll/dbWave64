#include "stdafx.h"
#include "DataListCtrl_SupportingClasses.h"
#include "DataListCtrl_Row_Optimized.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

// DataListCtrlException implementation
DataListCtrlException::DataListCtrlException(DataListCtrlError error, const CString& message)
    : m_error(error)
    , m_message(message)
{
}

const char* DataListCtrlException::what() const noexcept
{
    static CStringA errorMessage;
    errorMessage = m_message;
    return errorMessage.GetString();
}

// DataListCtrlCache implementation
DataListCtrlCache::DataListCtrlCache(size_t maxSize)
    : m_maxSize(maxSize)
    , m_hitCount(0)
    , m_missCount(0)
{
}

void DataListCtrlCache::AddRow(int index, const DataListCtrlRow& row)
{
    // This method is deprecated - use SetCachedRow instead for DataListCtrl_Row_Optimized
    // Keeping for backward compatibility but it won't work with the new cache structure
    TRACE(_T("DataListCtrlCache::AddRow - Deprecated method called, use SetCachedRow instead\n"));
}

bool DataListCtrlCache::GetRow(int index, DataListCtrlRow& row) const
{
    // This method is deprecated - use GetCachedRow instead for DataListCtrl_Row_Optimized
    // Keeping for backward compatibility but it won't work with the new cache structure
    TRACE(_T("DataListCtrlCache::GetRow - Deprecated method called, use GetCachedRow instead\n"));
    m_missCount++;
    return false;
}

void DataListCtrlCache::RemoveRow(int index)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    m_cache.erase(index);
}

void DataListCtrlCache::InvalidateCache(int index)
{
    // Alias for RemoveRow for compatibility
    RemoveRow(index);
}

void DataListCtrlCache::SetCachedRow(int index, DataListCtrl_Row_Optimized* row, int displayMode)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    
    // Remove existing entry if it exists
    m_cache.erase(index);
    
    // Add the new row
    if (row)
    {
        // Create a copy of the row for caching
        auto cachedRow = std::make_unique<DataListCtrl_Row_Optimized>(*row);
        m_cache[index] = CachedRowData(std::move(cachedRow), displayMode);
        
        TRACE(_T("DataListCtrlCache::SetCachedRow - Cached row data for index: %d\n"), index);
        
        // Evict oldest entries if cache is full
        if (m_cache.size() > m_maxSize)
        {
            auto oldest = m_cache.begin();
            m_cache.erase(oldest);
            TRACE(_T("DataListCtrlCache::SetCachedRow - Evicted oldest cache entry\n"));
        }
    }
}

DataListCtrl_Row_Optimized* DataListCtrlCache::GetCachedRow(int index) const
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    
    auto it = m_cache.find(index);
    if (it != m_cache.end() && it->second.isValid)
    {
        // Check if the cached data is still valid (not expired)
        auto now = std::chrono::steady_clock::now();
        if (now - it->second.timestamp < std::chrono::minutes(10)) // 10 minute expiry
        {
            // Cache hit - return the cached row
            m_hitCount++;
            TRACE(_T("DataListCtrlCache::GetCachedRow - Cache HIT for index: %d\n"), index);
            return it->second.row.get();
        }
        else
        {
            // Expired, remove it
            const_cast<DataListCtrlCache*>(this)->m_cache.erase(it);
            TRACE(_T("DataListCtrlCache::GetCachedRow - Cache entry expired for index: %d\n"), index);
        }
    }
    
    // Cache miss
    m_missCount++;
    TRACE(_T("DataListCtrlCache::GetCachedRow - Cache MISS for index: %d\n"), index);
    return nullptr;
}

void DataListCtrlCache::Clear()
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    m_cache.clear();
}

size_t DataListCtrlCache::GetSize() const
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    return m_cache.size();
}

void DataListCtrlCache::SetMaxSize(size_t maxSize)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    m_maxSize = maxSize;
    
    // Remove excess entries if necessary
    while (m_cache.size() > m_maxSize)
    {
        m_cache.erase(m_cache.begin());
    }
}

double DataListCtrlCache::GetHitRatio() const
{
    size_t total = m_hitCount + m_missCount;
    if (total == 0)
        return 0.0;
    
    return static_cast<double>(m_hitCount) / total;
}

void DataListCtrlCache::ResetStats()
{
    m_hitCount = 0;
    m_missCount = 0;
}

// Bitmap caching methods (for DataListCtrl_Row_Optimized)
CBitmap* DataListCtrlCache::GetCachedBitmap(int index, int displayMode)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    
    auto it = m_bitmapCache.find(index);
    if (it != m_bitmapCache.end() && it->second.isValid && it->second.displayMode == displayMode)
    {
        auto now = std::chrono::steady_clock::now();
        if (now - it->second.timestamp < std::chrono::minutes(5)) // 5 minute expiry
        {
            return it->second.bitmap.get();
        }
        else
        {
            // Expired, remove it
            m_bitmapCache.erase(it);
        }
    }
    
    return nullptr;
}

void DataListCtrlCache::SetCachedBitmap(int index, std::unique_ptr<CBitmap> bitmap, int displayMode)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    
    EvictOldestIfNeeded();
    
    m_bitmapCache[index] = CachedBitmap(std::move(bitmap), displayMode);
}

void DataListCtrlCache::ClearExpiredCache()
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    
    auto now = std::chrono::steady_clock::now();
    for (auto it = m_bitmapCache.begin(); it != m_bitmapCache.end();)
    {
        if (now - it->second.timestamp >= std::chrono::minutes(5)) // 5 minute expiry
        {
            it = m_bitmapCache.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void DataListCtrlCache::ClearAll()
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    m_cache.clear();
    m_bitmapCache.clear();
}

void DataListCtrlCache::EvictOldestIfNeeded()
{
    if (m_bitmapCache.size() >= m_maxSize)
    {
        auto oldest = m_bitmapCache.begin();
        for (auto it = m_bitmapCache.begin(); it != m_bitmapCache.end(); ++it)
        {
            if (it->second.timestamp < oldest->second.timestamp)
            {
                oldest = it;
            }
        }
        m_bitmapCache.erase(oldest);
    }
}

// GdiResourceManager implementation
GdiResourceManager::GdiResourceManager()
{
}

GdiResourceManager::~GdiResourceManager()
{
    // All resources will be automatically cleaned up by unique_ptr
}

CDC* GdiResourceManager::CreateCompatibleDC(CDC* pDC)
{
    std::lock_guard<std::mutex> lock(m_resourceMutex);
    
    auto pNewDC = std::make_unique<CDC>();
    if (pNewDC->CreateCompatibleDC(pDC))
    {
        CDC* pResult = pNewDC.get();
        m_dcList.push_back(std::move(pNewDC));
        return pResult;
    }
    
    return nullptr;
}

void GdiResourceManager::DeleteDC(CDC* pDC)
{
    std::lock_guard<std::mutex> lock(m_resourceMutex);
    
    auto it = std::find_if(m_dcList.begin(), m_dcList.end(),
        [pDC](const std::unique_ptr<CDC>& dc) { return dc.get() == pDC; });
    
    if (it != m_dcList.end())
    {
        m_dcList.erase(it);
    }
}

CBitmap* GdiResourceManager::CreateBitmap(int width, int height, int planes, int bitsPerPixel, const void* data)
{
    std::lock_guard<std::mutex> lock(m_resourceMutex);
    
    auto pBitmap = std::make_unique<CBitmap>();
    if (pBitmap->CreateBitmap(width, height, planes, bitsPerPixel, data))
    {
        CBitmap* pResult = pBitmap.get();
        m_bitmapList.push_back(std::move(pBitmap));
        return pResult;
    }
    
    return nullptr;
}

void GdiResourceManager::DeleteBitmap(CBitmap* pBitmap)
{
    std::lock_guard<std::mutex> lock(m_resourceMutex);
    
    auto it = std::find_if(m_bitmapList.begin(), m_bitmapList.end(),
        [pBitmap](const std::unique_ptr<CBitmap>& bmp) { return bmp.get() == pBitmap; });
    
    if (it != m_bitmapList.end())
    {
        m_bitmapList.erase(it);
    }
}

CBrush* GdiResourceManager::CreateSolidBrush(COLORREF color)
{
    std::lock_guard<std::mutex> lock(m_resourceMutex);
    
    auto pBrush = std::make_unique<CBrush>();
    if (pBrush->CreateSolidBrush(color))
    {
        CBrush* pResult = pBrush.get();
        m_brushList.push_back(std::move(pBrush));
        return pResult;
    }
    
    return nullptr;
}

void GdiResourceManager::DeleteBrush(CBrush* pBrush)
{
    std::lock_guard<std::mutex> lock(m_resourceMutex);
    
    auto it = std::find_if(m_brushList.begin(), m_brushList.end(),
        [pBrush](const std::unique_ptr<CBrush>& brush) { return brush.get() == pBrush; });
    
    if (it != m_brushList.end())
    {
        m_brushList.erase(it);
    }
}

CPen* GdiResourceManager::CreatePen(int style, int width, COLORREF color)
{
    std::lock_guard<std::mutex> lock(m_resourceMutex);
    
    auto pPen = std::make_unique<CPen>();
    if (pPen->CreatePen(style, width, color))
    {
        CPen* pResult = pPen.get();
        m_penList.push_back(std::move(pPen));
        return pResult;
    }
    
    return nullptr;
}

void GdiResourceManager::DeletePen(CPen* pPen)
{
    std::lock_guard<std::mutex> lock(m_resourceMutex);
    
    auto it = std::find_if(m_penList.begin(), m_penList.end(),
        [pPen](const std::unique_ptr<CPen>& pen) { return pen.get() == pPen; });
    
    if (it != m_penList.end())
    {
        m_penList.erase(it);
    }
}

CImageList* GdiResourceManager::CreateImageList(int width, int height, UINT flags, int initial, int grow)
{
    std::lock_guard<std::mutex> lock(m_resourceMutex);
    
    auto pImageList = std::make_unique<CImageList>();
    if (pImageList->Create(width, height, flags, initial, grow))
    {
        CImageList* pResult = pImageList.get();
        m_imageListList.push_back(std::move(pImageList));
        return pResult;
    }
    
    return nullptr;
}

void GdiResourceManager::DeleteImageList(CImageList* pImageList)
{
    std::lock_guard<std::mutex> lock(m_resourceMutex);
    
    auto it = std::find_if(m_imageListList.begin(), m_imageListList.end(),
        [pImageList](const std::unique_ptr<CImageList>& imgList) { return imgList.get() == pImageList; });
    
    if (it != m_imageListList.end())
    {
        m_imageListList.erase(it);
    }
}

// DataListCtrlPerformanceMonitor implementation
DataListCtrlPerformanceMonitor::DataListCtrlPerformanceMonitor()
    : m_enabled(true)
    , m_startTime(std::chrono::steady_clock::now())
{
    Reset();
}

void DataListCtrlPerformanceMonitor::StartOperation(const CString& operationName)
{
    if (!m_enabled)
        return;
    
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    m_operationStartTimes[operationName] = std::chrono::steady_clock::now();
}

void DataListCtrlPerformanceMonitor::EndOperation(const CString& operationName)
{
    if (!m_enabled)
        return;
    
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    auto it = m_operationStartTimes.find(operationName);
    if (it != m_operationStartTimes.end())
    {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - it->second);
        
        m_operationDurations[operationName] += duration;
        m_operationCounts[operationName]++;
        
        m_operationStartTimes.erase(it);
    }
}

void DataListCtrlPerformanceMonitor::Metrics::Reset()
{
    lastUIUpdateTime = std::chrono::microseconds{0};
    averageUIUpdateTime = std::chrono::microseconds{0};
    totalUIUpdates = 0;
    cacheHits = 0;
    cacheMisses = 0;
    rowOperations = 0;
    displayModeChanges = 0;
}

DataListCtrlPerformanceMonitor::Metrics DataListCtrlPerformanceMonitor::GetMetrics() const
{
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    Metrics metrics;
    
    // Calculate averages
    for (const auto& pair : m_operationDurations)
    {
        auto count = m_operationCounts.find(pair.first);
        if (count != m_operationCounts.end() && count->second > 0)
        {
            auto avgDuration = pair.second / count->second;
            if (pair.first == _T("UIUpdate"))
            {
                metrics.lastUIUpdateTime = avgDuration;
                metrics.averageUIUpdateTime = avgDuration;
                metrics.totalUIUpdates = count->second;
            }
        }
    }
    
    return metrics;
}

CString DataListCtrlPerformanceMonitor::GetPerformanceReport() const
{
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    CString report;
    report.Format(_T("DataListCtrl Performance Report:\n"));
    
    for (const auto& pair : m_operationDurations)
    {
        auto count = m_operationCounts.find(pair.first);
        if (count != m_operationCounts.end() && count->second > 0)
        {
            auto avgDuration = pair.second / count->second;
            CString operationReport;
            operationReport.Format(_T("  %s: %d calls, avg %.2f ms\n"), 
                pair.first, count->second, avgDuration.count() / 1000.0);
            report += operationReport;
        }
    }
    
    return report;
}

void DataListCtrlPerformanceMonitor::Reset()
{
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    m_operationCounts.clear();
    m_operationDurations.clear();
    m_startTime = std::chrono::steady_clock::now();
}

// DataListCtrlConfigurationValidator implementation
bool DataListCtrlConfigurationValidator::ValidateImageDimensions(int width, int height)
{
    return width > 0 && height > 0 && width <= 1024 && height <= 1024;
}

bool DataListCtrlConfigurationValidator::ValidateTimeRange(double first, double last)
{
    return first >= 0.0 && last > first;
}

bool DataListCtrlConfigurationValidator::ValidateAmplitudeSpan(double span)
{
    return span > 0.0;
}

bool DataListCtrlConfigurationValidator::ValidateDisplayMode(int mode)
{
    return mode >= 0 && mode <= 3; // Assuming 4 display modes (0-3)
}

bool DataListCtrlConfigurationValidator::ValidateCacheSize(size_t size)
{
    return size > 0 && size <= 10000;
}

bool DataListCtrlConfigurationValidator::ValidateBatchSize(size_t size)
{
    return size > 0 && size <= 1000;
}

CString DataListCtrlConfigurationValidator::GetValidationErrorMessage(DataListCtrlError error)
{
    switch (error)
    {
    case DataListCtrlError::INVALID_PARAMETER:
        return _T("Invalid parameter provided");
    case DataListCtrlError::MEMORY_ALLOCATION_FAILED:
        return _T("Memory allocation failed");
    case DataListCtrlError::UI_UPDATE_FAILED:
        return _T("UI update failed");
    case DataListCtrlError::CACHE_OPERATION_FAILED:
        return _T("Cache operation failed");
    case DataListCtrlError::INITIALIZATION_FAILED:
        return _T("Initialization failed");
    case DataListCtrlError::CONFIGURATION_ERROR:
        return _T("Configuration error");
    case DataListCtrlError::ROW_OPERATION_FAILED:
        return _T("Row operation failed");
    case DataListCtrlError::DISPLAY_MODE_ERROR:
        return _T("Display mode error");
    default:
        return _T("Unknown error");
    }
}

// DataListCtrlRowManager implementation
DataListCtrlRowManager::DataListCtrlRowManager()
{
}

void DataListCtrlRowManager::AddRow(const DataListCtrlRow& row)
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    m_rows.push_back(row);
}

void DataListCtrlRowManager::RemoveRow(int index)
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    
    auto it = std::find_if(m_rows.begin(), m_rows.end(),
        [index](const DataListCtrlRow& row) { return row.index == index; });
    
    if (it != m_rows.end())
    {
        m_rows.erase(it);
    }
}

void DataListCtrlRowManager::UpdateRow(int index, const DataListCtrlRow& row)
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    
    auto it = std::find_if(m_rows.begin(), m_rows.end(),
        [index](const DataListCtrlRow& r) { return r.index == index; });
    
    if (it != m_rows.end())
    {
        *it = row;
    }
}

bool DataListCtrlRowManager::GetRow(int index, DataListCtrlRow& row) const
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    
    auto it = std::find_if(m_rows.begin(), m_rows.end(),
        [index](const DataListCtrlRow& r) { return r.index == index; });
    
    if (it != m_rows.end())
    {
        row = *it;
        return true;
    }
    
    return false;
}

void DataListCtrlRowManager::ClearRows()
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    m_rows.clear();
}

void DataListCtrlRowManager::AddRows(const std::vector<DataListCtrlRow>& rows)
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    m_rows.insert(m_rows.end(), rows.begin(), rows.end());
}

void DataListCtrlRowManager::RemoveRows(const std::vector<int>& indices)
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    
    for (int index : indices)
    {
        auto it = std::find_if(m_rows.begin(), m_rows.end(),
            [index](const DataListCtrlRow& row) { return row.index == index; });
        
        if (it != m_rows.end())
        {
            m_rows.erase(it);
        }
    }
}

void DataListCtrlRowManager::UpdateRows(const std::map<int, DataListCtrlRow>& rowUpdates)
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    
    for (const auto& pair : rowUpdates)
    {
        auto it = std::find_if(m_rows.begin(), m_rows.end(),
            [pair](const DataListCtrlRow& row) { return row.index == pair.first; });
        
        if (it != m_rows.end())
        {
            *it = pair.second;
        }
    }
}

size_t DataListCtrlRowManager::GetRowCount() const
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    return m_rows.size();
}

std::vector<DataListCtrlRow> DataListCtrlRowManager::GetVisibleRows() const
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    
    std::vector<DataListCtrlRow> visibleRows;
    std::copy_if(m_rows.begin(), m_rows.end(), std::back_inserter(visibleRows),
        [](const DataListCtrlRow& row) { return row.visible; });
    
    return visibleRows;
}

std::vector<DataListCtrlRow> DataListCtrlRowManager::GetSelectedRows() const
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    
    std::vector<DataListCtrlRow> selectedRows;
    std::copy_if(m_rows.begin(), m_rows.end(), std::back_inserter(selectedRows),
        [](const DataListCtrlRow& row) { return row.selected; });
    
    return selectedRows;
}

std::vector<int> DataListCtrlRowManager::GetRowIndices() const
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    
    std::vector<int> indices;
    indices.reserve(m_rows.size());
    
    for (const auto& row : m_rows)
    {
        indices.push_back(row.index);
    }
    
    return indices;
}

void DataListCtrlRowManager::SetFilter(std::function<bool(const DataListCtrlRow&)> filter)
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    m_filter = filter;
}

void DataListCtrlRowManager::ClearFilter()
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    m_filter = nullptr;
}

void DataListCtrlRowManager::SortRows(std::function<bool(const DataListCtrlRow&, const DataListCtrlRow&)> comparator)
{
    std::lock_guard<std::mutex> lock(m_rowsMutex);
    std::sort(m_rows.begin(), m_rows.end(), comparator);
}

// DataListCtrlDisplayManager implementation
DataListCtrlDisplayManager::DataListCtrlDisplayManager()
    : m_displayMode(0)
    , m_backgroundColor(RGB(255, 255, 255))
    , m_textColor(RGB(0, 0, 0))
    , m_selectionColor(RGB(0, 120, 215))
{
}

void DataListCtrlDisplayManager::SetDisplayMode(int mode)
{
    std::lock_guard<std::mutex> lock(m_displayMutex);
    
    if (IsValidDisplayMode(mode))
    {
        m_displayMode = mode;
    }
}

bool DataListCtrlDisplayManager::IsValidDisplayMode(int mode) const
{
    return mode >= 0 && mode <= 3; // Assuming 4 display modes (0-3)
}

void DataListCtrlDisplayManager::CreateImageList(int width, int height)
{
    std::lock_guard<std::mutex> lock(m_displayMutex);
    
    m_pImageList = std::make_unique<CImageList>();
    m_pImageList->Create(width, height, ILC_COLOR32 | ILC_MASK, 3, 1);
}

void DataListCtrlDisplayManager::DestroyImageList()
{
    std::lock_guard<std::mutex> lock(m_displayMutex);
    m_pImageList.reset();
}

void DataListCtrlDisplayManager::DrawRow(CDC* pDC, const DataListCtrlRow& row, const CRect& rect)
{
    if (!pDC)
        return;
    
    std::lock_guard<std::mutex> lock(m_displayMutex);
    
    // Draw background
    DrawBackground(pDC, rect);
    
    // Draw selection if selected
    if (row.selected)
    {
        DrawSelection(pDC, rect);
    }
    
    // Draw text
    pDC->SetTextColor(m_textColor);
    pDC->SetBkMode(TRANSPARENT);
    
    CString text;
    text.Format(_T("%d"), row.index);
    pDC->TextOut(rect.left + 5, rect.top + 2, text);
}

void DataListCtrlDisplayManager::DrawBackground(CDC* pDC, const CRect& rect)
{
    if (!pDC)
        return;
    
    CBrush brush(m_backgroundColor);
    pDC->FillRect(&rect, &brush);
}

void DataListCtrlDisplayManager::DrawSelection(CDC* pDC, const CRect& rect)
{
    if (!pDC)
        return;
    
    CBrush brush(m_selectionColor);
    pDC->FillRect(&rect, &brush);
}

void DataListCtrlDisplayManager::SetBackgroundColor(COLORREF color)
{
    std::lock_guard<std::mutex> lock(m_displayMutex);
    m_backgroundColor = color;
}

void DataListCtrlDisplayManager::SetTextColor(COLORREF color)
{
    std::lock_guard<std::mutex> lock(m_displayMutex);
    m_textColor = color;
}

void DataListCtrlDisplayManager::SetSelectionColor(COLORREF color)
{
    std::lock_guard<std::mutex> lock(m_displayMutex);
    m_selectionColor = color;
}
