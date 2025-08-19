#include "StdAfx.h"
#include "ViewdbWave_Optimized.h"
#include "ViewdbWave_SupportingClasses.h"
#include "DataListCtrl_Optimized.h"
#include "DataListCtrl_Configuration.h"
#include "dbWave_constants.h"

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
    , m_processing(false)
    , m_autoRefreshEnabled(false)
    , m_autoRefreshTimer(0)
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
        if (m_initialized)
        {
            TRACE(_T("ViewdbWave_Optimized::Initialize - Already initialized\n"));
            return;
        }
        
        TRACE(_T("ViewdbWave_Optimized::Initialize - Starting initialization\n"));
        
        InitializeControls();
        InitializeDataListControl();
        make_controls_stretchable();
        InitializeConfiguration();
        
        m_initialized = true;
        TRACE(_T("ViewdbWave_Optimized::Initialize - Initialization complete\n"));
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::InitializeConfiguration()
{
    try
    {
        // Load configuration from registry
        m_configManager.LoadFromRegistry(_T("ViewdbWave"));
        
        // Apply configuration to controls
        ApplyConfigurationToControls();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::InitializeControls()
{
    try
    {
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
            throw std::runtime_error("Required controls not found");
        }
        
        // Set initial control states
        UpdateControlStates();
        
        // Set default display mode to "no display" (grey rectangle)
        SetDisplayMode(DataListCtrlConfigConstants::DISPLAY_MODE_EMPTY);
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::InitializeDataListControl()
{
    try
    {
        if (!m_pDataListCtrl)
        {
            throw std::runtime_error("Data list control not initialized");
        }

        // Check if the control has a valid HWND before initializing
        if (!m_pDataListCtrl->GetSafeHwnd())
        {
            // Control not ready yet, this is expected during early initialization
            // The control will be initialized later when the HWND is available
            return;
        }

        m_pDataListCtrl->Initialize(*m_pConfiguration);
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

// EnsureDataListControlInitialized method removed - simplified implementation

void ViewdbWave_Optimized::make_controls_stretchable()
{
    stretch_.attach_parent(this);
    stretch_.new_prop(IDC_LISTCTRL, XLEQ_XREQ, YTEQ_YBEQ);
    stretch_.new_prop(IDC_TAB1, XLEQ_XREQ, SZEQ_YBEQ);
    b_init_ = TRUE;
}

void ViewdbWave_Optimized::SetDocument(CdbWaveDoc* pDoc)
{
    TRACE(_T("ViewdbWave_Optimized::SetDocument - Document pointer: %p\n"), pDoc);
    
    // Note: This method is kept for compatibility but the document is now managed by MFC framework
    // The actual document is obtained through GetDocument() which uses CDaoRecordView::GetDocument()
    
    if (pDoc && m_initialized)
    {
        TRACE(_T("ViewdbWave_Optimized::SetDocument - Document set and initialized, calling LoadData\n"));
        LoadData();
    }
    else
    {
        TRACE(_T("ViewdbWave_Optimized::SetDocument - Document set but not initialized yet\n"));
        TRACE(_T("ViewdbWave_Optimized::SetDocument - Document: %p, Initialized: %s\n"), 
              pDoc, m_initialized ? _T("true") : _T("false"));
    }
}

void ViewdbWave_Optimized::SetApplication(CdbWaveApp* pApp)
{
    m_pApplication = pApp;  // Simple assignment for raw pointer
}

void ViewdbWave_Optimized::LoadData()
{
    try
    {
        TRACE(_T("ViewdbWave_Optimized::LoadData - Starting\n"));
        
        CdbWaveDoc* pDoc = GetDocument();
        if (!pDoc)
        {
            TRACE(_T("ViewdbWave_Optimized::LoadData - No document available\n"));
            return;
        }
        
        m_processing = true;
        TRACE(_T("ViewdbWave_Optimized::LoadData - Document available, loading data\n"));
        
        // Load data directly
        LoadDataFromDocument(pDoc);
        
        // Refresh the display after data loading is complete
        if (m_pDataListCtrl && m_pDataListCtrl->GetSafeHwnd())
        {
            TRACE(_T("ViewdbWave_Optimized::LoadData - Refreshing display\n"));
            m_pDataListCtrl->RefreshDisplay();
        }
        else
        {
            TRACE(_T("ViewdbWave_Optimized::LoadData - DataListCtrl not available for refresh\n"));
        }
        
        m_processing = false;
        TRACE(_T("ViewdbWave_Optimized::LoadData - Complete\n"));
    }
    catch (const std::exception& e)
    {
        TRACE(_T("ViewdbWave_Optimized::LoadData - Exception: %s\n"), CString(e.what()));
        m_processing = false;
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::LoadDataFromDocument(CdbWaveDoc* pDoc)
{
    try
    {
        TRACE(_T("ViewdbWave_Optimized::LoadDataFromDocument - Starting\n"));
        
        if (pDoc && m_pDataListCtrl)
        {
            // Get the number of records from the database
            const int n_records = pDoc->db_get_records_count();
            TRACE(_T("ViewdbWave_Optimized::LoadDataFromDocument - Record count: %d\n"), n_records);
            
            // Set the item count for the virtual list control
            m_pDataListCtrl->SetItemCount(n_records);
            TRACE(_T("ViewdbWave_Optimized::LoadDataFromDocument - Set item count to: %d\n"), n_records);
            
            // Also try setting row count as backup
            m_pDataListCtrl->SetRowCount(n_records);
            TRACE(_T("ViewdbWave_Optimized::LoadDataFromDocument - Set row count to: %d\n"), n_records);
            
            // Force a refresh to trigger the display
            m_pDataListCtrl->Invalidate();
            TRACE(_T("ViewdbWave_Optimized::LoadDataFromDocument - Forced invalidate\n"));
        }
        else
        {
            TRACE(_T("ViewdbWave_Optimized::LoadDataFromDocument - No document or data list control\n"));
        }
    }
    catch (const std::exception& e)
    {
        TRACE(_T("ViewdbWave_Optimized::LoadDataFromDocument - Exception: %s\n"), CString(e.what()));
        throw;
    }
}

void ViewdbWave_Optimized::RefreshDisplay()
{
    try
    {
        if (!m_initialized)
        {
            TRACE(_T("ViewdbWave_Optimized::RefreshDisplay - Not initialized\n"));
            return;
        }
        
        UpdateDisplay();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::UpdateDisplay()
{
    try
    {
        // Update control values from configuration
        UpdateControlValues();
        
        // Update the view
        Invalidate();
        UpdateWindow();
        
        // Refresh the data list control if available
        if (m_pDataListCtrl != nullptr && m_pDataListCtrl->GetSafeHwnd())
        {
            m_pDataListCtrl->RefreshDisplay();
        }
        
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
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
        m_configManager.LoadFromRegistry(_T("ViewdbWave"));
        ApplyConfigurationToControls();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::SaveConfiguration()
{
    try
    {
        SaveControlValuesToConfiguration();
        m_configManager.SaveToRegistry(_T("ViewdbWave"));
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::ResetConfiguration()
{
    try
    {
        // Reset configuration to default values instead of creating new object
        m_configManager.SetTimeFirst(0.0);
        m_configManager.SetTimeLast(100.0);
        m_configManager.SetAmplitudeSpan(1.0);
        m_configManager.SetDisplayFileName(false);
        m_configManager.SetFilterEnabled(false);
        m_configManager.SetDisplayMode(DataListCtrlConfigConstants::DISPLAY_MODE_EMPTY);
        m_configManager.SetDisplayAllClasses(true);
        
        ApplyConfigurationToControls();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

// Performance monitoring and complex state management methods removed - simplified implementation

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
        TRACE(_T("ViewdbWave_Optimized::OnInitialUpdate - Starting\n"));
        
        Initialize();
        
        // Load data if document is already available
        CdbWaveDoc* pDoc = GetDocument();
        if (pDoc && m_initialized)
        {
            TRACE(_T("ViewdbWave_Optimized::OnInitialUpdate - Document available, loading data\n"));
            LoadData();
        }
        else
        {
            TRACE(_T("ViewdbWave_Optimized::OnInitialUpdate - No document or not initialized\n"));
            TRACE(_T("ViewdbWave_Optimized::OnInitialUpdate - GetDocument() returned: %p, Initialized: %s\n"), 
                  pDoc, m_initialized ? _T("true") : _T("false"));
        }
        
        TRACE(_T("ViewdbWave_Optimized::OnInitialUpdate - Complete\n"));
        
        // Set a timer to check for document availability after a short delay
        SetTimer(999, 100, nullptr); // 100ms delay
    }
    catch (const std::exception& e)
    {
        TRACE(_T("ViewdbWave_Optimized::OnInitialUpdate - Exception: %s\n"), CString(e.what()));
        HandleError(CString(e.what()));
    }
}

// Message map functions
void ViewdbWave_Optimized::OnSize(UINT nType, int cx, int cy)
{
    ViewDbTable::OnSize(nType, cx, cy);
    
    try
    {
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
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == m_autoRefreshTimer)
    {
        AutoRefresh();
    }
    else if (nIDEvent == 999)
    {
        // Check for document availability
        TRACE(_T("ViewdbWave_Optimized::OnTimer - Checking for document availability\n"));
        KillTimer(999);
        
        CdbWaveDoc* pDoc = GetDocument();
        if (pDoc && m_initialized)
        {
            TRACE(_T("ViewdbWave_Optimized::OnTimer - Document now available, loading data\n"));
            LoadData();
        }
        else
        {
            TRACE(_T("ViewdbWave_Optimized::OnTimer - Document still not available\n"));
            TRACE(_T("ViewdbWave_Optimized::OnTimer - GetDocument(): %p, Initialized: %s\n"), 
                  pDoc, m_initialized ? _T("true") : _T("false"));
        }
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

void ViewdbWave_Optimized::OnUpdate(CView* p_sender, const LPARAM l_hint, CObject* p_hint)
{
    TRACE(_T("ViewdbWave_Optimized::OnUpdate - Hint: 0x%08X\n"), l_hint);
    
    if (!m_initialized)
    {
        TRACE(_T("ViewdbWave_Optimized::OnUpdate - Not initialized, returning\n"));
        return;
    }

    switch (LOWORD(l_hint))
    {
    case HINT_REQUERY:
        TRACE(_T("ViewdbWave_Optimized::OnUpdate - HINT_REQUERY received\n"));
        LoadData();
        break;
        
    case HINT_DOC_HAS_CHANGED:
        TRACE(_T("ViewdbWave_Optimized::OnUpdate - HINT_DOC_HAS_CHANGED received\n"));
        LoadData();
        break;
        
    case HINT_DOC_MOVE_RECORD:
        TRACE(_T("ViewdbWave_Optimized::OnUpdate - HINT_DOC_MOVE_RECORD received\n"));
        UpdateDisplay();
        break;
        
    case HINT_REPLACE_VIEW:
        TRACE(_T("ViewdbWave_Optimized::OnUpdate - HINT_REPLACE_VIEW received\n"));
        LoadData();
        break;
        
    default:
        TRACE(_T("ViewdbWave_Optimized::OnUpdate - Default case, updating display\n"));
        UpdateDisplay();
        break;
    }
}

// OnStateChanged method removed - simplified implementation

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
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::UpdateControlValues()
{
    try
    {
        if (m_pTimeFirstEdit)
        {
            CString value;
            value.Format(_T("%.3f"), m_configManager.GetTimeFirst());
            m_pTimeFirstEdit->SetWindowText(value);
        }
        
        if (m_pTimeLastEdit)
        {
            CString value;
            value.Format(_T("%.3f"), m_configManager.GetTimeLast());
            m_pTimeLastEdit->SetWindowText(value);
        }
        
        if (m_pAmplitudeSpanEdit)
        {
            CString value;
            value.Format(_T("%.3f"), m_configManager.GetAmplitudeSpan());
            m_pAmplitudeSpanEdit->SetWindowText(value);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::ValidateConfiguration()
{
    try
    {
        if (m_configManager.GetTimeFirst() >= m_configManager.GetTimeLast())
        {
            throw std::runtime_error("Time first must be less than time last");
        }
        
        if (m_configManager.GetAmplitudeSpan() <= 0.0)
        {
            throw std::runtime_error("Amplitude span must be positive");
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

// LogPerformanceMetrics method removed - simplified implementation

void ViewdbWave_Optimized::LoadControlValuesFromConfiguration()
{
    try
    {
        // Load values from configuration to controls
        UpdateControlValues();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
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
            m_configManager.SetTimeFirst(_ttof(value));
        }
        
        if (m_pTimeLastEdit)
        {
            CString value;
            m_pTimeLastEdit->GetWindowText(value);
            m_configManager.SetTimeLast(_ttof(value));
        }
        
        if (m_pAmplitudeSpanEdit)
        {
            CString value;
            m_pAmplitudeSpanEdit->GetWindowText(value);
            m_configManager.SetAmplitudeSpan(_ttof(value));
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
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
        HandleError(CString(e.what()));
    }
}

// Complex error handling methods removed - simplified implementation

CdbWaveDoc* ViewdbWave_Optimized::GetDocument() const
{
    // Use MFC framework to get the document
    return static_cast<CdbWaveDoc*>(CDaoRecordView::GetDocument());
}

// Display mode methods
void ViewdbWave_Optimized::SetDisplayMode(int mode)
{
    try
    {
        TRACE(_T("ViewdbWave_Optimized::SetDisplayMode - Setting mode: %d\n"), mode);
        
        // Update the configuration
        m_configManager.SetDisplayMode(mode);
        
        // Update the data list control
        if (m_pDataListCtrl)
        {
            m_pDataListCtrl->SetDisplayMode(mode);
        }
        
        // Update button states based on mode
        switch (mode)
        {
        case DataListCtrlConfigConstants::DISPLAY_MODE_DATA:
            DisplayData();
            break;
        case DataListCtrlConfigConstants::DISPLAY_MODE_SPIKE:
            DisplaySpikes();
            break;
        case DataListCtrlConfigConstants::DISPLAY_MODE_EMPTY:
        default:
            DisplayNothing();
            break;
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::DisplayData()
{
    try
    {
        TRACE(_T("ViewdbWave_Optimized::DisplayData - Setting display data mode\n"));
        
        // Set button states
        if (m_pDisplayDataButton) m_pDisplayDataButton->SetCheck(BST_CHECKED);
        if (m_pDisplaySpikesButton) m_pDisplaySpikesButton->SetCheck(BST_UNCHECKED);
        if (m_pDisplayNothingButton) m_pDisplayNothingButton->SetCheck(BST_UNCHECKED);
        
        // Enable/disable related controls
        if (m_pRadioAllClassesButton) m_pRadioAllClassesButton->EnableWindow(FALSE);
        if (m_pRadioOneClassButton) m_pRadioOneClassButton->EnableWindow(FALSE);
        if (m_pSpikeClassEdit) m_pSpikeClassEdit->EnableWindow(FALSE);
        if (m_pFilterCheckButton) m_pFilterCheckButton->EnableWindow(TRUE);
        
        // Set filter check state
        if (m_pFilterCheckButton)
        {
            m_pFilterCheckButton->SetCheck(m_configManager.GetFilterData() ? BST_CHECKED : BST_UNCHECKED);
        }
        
        // Update data list control
        if (m_pDataListCtrl)
        {
            m_pDataListCtrl->SetDisplayMode(DataListCtrlConfigConstants::DISPLAY_MODE_DATA);
        }
        
        // Hide spike tab control if it exists
        if (m_pTabCtrl)
        {
            m_pTabCtrl->ShowWindow(SW_HIDE);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::DisplaySpikes()
{
    try
    {
        TRACE(_T("ViewdbWave_Optimized::DisplaySpikes - Setting display spikes mode\n"));
        
        // Set button states
        if (m_pDisplaySpikesButton) m_pDisplaySpikesButton->SetCheck(BST_CHECKED);
        if (m_pDisplayDataButton) m_pDisplayDataButton->SetCheck(BST_UNCHECKED);
        if (m_pDisplayNothingButton) m_pDisplayNothingButton->SetCheck(BST_UNCHECKED);
        
        // Enable/disable related controls
        if (m_pFilterCheckButton) m_pFilterCheckButton->EnableWindow(FALSE);
        if (m_pRadioAllClassesButton) m_pRadioAllClassesButton->EnableWindow(TRUE);
        if (m_pRadioOneClassButton) m_pRadioOneClassButton->EnableWindow(TRUE);
        if (m_pSpikeClassEdit) m_pSpikeClassEdit->EnableWindow(TRUE);
        
        // Set radio button states based on configuration
        if (m_configManager.GetDisplayAllClasses())
        {
            if (m_pRadioAllClassesButton) m_pRadioAllClassesButton->SetCheck(BST_CHECKED);
            if (m_pRadioOneClassButton) m_pRadioOneClassButton->SetCheck(BST_UNCHECKED);
        }
        else
        {
            if (m_pRadioAllClassesButton) m_pRadioAllClassesButton->SetCheck(BST_UNCHECKED);
            if (m_pRadioOneClassButton) m_pRadioOneClassButton->SetCheck(BST_CHECKED);
        }
        
        // Update data list control
        if (m_pDataListCtrl)
        {
            m_pDataListCtrl->SetDisplayMode(DataListCtrlConfigConstants::DISPLAY_MODE_SPIKE);
        }
        
        // Show spike tab control if it exists
        if (m_pTabCtrl)
        {
            m_pTabCtrl->ShowWindow(SW_SHOW);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void ViewdbWave_Optimized::DisplayNothing()
{
    try
    {
        TRACE(_T("ViewdbWave_Optimized::DisplayNothing - Setting display nothing mode\n"));
        
        // Set button states
        if (m_pDisplayNothingButton) m_pDisplayNothingButton->SetCheck(BST_CHECKED);
        if (m_pDisplayDataButton) m_pDisplayDataButton->SetCheck(BST_UNCHECKED);
        if (m_pDisplaySpikesButton) m_pDisplaySpikesButton->SetCheck(BST_UNCHECKED);
        
        // Disable related controls
        if (m_pFilterCheckButton) m_pFilterCheckButton->EnableWindow(FALSE);
        if (m_pRadioAllClassesButton) m_pRadioAllClassesButton->EnableWindow(FALSE);
        if (m_pRadioOneClassButton) m_pRadioOneClassButton->EnableWindow(FALSE);
        if (m_pSpikeClassEdit) m_pSpikeClassEdit->EnableWindow(FALSE);
        
        // Update data list control
        if (m_pDataListCtrl)
        {
            m_pDataListCtrl->SetDisplayMode(DataListCtrlConfigConstants::DISPLAY_MODE_EMPTY);
        }
        
        // Hide spike tab control if it exists
        if (m_pTabCtrl)
        {
            m_pTabCtrl->ShowWindow(SW_HIDE);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

// Simplified error handling methods
void ViewdbWave_Optimized::HandleError(const CString& message)
{
    m_lastError = message;
    TRACE(_T("ViewdbWave_Optimized Error: %s\n"), message);
}

void ViewdbWave_Optimized::ClearError()
{
    m_lastError.Empty();
}
