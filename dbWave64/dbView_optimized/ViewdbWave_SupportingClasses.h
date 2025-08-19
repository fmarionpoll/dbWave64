#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <functional>
#include <future>
#include <atomic>
#include <chrono>
#include <map>
#include <string>
#include <afxwin.h>


// Forward declarations
class CdbWaveDoc;
class CdbWaveApp;
class DataListCtrl_Optimized;
class DataListCtrlConfiguration;
class CdbTableMain;
class CSpikeDoc;
class CTabCtrl;
class CEdit;
class CButton;

// Constants namespace
namespace ViewdbWaveConstants
{
    // Control IDs (using VW_ prefix to avoid macro conflicts)
    constexpr int VW_IDC_TIMEFIRST = IDC_TIMEFIRST;
    constexpr int VW_IDC_TIMELAST = IDC_TIMELAST;
    constexpr int VW_IDC_AMPLITUDESPAN = IDC_AMPLITUDESPAN;
    constexpr int VW_IDC_SPIKECLASS = IDC_SPIKECLASS;
    constexpr int VW_IDC_DISPLAYDATA = IDC_DISPLAYDATA;
    constexpr int VW_IDC_DISPLAY_SPIKES = IDC_DISPLAY_SPIKES;
    constexpr int VW_IDC_DISPLAY_NOTHING = IDC_DISPLAY_NOTHING;
    constexpr int VW_IDC_CHECKFILENAME = IDC_CHECKFILENAME;
    constexpr int VW_IDC_FILTERCHECK = IDC_FILTERCHECK;
    constexpr int VW_IDC_RADIOALLCLASSES = IDC_RADIOALLCLASSES;
    constexpr int VW_IDC_RADIOONECLASS = IDC_RADIOONECLASS;
    constexpr int VW_IDC_TAB1 = IDC_TAB1;
    constexpr int VW_IDC_LISTCTRL = IDC_LISTCTRL;
    
    // Performance settings
    constexpr std::chrono::milliseconds UI_UPDATE_THROTTLE{16}; // ~60 FPS
    constexpr int MAX_ASYNC_OPERATIONS = 4;
    constexpr size_t MAX_CACHE_SIZE = 1000;
    
    // UI settings
    constexpr int DEFAULT_WINDOW_WIDTH = 800;
    constexpr int DEFAULT_WINDOW_HEIGHT = 600;
    constexpr int MIN_WINDOW_WIDTH = 400;
    constexpr int MIN_WINDOW_HEIGHT = 300;
    
    // Command IDs (if not defined in resource.h)
    constexpr int ID_VIEW_REFRESH = 32771;  // Custom refresh command
    constexpr int ID_VIEW_AUTO_REFRESH = 32772;  // Custom auto-refresh command
}

// Exception handling
enum class ViewdbWaveError
{
    SUCCESS,
    INVALID_DOCUMENT,
    INVALID_CONTROL,
    MEMORY_ALLOCATION_FAILED,
    UI_UPDATE_FAILED,
    ASYNC_OPERATION_FAILED,
    INITIALIZATION_FAILED,
    STATE_TRANSITION_FAILED,
    CONFIGURATION_ERROR,
    DATA_LOAD_FAILED
};

class ViewdbWaveException : public std::exception
{
public:
    explicit ViewdbWaveException(ViewdbWaveError error, const CString& message = _T(""));
    ViewdbWaveError GetError() const { return m_error; }
    CString GetMessage() const { return m_message; }
    const char* what() const noexcept override;
    
private:
    ViewdbWaveError m_error;
    CString m_message;
};

// View states
enum class ViewState
{
    UNINITIALIZED,
    INITIALIZED,
    LOADING,
    READY,
    PROCESSING,
    ERROR_STATE
};

// State change callback type
using StateChangeCallback = std::function<void(ViewState, ViewState)>;

// State management
class ViewdbWaveStateManager
{
public:
    ViewdbWaveStateManager();
    ~ViewdbWaveStateManager() = default;
    
    void SetState(ViewState newState);
    ViewState GetCurrentState() const;
    ViewState GetPreviousState() const;
    bool CanTransitionTo(ViewState newState) const;
    void RegisterStateChangeCallback(StateChangeCallback callback);
    void Reset();
    
private:
    ViewState m_currentState;
    ViewState m_previousState;
    StateChangeCallback m_stateChangeCallback;
    mutable std::mutex m_stateMutex;
};

// Performance monitoring
struct ViewdbWavePerformanceMetrics
{
    std::chrono::microseconds lastUIUpdateTime{0};
    std::chrono::microseconds averageUIUpdateTime{0};
    size_t totalUIUpdates = 0;
    size_t asyncOperationsCompleted = 0;
    size_t asyncOperationsFailed = 0;
    size_t cacheHits = 0;
    size_t cacheMisses = 0;
    
    void Reset();
};

class ViewdbWavePerformanceMonitor
{
public:
    ViewdbWavePerformanceMonitor();
    ~ViewdbWavePerformanceMonitor() = default;
    
    void Reset();
    void StartOperation(const CString& operationName);
    void EndOperation(const CString& operationName);
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    
    ViewdbWavePerformanceMetrics GetMetrics() const;
    CString GetPerformanceReport() const;
    
private:
    bool m_enabled;
    std::chrono::steady_clock::time_point m_startTime;
    std::map<CString, std::chrono::steady_clock::time_point> m_operationStartTimes;
    std::map<CString, size_t> m_operationCounts;
    std::map<CString, std::chrono::microseconds> m_operationDurations;
    std::map<CString, size_t> m_memoryUsage;
    mutable std::mutex m_metricsMutex;
};

// UI state management
class UIStateManager
{
public:
    UIStateManager();
    ~UIStateManager() = default;
    
    void SetControlState(bool enabled);
    void UpdateControlVisibility(bool show);
    void SetLoadingState(bool loading);
    void SetErrorState(bool error, const CString& errorMessage = _T(""));
    
private:
    bool m_controlsEnabled;
    bool m_controlsVisible;
    bool m_loading;
    bool m_error;
    CString m_errorMessage;
    std::mutex m_uiMutex;
};

// Async operation manager
class AsyncOperationManager
{
public:
    AsyncOperationManager();
    ~AsyncOperationManager();
    
    template<typename Func, typename... Args>
    auto SubmitOperation(Func&& func, Args&&... args) -> std::future<decltype(func(args...))>
    {
        return std::async(std::launch::async, std::forward<Func>(func), std::forward<Args>(args)...);
    }
    
    void WaitForAllOperations();
    size_t GetActiveOperationCount() const;
    void CancelAllOperations();
    
private:
    std::unique_ptr<std::thread> m_workerThread;
    std::vector<std::future<void>> m_pendingOperations;
    std::atomic<bool> m_shutdown;
    mutable std::mutex m_operationsMutex;
};

// Configuration manager
class ViewdbWaveConfiguration
{
public:
    ViewdbWaveConfiguration();
    ~ViewdbWaveConfiguration() = default;
    
    void LoadFromRegistry(const CString& section);
    void SaveToRegistry(const CString& section) const;
    void LoadFromIniFile(const CString& filename, const CString& section);
    void SaveToIniFile(const CString& filename, const CString& section) const;
    
    // Getters and setters for configuration values
    double GetTimeFirst() const { return m_timeFirst; }
    void SetTimeFirst(double value) { m_timeFirst = value; }
    
    double GetTimeLast() const { return m_timeLast; }
    void SetTimeLast(double value) { m_timeLast = value; }
    
    double GetAmplitudeSpan() const { return m_amplitudeSpan; }
    void SetAmplitudeSpan(double value) { m_amplitudeSpan = value; }
    
    bool IsDisplayFileNameEnabled() const { return m_displayFileName; }
    void SetDisplayFileName(bool enabled) { m_displayFileName = enabled; }
    
    bool IsFilterEnabled() const { return m_filterEnabled; }
    void SetFilterEnabled(bool enabled) { m_filterEnabled = enabled; }
    
private:
    double m_timeFirst;
    double m_timeLast;
    double m_amplitudeSpan;
    bool m_displayFileName;
    bool m_filterEnabled;
    mutable std::mutex m_configMutex;
};
