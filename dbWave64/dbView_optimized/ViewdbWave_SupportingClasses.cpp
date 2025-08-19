#include "stdafx.h"
#include "ViewdbWave_SupportingClasses.h"
#include "RegistryManager.h"
#include <sstream>

// ViewdbWaveException implementation
ViewdbWaveException::ViewdbWaveException(ViewdbWaveError error, const CString& message)
    : m_error(error)
    , m_message(message)
{
}

const char* ViewdbWaveException::what() const noexcept
{
    static CStringA errorMessage;
    errorMessage = m_message;
    return errorMessage.GetString();
}

// ViewdbWaveStateManager implementation
ViewdbWaveStateManager::ViewdbWaveStateManager()
    : m_currentState(ViewState::UNINITIALIZED)
    , m_previousState(ViewState::UNINITIALIZED)
    , m_stateChangeCallback(nullptr)
{
}

void ViewdbWaveStateManager::SetState(ViewState newState)
{
    std::unique_lock<std::mutex> lock(m_stateMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    
    try
    {
        if (m_currentState != newState)
        {
            if (!CanTransitionTo(newState))
            {
                throw ViewdbWaveException(ViewdbWaveError::STATE_TRANSITION_FAILED, 
                    _T("Invalid state transition"));
            }
            
            m_previousState = m_currentState;
            m_currentState = newState;
            
            if (m_stateChangeCallback)
            {
                m_stateChangeCallback(m_previousState, m_currentState);
            }
        }
    }
    catch (const std::exception& e)
    {
        throw ViewdbWaveException(ViewdbWaveError::STATE_TRANSITION_FAILED, CString(e.what()));
    }
}

ViewState ViewdbWaveStateManager::GetCurrentState() const
{
    std::unique_lock<std::mutex> lock(m_stateMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    return m_currentState;
}

ViewState ViewdbWaveStateManager::GetPreviousState() const
{
    std::unique_lock<std::mutex> lock(m_stateMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    return m_previousState;
}

bool ViewdbWaveStateManager::CanTransitionTo(ViewState newState) const
{
    // Define valid state transitions
    switch (m_currentState)
    {
    case ViewState::UNINITIALIZED:
        return newState == ViewState::INITIALIZED || newState == ViewState::ERROR_STATE;
    case ViewState::INITIALIZED:
        return newState == ViewState::LOADING || newState == ViewState::READY || newState == ViewState::ERROR_STATE;
    case ViewState::LOADING:
        return newState == ViewState::READY || newState == ViewState::ERROR_STATE;
    case ViewState::READY:
        return newState == ViewState::PROCESSING || newState == ViewState::LOADING || newState == ViewState::ERROR_STATE;
    case ViewState::PROCESSING:
        return newState == ViewState::READY || newState == ViewState::ERROR_STATE;
    case ViewState::ERROR_STATE:
        return newState == ViewState::INITIALIZED || newState == ViewState::LOADING;
    default:
        return false;
    }
}

void ViewdbWaveStateManager::RegisterStateChangeCallback(StateChangeCallback callback)
{
    std::unique_lock<std::mutex> lock(m_stateMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    m_stateChangeCallback = callback;
}

void ViewdbWaveStateManager::Reset()
{
    std::unique_lock<std::mutex> lock(m_stateMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    m_currentState = ViewState::UNINITIALIZED;
    m_previousState = ViewState::UNINITIALIZED;
}

// ViewdbWavePerformanceMetrics implementation
void ViewdbWavePerformanceMetrics::Reset()
{
    lastUIUpdateTime = std::chrono::microseconds{0};
    averageUIUpdateTime = std::chrono::microseconds{0};
    totalUIUpdates = 0;
    asyncOperationsCompleted = 0;
    asyncOperationsFailed = 0;
    cacheHits = 0;
    cacheMisses = 0;
}

// ViewdbWavePerformanceMonitor implementation
ViewdbWavePerformanceMonitor::ViewdbWavePerformanceMonitor()
    : m_enabled(true)
    , m_startTime(std::chrono::steady_clock::now())
{
    Reset();
}

void ViewdbWavePerformanceMonitor::Reset()
{
    std::unique_lock<std::mutex> lock(m_metricsMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    m_operationCounts.clear();
    m_operationDurations.clear();
    m_memoryUsage.clear();
    m_startTime = std::chrono::steady_clock::now();
}

void ViewdbWavePerformanceMonitor::StartOperation(const CString& operationName)
{
    if (!m_enabled)
        return;
    
    std::unique_lock<std::mutex> lock(m_metricsMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    m_operationStartTimes[operationName] = std::chrono::steady_clock::now();
}

void ViewdbWavePerformanceMonitor::EndOperation(const CString& operationName)
{
    if (!m_enabled)
        return;
    
    std::unique_lock<std::mutex> lock(m_metricsMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    
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

ViewdbWavePerformanceMetrics ViewdbWavePerformanceMonitor::GetMetrics() const
{
    std::unique_lock<std::mutex> lock(m_metricsMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    
    ViewdbWavePerformanceMetrics metrics;
    
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

CString ViewdbWavePerformanceMonitor::GetPerformanceReport() const
{
    std::unique_lock<std::mutex> lock(m_metricsMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    
    CString report;
    report.Format(_T("Performance Report:\n"));
    
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

// UIStateManager implementation
UIStateManager::UIStateManager()
    : m_controlsEnabled(true)
    , m_controlsVisible(true)
    , m_loading(false)
    , m_error(false)
{
}

void UIStateManager::SetControlState(bool enabled)
{
    std::unique_lock<std::mutex> lock(m_uiMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    m_controlsEnabled = enabled;
}

void UIStateManager::UpdateControlVisibility(bool show)
{
    std::unique_lock<std::mutex> lock(m_uiMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    m_controlsVisible = show;
}

void UIStateManager::SetLoadingState(bool loading)
{
    std::unique_lock<std::mutex> lock(m_uiMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    m_loading = loading;
}

void UIStateManager::SetErrorState(bool error, const CString& errorMessage)
{
    std::unique_lock<std::mutex> lock(m_uiMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    m_error = error;
    m_errorMessage = errorMessage;
}

// AsyncOperationManager implementation
AsyncOperationManager::AsyncOperationManager()
    : m_shutdown(false)
{
}

AsyncOperationManager::~AsyncOperationManager()
{
    m_shutdown = true;
    WaitForAllOperations();
}

// Template method SubmitOperation is now implemented inline in the header file

void AsyncOperationManager::WaitForAllOperations()
{
    std::unique_lock<std::mutex> lock(m_operationsMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    
    for (auto& future : m_pendingOperations)
    {
        if (future.valid())
        {
            future.wait();
        }
    }
    
    m_pendingOperations.clear();
}

size_t AsyncOperationManager::GetActiveOperationCount() const
{
    std::unique_lock<std::mutex> lock(m_operationsMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    
    size_t count = 0;
    for (const auto& future : m_pendingOperations)
    {
        if (future.valid())
        {
            count++;
        }
    }
    
    return count;
}

void AsyncOperationManager::CancelAllOperations()
{
    m_shutdown = true;
    WaitForAllOperations();
    m_shutdown = false;
}

// ViewdbWaveConfiguration implementation
ViewdbWaveConfiguration::ViewdbWaveConfiguration()
    : m_timeFirst(0.0)
    , m_timeLast(100.0)
    , m_amplitudeSpan(1.0)
    , m_displayFileName(false)
    , m_filterEnabled(false)
{
}

void ViewdbWaveConfiguration::LoadFromRegistry(const CString& section)
{
    std::unique_lock<std::mutex> lock(m_configMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    
    try
    {
        // Use centralized registry manager
        auto& registry = RegistryManager::GetInstance();
        
        CString timeFirstStr = registry.GetProfileString(section, _T("TimeFirst"), _T("0.0"));
        CString timeLastStr = registry.GetProfileString(section, _T("TimeLast"), _T("100.0"));
        CString amplitudeSpanStr = registry.GetProfileString(section, _T("AmplitudeSpan"), _T("1.0"));
        
        m_timeFirst = _ttof(timeFirstStr);
        m_timeLast = _ttof(timeLastStr);
        m_amplitudeSpan = _ttof(amplitudeSpanStr);
        m_displayFileName = registry.GetProfileInt(section, _T("DisplayFileName"), 0) != 0;
        m_filterEnabled = registry.GetProfileInt(section, _T("FilterEnabled"), 0) != 0;
    }
    catch (const std::exception& e)
    {
        throw ViewdbWaveException(ViewdbWaveError::CONFIGURATION_ERROR, 
            _T("Failed to load configuration from registry: ") + CString(e.what()));
    }
}

void ViewdbWaveConfiguration::SaveToRegistry(const CString& section) const
{
    std::unique_lock<std::mutex> lock(m_configMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    
    try
    {
        // Use centralized registry manager
        auto& registry = RegistryManager::GetInstance();
        
        CString timeFirstStr, timeLastStr, amplitudeSpanStr;
        timeFirstStr.Format(_T("%.6f"), m_timeFirst);
        timeLastStr.Format(_T("%.6f"), m_timeLast);
        amplitudeSpanStr.Format(_T("%.6f"), m_amplitudeSpan);
        
        registry.WriteProfileString(section, _T("TimeFirst"), timeFirstStr);
        registry.WriteProfileString(section, _T("TimeLast"), timeLastStr);
        registry.WriteProfileString(section, _T("AmplitudeSpan"), amplitudeSpanStr);
        registry.WriteProfileInt(section, _T("DisplayFileName"), m_displayFileName ? 1 : 0);
        registry.WriteProfileInt(section, _T("FilterEnabled"), m_filterEnabled ? 1 : 0);
    }
    catch (const std::exception& e)
    {
        throw ViewdbWaveException(ViewdbWaveError::CONFIGURATION_ERROR, 
            _T("Failed to save configuration to registry: ") + CString(e.what()));
    }
}

void ViewdbWaveConfiguration::LoadFromIniFile(const CString& filename, const CString& section)
{
    std::unique_lock<std::mutex> lock(m_configMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    
    try
    {
        TCHAR buffer[256];
        
        GetPrivateProfileString(section, _T("TimeFirst"), _T("0.0"), buffer, sizeof(buffer), filename);
        m_timeFirst = _ttof(buffer);
        
        GetPrivateProfileString(section, _T("TimeLast"), _T("100.0"), buffer, sizeof(buffer), filename);
        m_timeLast = _ttof(buffer);
        
        GetPrivateProfileString(section, _T("AmplitudeSpan"), _T("1.0"), buffer, sizeof(buffer), filename);
        m_amplitudeSpan = _ttof(buffer);
        
        m_displayFileName = GetPrivateProfileInt(section, _T("DisplayFileName"), 0, filename) != 0;
        m_filterEnabled = GetPrivateProfileInt(section, _T("FilterEnabled"), 0, filename) != 0;
    }
    catch (const std::exception& e)
    {
        throw ViewdbWaveException(ViewdbWaveError::CONFIGURATION_ERROR, 
            _T("Failed to load configuration from INI file: ") + CString(e.what()));
    }
}

void ViewdbWaveConfiguration::SaveToIniFile(const CString& filename, const CString& section) const
{
    std::unique_lock<std::mutex> lock(m_configMutex, std::defer_lock);
    if (m_threadSafe) lock.lock();
    
    try
    {
        CString value;
        
        value.Format(_T("%.6f"), m_timeFirst);
        WritePrivateProfileString(section, _T("TimeFirst"), value, filename);
        
        value.Format(_T("%.6f"), m_timeLast);
        WritePrivateProfileString(section, _T("TimeLast"), value, filename);
        
        value.Format(_T("%.6f"), m_amplitudeSpan);
        WritePrivateProfileString(section, _T("AmplitudeSpan"), value, filename);
        
        WritePrivateProfileString(section, _T("DisplayFileName"), 
            m_displayFileName ? _T("1") : _T("0"), filename);
        
        WritePrivateProfileString(section, _T("FilterEnabled"), 
            m_filterEnabled ? _T("1") : _T("0"), filename);
    }
    catch (const std::exception& e)
    {
        throw ViewdbWaveException(ViewdbWaveError::CONFIGURATION_ERROR, 
            _T("Failed to save configuration to INI file: ") + CString(e.what()));
    }
}
