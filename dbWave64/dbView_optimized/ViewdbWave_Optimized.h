#pragma once

#include "ViewdbWave_SupportingClasses.h"
#include <afxwin.h>
#include <afxext.h>

#include "Editctrl.h"
#include "ViewDbTable.h"


// Forward declarations for MFC classes
class CdbWaveDoc;
class CdbWaveApp;
class DataListCtrl_Optimized;
class DataListCtrlConfiguration;
class CdbTableMain;
class CSpikeDoc;

// Main ViewdbWave_Optimized class
class ViewdbWave_Optimized : public ViewDbTable
{
    DECLARE_DYNCREATE(ViewdbWave_Optimized)

    enum { IDD = IDD_VIEWDBWAVE };

public:
    ViewdbWave_Optimized();
    virtual ~ViewdbWave_Optimized();

    // Initialization and setup
    void Initialize();
    void InitializeConfiguration();
    void InitializeControls();
    void InitializeDataListControl();
    void EnsureDataListControlInitialized();
    void make_controls_stretchable();

    // Document and application integration
    void SetDocument(CdbWaveDoc* pDoc);
    void SetApplication(CdbWaveApp* pApp);
    CdbWaveDoc* GetDocument() const;  // Override to use MFC framework
    CdbWaveApp* GetApplication() const { return m_pApplication; }
    
    // Data operations
    void LoadData();
    void LoadDataFromDocument(CdbWaveDoc* pDoc);
    void RefreshDisplay();
    void UpdateDisplay();
    void AutoRefresh();
    
    // Configuration management
    void LoadConfiguration();
    void SaveConfiguration();
    void ResetConfiguration();
    
    // Performance monitoring
    void EnablePerformanceMonitoring(bool enable);
    CString GetPerformanceReport() const;
    void ResetPerformanceMetrics();
    
    // State management
    ViewState GetCurrentState() const;
    bool IsReady() const;
    bool IsProcessing() const;
    bool HasError() const;
    
    // Error handling
    void HandleError(ViewdbWaveError error, const CString& message);
    void ClearError();
    CString GetLastErrorMessage() const;

protected:
    // MFC overrides
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnUpdate(CView* p_sender, const LPARAM l_hint, CObject* p_hint);
    
    // Message map functions
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg void OnUpdateViewRefresh(CCmdUI* pCmdUI);
    afx_msg void OnViewRefresh();
    afx_msg void OnUpdateViewAutoRefresh(CCmdUI* pCmdUI);
    afx_msg void OnViewAutoRefresh();
    
    DECLARE_MESSAGE_MAP()

private:
    // Core components
    CdbWaveDoc* m_pDocument; 
    CdbWaveApp* m_pApplication; 
    std::unique_ptr<DataListCtrl_Optimized> m_pDataListCtrl;
    std::unique_ptr<::DataListCtrlConfiguration> m_pConfiguration;
    
    // Supporting managers
    std::unique_ptr<ViewdbWaveStateManager> m_stateManager;
    std::unique_ptr<ViewdbWavePerformanceMonitor> m_performanceMonitor;
    std::unique_ptr<UIStateManager> m_uiStateManager;
    std::unique_ptr<AsyncOperationManager> m_asyncManager;
    std::unique_ptr<ViewdbWaveConfiguration> m_configManager;
    
    // Control references
    std::unique_ptr<CEditCtrl> m_pTimeFirstEdit;
    std::unique_ptr<CEditCtrl> m_pTimeLastEdit;
    std::unique_ptr<CEditCtrl> m_pAmplitudeSpanEdit;
    std::unique_ptr<CEditCtrl> m_pSpikeClassEdit;

    CButton* m_pDisplayDataButton;
    CButton* m_pDisplaySpikesButton;
    CButton* m_pDisplayNothingButton;
    CButton* m_pCheckFileNameButton;
    CButton* m_pFilterCheckButton;
    CButton* m_pRadioAllClassesButton;
    CButton* m_pRadioOneClassButton;
    CTabCtrl* m_pTabCtrl;
    
    // State variables
    bool m_initialized;
    bool m_autoRefreshEnabled;
    UINT_PTR m_autoRefreshTimer;
    std::chrono::steady_clock::time_point m_lastUpdateTime;
    
    // Thread safety
    mutable std::mutex m_viewMutex;
    
    // Private helper methods
    void OnStateChanged(ViewState oldState, ViewState newState);
    void UpdateControlStates();
    void UpdateControlValues();
    void ValidateConfiguration();
    void LogPerformanceMetrics(const CString& operation);
    
    // Configuration helpers
    void LoadControlValuesFromConfiguration();
    void SaveControlValuesToConfiguration();
    void ApplyConfigurationToControls();
    
    // Error handling helpers
    void LogError(ViewdbWaveError error, const CString& message);
    void DisplayErrorMessage(const CString& message);
    void ClearErrorMessage();
};
