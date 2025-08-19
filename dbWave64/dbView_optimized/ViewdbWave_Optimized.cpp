#include "StdAfx.h"
#include "ViewdbWave_Optimized.h"
#include "ViewdbWave_SupportingClasses.h"
#include "DataListCtrl_Optimized.h"
#include "DataListCtrl_Configuration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Main ViewdbWave_Optimized implementation
IMPLEMENT_DYNCREATE(ViewdbWave_Optimized, ViewDbTable)

BEGIN_MESSAGE_MAP(ViewdbWave_Optimized, ViewDbTable)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_UPDATE_COMMAND_UI(ViewdbWaveConstants::ID_VIEW_REFRESH, OnUpdateViewRefresh)
    ON_COMMAND(ViewdbWaveConstants::ID_VIEW_REFRESH, OnViewRefresh)
    ON_UPDATE_COMMAND_UI(ViewdbWaveConstants::ID_VIEW_AUTO_REFRESH, OnUpdateViewAutoRefresh)
    ON_COMMAND(ViewdbWaveConstants::ID_VIEW_AUTO_REFRESH, OnViewAutoRefresh)
END_MESSAGE_MAP()

ViewdbWave_Optimized::ViewdbWave_Optimized()
    : ViewDbTable(IDD)
    , m_pDocument(nullptr)
    , m_pApplication(nullptr)
    , m_pDataListCtrl(std::make_unique<DataListCtrl_Optimized>())
    , m_pConfiguration(std::make_unique<::DataListCtrlConfiguration>())
    , m_stateManager(std::make_unique<ViewdbWaveStateManager>())
    , m_performanceMonitor(std::make_unique<ViewdbWavePerformanceMonitor>())
    , m_uiStateManager(std::make_unique<UIStateManager>())
    , m_asyncManager(std::make_unique<AsyncOperationManager>())
    , m_configManager(std::make_unique<ViewdbWaveConfiguration>())
    , m_pTimeFirstEdit(std::make_unique<CEditCtrl>())
    , m_pTimeLastEdit(std::make_unique<CEditCtrl>())
    , m_pAmplitudeSpanEdit(std::make_unique<CEditCtrl>())
    , m_pSpikeClassEdit(std::make_unique<CEditCtrl>())
    , m_pDisplayDataButton(nullptr)
    , m_pDisplaySpikesButton(nullptr)
    , m_pDisplayNothingButton(nullptr)
    , m_pCheckFileNameButton(nullptr)
    , m_pFilterCheckButton(nullptr)
    , m_pRadioAllClassesButton(nullptr)
    , m_pRadioOneClassButton(nullptr)
    , m_pTabCtrl(nullptr)
    , m_initialized(false)
    , m_autoRefreshEnabled(false)
    , m_autoRefreshTimer(0)
    , m_lastUpdateTime(std::chrono::steady_clock::now())
{
}

ViewdbWave_Optimized::~ViewdbWave_Optimized()
{
    if (m_autoRefreshTimer)
    {
        KillTimer(m_autoRefreshTimer);
    }
}

void ViewdbWave_Optimized::DoDataExchange(CDataExchange* pDX)
{
    ViewDbTable::DoDataExchange(pDX);
    
    // Associate the DataListCtrl_Optimized with the dialog control
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_LISTCTRL, *m_pDataListCtrl);
    
    // Add data exchange for controls if needed
    // Example: DDX_Text(pDX, IDC_TIMEFIRST, m_timeFirst);
}

void ViewdbWave_Optimized::Initialize()
{
    try
    {
        m_performanceMonitor->StartOperation(_T("Initialize"));
        
        if (m_initialized)
        {
            throw ViewdbWaveException(ViewdbWaveError::INITIALIZATION_FAILED, 
                _T("View already initialized"));
        }
        
        // Set initial state
        m_stateManager->SetState(ViewState::INITIALIZED);

        InitializeControls();
        InitializeDataListControl();
        make_controls_stretchable();
        InitializeConfiguration();

        // Set up state change callback
        m_stateManager->RegisterStateChangeCallback([this](ViewState oldState, ViewState newState) {
            OnStateChanged(oldState, newState);
        });
        
        m_initialized = true;
        m_stateManager->SetState(ViewState::READY);
        
        m_performanceMonitor->EndOperation(_T("Initialize"));
    }
    catch (const std::exception& e)
    {
        m_stateManager->SetState(ViewState::ERROR_STATE);
        HandleError(ViewdbWaveError::INITIALIZATION_FAILED, CString(e.what()));
    }
}

void ViewdbWave_Optimized::InitializeConfiguration()
{
    try
    {
        m_performanceMonitor->StartOperation(_T("InitializeConfiguration"));
        
        // Load configuration from registry
        m_configManager->LoadFromRegistry(_T("ViewdbWave"));
        
        // Apply configuration to controls
        ApplyConfigurationToControls();
        
        m_performanceMonitor->EndOperation(_T("InitializeConfiguration"));
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::CONFIGURATION_ERROR, CString(e.what()));
    }
}

void ViewdbWave_Optimized::InitializeControls()
{
    try
    {
        m_performanceMonitor->StartOperation(_T("InitializeControls"));

        VERIFY(m_pTimeFirstEdit->SubclassDlgItem(ViewdbWaveConstants::VW_IDC_TIMEFIRST, this));
        VERIFY(m_pTimeLastEdit->SubclassDlgItem(ViewdbWaveConstants::VW_IDC_TIMELAST, this));
        VERIFY(m_pAmplitudeSpanEdit->SubclassDlgItem(ViewdbWaveConstants::VW_IDC_AMPLITUDESPAN, this));
        VERIFY(m_pSpikeClassEdit->SubclassDlgItem(ViewdbWaveConstants::VW_IDC_SPIKECLASS, this));

        m_pDisplayDataButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_DISPLAYDATA));
        m_pDisplaySpikesButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_DISPLAY_SPIKES));
        m_pDisplayNothingButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_DISPLAY_NOTHING));

        m_pCheckFileNameButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_CHECKFILENAME));
        m_pFilterCheckButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_FILTERCHECK));
        m_pRadioAllClassesButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_RADIOALLCLASSES));
        m_pRadioOneClassButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_RADIOONECLASS));
        m_pTabCtrl = static_cast<CTabCtrl*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_TAB1));
        
        // Validate controls
        if (!m_pTimeFirstEdit || !m_pTimeLastEdit || !m_pAmplitudeSpanEdit || !m_pSpikeClassEdit)
        {
            throw ViewdbWaveException(ViewdbWaveError::INVALID_CONTROL, 
                _T("Required controls not found"));
        }
        
        // Set initial control states
        UpdateControlStates();
        
        m_performanceMonitor->EndOperation(_T("InitializeControls"));
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::INVALID_CONTROL, CString(e.what()));
    }
}

void ViewdbWave_Optimized::InitializeDataListControl()
{
    try
    {
        m_performanceMonitor->StartOperation(_T("InitializeDataListControl"));
        
        if (!m_pDataListCtrl)
        {
            throw ViewdbWaveException(ViewdbWaveError::INVALID_CONTROL, 
                _T("Data list control not initialized"));
        }

        // Check if the control has a valid HWND before initializing
        if (!m_pDataListCtrl->GetSafeHwnd())
        {
            // Control not ready yet, this is expected during early initialization
            // The control will be initialized later when the HWND is available
            m_performanceMonitor->EndOperation(_T("InitializeDataListControl"));
            return;
        }

        m_pDataListCtrl->Initialize(*m_pConfiguration);
        
        m_performanceMonitor->EndOperation(_T("InitializeDataListControl"));
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::INVALID_CONTROL, CString(e.what()));
    }
}

void ViewdbWave_Optimized::EnsureDataListControlInitialized()
{
    try
    {
        if (!m_pDataListCtrl)
        {
            return;
        }

        // Check if the control has a valid HWND and hasn't been initialized yet
        if (m_pDataListCtrl->GetSafeHwnd() && !m_pDataListCtrl->IsInitialized())
        {
            m_pDataListCtrl->Initialize(*m_pConfiguration);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::INVALID_CONTROL, CString(e.what()));
    }
}

void ViewdbWave_Optimized::make_controls_stretchable()
{
    stretch_.attach_parent(this);
    stretch_.new_prop(IDC_LISTCTRL, XLEQ_XREQ, YTEQ_YBEQ);
    stretch_.new_prop(IDC_TAB1, XLEQ_XREQ, SZEQ_YBEQ);
    b_init_ = TRUE;
}

void ViewdbWave_Optimized::SetDocument(CdbWaveDoc* pDoc)
{
    std::lock_guard<std::mutex> lock(m_viewMutex);
    m_pDocument = pDoc;  // Simple assignment for raw pointer
    
    if (m_pDocument && m_initialized)
    {
        LoadData();
    }
}

void ViewdbWave_Optimized::SetApplication(CdbWaveApp* pApp)
{
    std::lock_guard<std::mutex> lock(m_viewMutex);
    m_pApplication = pApp;  // Simple assignment for raw pointer
}

void ViewdbWave_Optimized::LoadData()
{
    try
    {
        m_performanceMonitor->StartOperation(_T("LoadData"));
        
        if (!m_pDocument)
        {
            throw ViewdbWaveException(ViewdbWaveError::INVALID_DOCUMENT, 
                _T("No document available"));
        }
        
        m_stateManager->SetState(ViewState::LOADING);
        
        // Load data directly (simplified approach)
        LoadDataFromDocument();
        
        m_stateManager->SetState(ViewState::READY);
        
        m_performanceMonitor->EndOperation(_T("LoadData"));
    }
    catch (const std::exception& e)
    {
        m_stateManager->SetState(ViewState::ERROR_STATE);
        HandleError(ViewdbWaveError::INVALID_DOCUMENT, CString(e.what()));
    }
}

void ViewdbWave_Optimized::LoadDataFromDocument()
{
    try
    {
        // Direct data loading from document
        // This replaces the asynchronous approach with simple, direct loading
        
        if (m_pDocument)
        {
            // Load data from the document
            // This is where you would implement the actual data loading logic
            // For now, we'll just simulate a brief loading operation
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            
            // Update the data list control if available
            if (m_pDataListCtrl)
            {
                m_pDataListCtrl->RefreshDisplay();
            }
        }
        
        LogPerformanceMetrics(_T("LoadDataFromDocument"));
    }
    catch (const std::exception& e)
    {
        throw ViewdbWaveException(ViewdbWaveError::DATA_LOAD_FAILED, CString(e.what()));
    }
}

void ViewdbWave_Optimized::RefreshDisplay()
{
    try
    {
        m_performanceMonitor->StartOperation(_T("RefreshDisplay"));
        
        if (!m_initialized)
        {
            throw ViewdbWaveException(ViewdbWaveError::INVALID_CONTROL, 
                _T("View not initialized"));
        }
        
        UpdateDisplay();
        
        m_performanceMonitor->EndOperation(_T("RefreshDisplay"));
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void ViewdbWave_Optimized::UpdateDisplay()
{
    try
    {
        std::lock_guard<std::mutex> lock(m_viewMutex);
        
        // Update control values from configuration
        UpdateControlValues();
        
        // Update the data list control
        if (m_pDataListCtrl)
        {
            m_pDataListCtrl->RefreshDisplay();
        }
        
        // Update the view
        Invalidate();
        UpdateWindow();
        
        m_lastUpdateTime = std::chrono::steady_clock::now();
        
        LogPerformanceMetrics(_T("UpdateDisplay"));
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void ViewdbWave_Optimized::AutoRefresh()
{
    if (m_autoRefreshEnabled)
    {
        RefreshDisplay();
    }
}

void ViewdbWave_Optimized::LoadConfiguration()
{
    try
    {
        m_configManager->LoadFromRegistry(_T("ViewdbWave"));
        ApplyConfigurationToControls();
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::CONFIGURATION_ERROR, CString(e.what()));
    }
}

void ViewdbWave_Optimized::SaveConfiguration()
{
    try
    {
        SaveControlValuesToConfiguration();
        m_configManager->SaveToRegistry(_T("ViewdbWave"));
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::CONFIGURATION_ERROR, CString(e.what()));
    }
}

void ViewdbWave_Optimized::ResetConfiguration()
{
    try
    {
        m_configManager = std::make_unique<ViewdbWaveConfiguration>();
        ApplyConfigurationToControls();
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::CONFIGURATION_ERROR, CString(e.what()));
    }
}

void ViewdbWave_Optimized::EnablePerformanceMonitoring(bool enable)
{
    if (m_performanceMonitor)
    {
        m_performanceMonitor->SetEnabled(enable);
    }
}

CString ViewdbWave_Optimized::GetPerformanceReport() const
{
    if (m_performanceMonitor)
    {
        return m_performanceMonitor->GetPerformanceReport();
    }
    return _T("Performance monitoring not available");
}

void ViewdbWave_Optimized::ResetPerformanceMetrics()
{
    if (m_performanceMonitor)
    {
        m_performanceMonitor->Reset();
    }
}

ViewState ViewdbWave_Optimized::GetCurrentState() const
{
    if (m_stateManager)
    {
        return m_stateManager->GetCurrentState();
    }
    return ViewState::UNINITIALIZED;
}

bool ViewdbWave_Optimized::IsReady() const
{
    return GetCurrentState() == ViewState::READY;
}

bool ViewdbWave_Optimized::IsProcessing() const
{
    return GetCurrentState() == ViewState::PROCESSING;
}

bool ViewdbWave_Optimized::HasError() const
{
    return GetCurrentState() == ViewState::ERROR_STATE;
}

void ViewdbWave_Optimized::HandleError(ViewdbWaveError error, const CString& message)
{
    LogError(error, message);
    DisplayErrorMessage(message);
    m_stateManager->SetState(ViewState::ERROR_STATE);
}

void ViewdbWave_Optimized::ClearError()
{
    ClearErrorMessage();
    if (GetCurrentState() == ViewState::ERROR_STATE)
    {
        m_stateManager->SetState(ViewState::READY);
    }
}

CString ViewdbWave_Optimized::GetLastErrorMessage() const
{
    // This would typically return the last error message from a member variable
    return _T("No error message available");
}

// MFC overrides

BOOL ViewdbWave_Optimized::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!ViewDbTable::PreCreateWindow(cs))
        return FALSE;
    
    cs.style |= WS_CLIPCHILDREN;
    return TRUE;
}

void ViewdbWave_Optimized::OnInitialUpdate()
{
    ViewDbTable::OnInitialUpdate();
    
    try
    {
        Initialize();
        
        // Ensure the DataListCtrl is initialized after the window is created
        EnsureDataListControlInitialized();
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::INITIALIZATION_FAILED, CString(e.what()));
    }
}

// Message map functions
void ViewdbWave_Optimized::OnSize(UINT nType, int cx, int cy)
{
    ViewDbTable::OnSize(nType, cx, cy);
    
    try
    {
        // Ensure the DataListCtrl is initialized when the window is resized
        EnsureDataListControlInitialized();
        
        // Handle window resize
        if (m_pDataListCtrl)
        {
            // Resize the data list control
            CRect rect;
            GetClientRect(&rect);
            // Adjust rect for other controls
            // m_pDataListCtrl->MoveWindow(&rect);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void ViewdbWave_Optimized::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == m_autoRefreshTimer)
    {
        AutoRefresh();
    }
    else
    {
        ViewDbTable::OnTimer(nIDEvent);
    }
}

void ViewdbWave_Optimized::OnDestroy()
{
    if (m_autoRefreshTimer)
    {
        KillTimer(m_autoRefreshTimer);
        m_autoRefreshTimer = 0;
    }
    
    ViewDbTable::OnDestroy();
}

void ViewdbWave_Optimized::OnUpdateViewRefresh(CCmdUI* pCmdUI)
{
    if (pCmdUI)
    {
        pCmdUI->Enable(IsReady());
    }
}

void ViewdbWave_Optimized::OnViewRefresh()
{
    RefreshDisplay();
}

void ViewdbWave_Optimized::OnUpdateViewAutoRefresh(CCmdUI* pCmdUI)
{
    if (pCmdUI)
    {
        pCmdUI->SetCheck(m_autoRefreshEnabled);
    }
}

void ViewdbWave_Optimized::OnViewAutoRefresh()
{
    m_autoRefreshEnabled = !m_autoRefreshEnabled;
    
    if (m_autoRefreshEnabled)
    {
        m_autoRefreshTimer = SetTimer(1, 5000, nullptr); // 5 second interval
    }
    else
    {
        if (m_autoRefreshTimer)
        {
            KillTimer(m_autoRefreshTimer);
            m_autoRefreshTimer = 0;
        }
    }
}

// Private helper methods
void ViewdbWave_Optimized::OnStateChanged(ViewState oldState, ViewState newState)
{
    try
    {
        // Handle state changes
        switch (newState)
        {
        case ViewState::READY:
            UpdateControlStates();
            break;
        case ViewState::ERROR_STATE:
            // Disable controls on error
            if (m_uiStateManager)
            {
                m_uiStateManager->SetControlState(false);
            }
            break;
        default:
            break;
        }
    }
    catch (const std::exception& e)
    {
        LogError(ViewdbWaveError::STATE_TRANSITION_FAILED, CString(e.what()));
    }
}

void ViewdbWave_Optimized::UpdateControlStates()
{
    try
    {
        bool enabled = IsReady();
        
        if (m_pTimeFirstEdit) m_pTimeFirstEdit->EnableWindow(enabled);
        if (m_pTimeLastEdit) m_pTimeLastEdit->EnableWindow(enabled);
        if (m_pAmplitudeSpanEdit) m_pAmplitudeSpanEdit->EnableWindow(enabled);
        if (m_pSpikeClassEdit) m_pSpikeClassEdit->EnableWindow(enabled);
        if (m_pDisplayDataButton) m_pDisplayDataButton->EnableWindow(enabled);
        if (m_pDisplaySpikesButton) m_pDisplaySpikesButton->EnableWindow(enabled);
        if (m_pDisplayNothingButton) m_pDisplayNothingButton->EnableWindow(enabled);
        if (m_pCheckFileNameButton) m_pCheckFileNameButton->EnableWindow(enabled);
        if (m_pFilterCheckButton) m_pFilterCheckButton->EnableWindow(enabled);
        if (m_pRadioAllClassesButton) m_pRadioAllClassesButton->EnableWindow(enabled);
        if (m_pRadioOneClassButton) m_pRadioOneClassButton->EnableWindow(enabled);
    }
    catch (const std::exception& e)
    {
        LogError(ViewdbWaveError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void ViewdbWave_Optimized::UpdateControlValues()
{
    try
    {
        if (m_pTimeFirstEdit)
        {
            CString value;
            value.Format(_T("%.3f"), m_configManager->GetTimeFirst());
            m_pTimeFirstEdit->SetWindowText(value);
        }
        
        if (m_pTimeLastEdit)
        {
            CString value;
            value.Format(_T("%.3f"), m_configManager->GetTimeLast());
            m_pTimeLastEdit->SetWindowText(value);
        }
        
        if (m_pAmplitudeSpanEdit)
        {
            CString value;
            value.Format(_T("%.3f"), m_configManager->GetAmplitudeSpan());
            m_pAmplitudeSpanEdit->SetWindowText(value);
        }
    }
    catch (const std::exception& e)
    {
        LogError(ViewdbWaveError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void ViewdbWave_Optimized::ValidateConfiguration()
{
    try
    {
        if (m_configManager->GetTimeFirst() >= m_configManager->GetTimeLast())
        {
            throw ViewdbWaveException(ViewdbWaveError::CONFIGURATION_ERROR, 
                _T("Time first must be less than time last"));
        }
        
        if (m_configManager->GetAmplitudeSpan() <= 0.0)
        {
            throw ViewdbWaveException(ViewdbWaveError::CONFIGURATION_ERROR, 
                _T("Amplitude span must be positive"));
        }
    }
    catch (const std::exception& e)
    {
        HandleError(ViewdbWaveError::CONFIGURATION_ERROR, CString(e.what()));
    }
}

void ViewdbWave_Optimized::LogPerformanceMetrics(const CString& operation)
{
    if (m_performanceMonitor && m_performanceMonitor->IsEnabled())
    {
        m_performanceMonitor->EndOperation(operation);
    }
}

// HandleAsyncOperationResult method removed - no longer needed with direct loading approach

void ViewdbWave_Optimized::LoadControlValuesFromConfiguration()
{
    try
    {
        // Load values from configuration to controls
        UpdateControlValues();
    }
    catch (const std::exception& e)
    {
        LogError(ViewdbWaveError::CONFIGURATION_ERROR, CString(e.what()));
    }
}

void ViewdbWave_Optimized::SaveControlValuesToConfiguration()
{
    try
    {
        if (m_pTimeFirstEdit)
        {
            CString value;
            m_pTimeFirstEdit->GetWindowText(value);
            m_configManager->SetTimeFirst(_ttof(value));
        }
        
        if (m_pTimeLastEdit)
        {
            CString value;
            m_pTimeLastEdit->GetWindowText(value);
            m_configManager->SetTimeLast(_ttof(value));
        }
        
        if (m_pAmplitudeSpanEdit)
        {
            CString value;
            m_pAmplitudeSpanEdit->GetWindowText(value);
            m_configManager->SetAmplitudeSpan(_ttof(value));
        }
    }
    catch (const std::exception& e)
    {
        LogError(ViewdbWaveError::CONFIGURATION_ERROR, CString(e.what()));
    }
}

void ViewdbWave_Optimized::ApplyConfigurationToControls()
{
    try
    {
        LoadControlValuesFromConfiguration();
        ValidateConfiguration();
    }
    catch (const std::exception& e)
    {
        LogError(ViewdbWaveError::CONFIGURATION_ERROR, CString(e.what()));
    }
}

void ViewdbWave_Optimized::LogError(ViewdbWaveError error, const CString& message)
{
    // Log error to debug output
    TRACE(_T("ViewdbWave_Optimized Error: %s\n"), message);
}

void ViewdbWave_Optimized::DisplayErrorMessage(const CString& message)
{
    // Display error message to user
    AfxMessageBox(message, MB_OK | MB_ICONERROR);
}

void ViewdbWave_Optimized::ClearErrorMessage()
{
    // Clear any displayed error messages
    // Implementation depends on how errors are displayed
}



