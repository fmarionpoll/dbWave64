#pragma once

#include "ViewdbWave_SupportingClasses.h"
#include <afxwin.h>
#include <afxext.h>

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
    
    // Document and application integration
    void SetDocument(CdbWaveDoc* pDoc);
    void SetApplication(CdbWaveApp* pApp);
    CdbWaveDoc* GetDocument() const { return m_pDocument; }
    CdbWaveApp* GetApplication() const { return m_pApplication; }
    
    // Data operations
    void LoadData();
    void LoadDataFromDocument();
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
    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual void DoDataExchange(CDataExchange* pDX);
    
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
    CdbWaveDoc* m_pDocument;  // Raw pointer - managed by application framework
    CdbWaveApp* m_pApplication;  // Raw pointer - managed by application framework
    std::unique_ptr<DataListCtrl_Optimized> m_pDataListCtrl;
    std::unique_ptr<::DataListCtrlConfiguration> m_pConfiguration;
    
    // Supporting managers
    std::unique_ptr<ViewdbWaveStateManager> m_stateManager;
    std::unique_ptr<ViewdbWavePerformanceMonitor> m_performanceMonitor;
    std::unique_ptr<UIStateManager> m_uiStateManager;
    std::unique_ptr<AsyncOperationManager> m_asyncManager;
    std::unique_ptr<ViewdbWaveConfiguration> m_configManager;
    
    // Control references
    CEdit* m_pTimeFirstEdit;
    CEdit* m_pTimeLastEdit;
    CEdit* m_pAmplitudeSpanEdit;
    CComboBox* m_pSpikeClassCombo;
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
