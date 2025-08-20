#include "StdAfx.h"
#include "ViewdbWave_Optimized.h"

#include "ChartWnd.h"
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
    ON_WM_DESTROY()
    ON_UPDATE_COMMAND_UI(ViewdbWaveConstants::ID_VIEW_REFRESH, OnUpdateViewRefresh)
    ON_COMMAND(ViewdbWaveConstants::ID_VIEW_REFRESH, OnViewRefresh)
    ON_UPDATE_COMMAND_UI(ViewdbWaveConstants::ID_VIEW_AUTO_REFRESH, OnUpdateViewAutoRefresh)
    ON_COMMAND(ViewdbWaveConstants::ID_VIEW_AUTO_REFRESH, OnViewAutoRefresh)

    ON_BN_CLICKED(ViewdbWaveConstants::VW_IDC_DISPLAYDATA, &ViewdbWave_Optimized::on_bn_clicked_data)
    ON_BN_CLICKED(ViewdbWaveConstants::VW_IDC_DISPLAY_SPIKES, &ViewdbWave_Optimized::on_bn_clicked_display_spikes)
    ON_BN_CLICKED(ViewdbWaveConstants::VW_IDC_DISPLAY_NOTHING, &ViewdbWave_Optimized::on_bn_clicked_display_nothing)
    ON_EN_CHANGE(ViewdbWaveConstants::VW_IDC_TIMEFIRST, &ViewdbWave_Optimized::on_en_change_time_first)
    ON_EN_CHANGE(ViewdbWaveConstants::VW_IDC_TIMELAST, &ViewdbWave_Optimized::on_en_change_time_last)
    ON_EN_CHANGE(ViewdbWaveConstants::VW_IDC_AMPLITUDESPAN, &ViewdbWave_Optimized::on_en_change_amplitude_span)
    ON_EN_CHANGE(ViewdbWaveConstants::VW_IDC_SPIKECLASS, &ViewdbWave_Optimized::on_en_change_spike_class)
    ON_BN_CLICKED(ViewdbWaveConstants::VW_IDC_CHECKFILENAME, &ViewdbWave_Optimized::on_bn_clicked_check_filename)
    ON_WM_TIMER()
    ON_NOTIFY(LVN_ITEMACTIVATE, ViewdbWaveConstants::VW_IDC_LISTCTRL, &ViewdbWave_Optimized::on_item_activate_list_ctrl)

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
    , m_initialSelectionSet(false)
    , m_initialSelectionProtectionCount(0)
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
    
    // Use DDX_Control for custom CEditCtrl objects instead of DDX_Text
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_TIMEFIRST, *m_pTimeFirstEdit);
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_TIMELAST, *m_pTimeLastEdit);
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_AMPLITUDESPAN, *m_pAmplitudeSpanEdit);
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_SPIKECLASS, *m_pSpikeClassEdit);
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_TAB1, spk_list_tab_ctrl);
    
    // Note: Button controls will be initialized manually in initialize_controls()
    // to avoid nullptr dereference issues during DoDataExchange
}

void ViewdbWave_Optimized::initialize()
{
    try
    {
        if (m_initialized)
        {
            OutputDebugString(_T("ViewdbWave_Optimized::Initialize - Already initialized\n"));
            return;
        }
        
        OutputDebugString(_T("ViewdbWave_Optimized::Initialize - Starting initialization\n"));
        
        initialize_controls();
        initialize_data_list_control();
        make_controls_stretchable();
        initialize_configuration();
        
        m_initialized = true;
        OutputDebugString(_T("ViewdbWave_Optimized::Initialize - Initialization complete\n"));
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::initialize_configuration()
{
    try
    {
        // Load configuration from registry
        m_configManager.LoadFromRegistry(_T("ViewdbWave"));
        
        // Apply configuration to controls
        apply_configuration_to_controls();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::initialize_controls()
{
    try
    {
        // Get tab control
        m_pTabCtrl = static_cast<CTabCtrl*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_TAB1));
        
        // Initialize button controls manually
        m_pDisplayDataButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_DISPLAYDATA));
        m_pDisplaySpikesButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_DISPLAY_SPIKES));
        m_pDisplayNothingButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_DISPLAY_NOTHING));
        m_pCheckFileNameButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_CHECKFILENAME));
        m_pFilterCheckButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_FILTERCHECK));
        m_pRadioAllClassesButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_RADIOALLCLASSES));
        m_pRadioOneClassButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_RADIOONECLASS));
        
        // Validate controls
        if (!m_pTimeFirstEdit || !m_pTimeLastEdit || !m_pAmplitudeSpanEdit || !m_pSpikeClassEdit)
        {
            throw std::runtime_error("Required edit controls not found");
        }
        
        // Validate button controls
        if (!m_pDisplayDataButton || !m_pDisplaySpikesButton || !m_pDisplayNothingButton ||
            !m_pCheckFileNameButton || !m_pFilterCheckButton || !m_pRadioAllClassesButton || !m_pRadioOneClassButton)
        {
            throw std::runtime_error("Required button controls not found");
        }
        
        // Set initial control states
        update_control_states();
        
        // Enable radio buttons by default
        if (m_pDisplayDataButton && m_pDisplayDataButton->GetSafeHwnd()) 
        {
            m_pDisplayDataButton->EnableWindow(TRUE);
            OutputDebugString(_T("ViewdbWave_Optimized::initialize_controls - DisplayData button enabled\n"));
        }
        if (m_pDisplaySpikesButton && m_pDisplaySpikesButton->GetSafeHwnd()) 
        {
            m_pDisplaySpikesButton->EnableWindow(TRUE);
            OutputDebugString(_T("ViewdbWave_Optimized::initialize_controls - DisplaySpikes button enabled\n"));
        }
        if (m_pDisplayNothingButton && m_pDisplayNothingButton->GetSafeHwnd()) 
        {
            m_pDisplayNothingButton->EnableWindow(TRUE);
            OutputDebugString(_T("ViewdbWave_Optimized::initialize_controls - DisplayNothing button enabled\n"));
        }
        
        // Set default display mode to "no display" (grey rectangle) and select the button
        set_display_mode(0); // DISPLAY_MODE_EMPTY
        
        // Set the default radio button selection
        if (m_pDisplayNothingButton && m_pDisplayNothingButton->GetSafeHwnd())
        {
            m_pDisplayNothingButton->SetCheck(BST_CHECKED);
            OutputDebugString(_T("ViewdbWave_Optimized::initialize_controls - DisplayNothing button set to checked\n"));
        }
        if (m_pDisplayDataButton && m_pDisplayDataButton->GetSafeHwnd())
        {
            m_pDisplayDataButton->SetCheck(BST_UNCHECKED);
        }
        if (m_pDisplaySpikesButton && m_pDisplaySpikesButton->GetSafeHwnd())
        {
            m_pDisplaySpikesButton->SetCheck(BST_UNCHECKED);
        }
        
        OutputDebugString(_T("ViewdbWave_Optimized::initialize_controls - Controls initialized successfully\n"));
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::initialize_data_list_control()
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

        // Set the parent window so the DataListCtrl can access the document
        m_pDataListCtrl->set_parent_window(this);
        
        m_pDataListCtrl->initialize(*m_pConfiguration);
        
        // Load saved column widths after initialization
        m_pDataListCtrl->load_column_widths();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
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
            CString docPtrMsg;
        docPtrMsg.Format(_T("ViewdbWave_Optimized::SetDocument - Document pointer: %p\n"), pDoc);
        OutputDebugString(docPtrMsg);
    
    // Store the document reference
    m_pDocument = pDoc;
    
    // Load data immediately if initialized (simplified approach for local database)
    if (pDoc && m_initialized)
    {
        OutputDebugString(_T("ViewdbWave_Optimized::SetDocument - Document set and initialized, calling LoadData\n"));
        load_data();
    }
    else
    {
        OutputDebugString(_T("ViewdbWave_Optimized::SetDocument - Document set but not initialized yet\n"));
                CString docInitMsg;
        docInitMsg.Format(_T("ViewdbWave_Optimized::SetDocument - Document: %p, Initialized: %s\n"),
              pDoc, m_initialized ? _T("true") : _T("false"));
        OutputDebugString(docInitMsg);
    }
}

void ViewdbWave_Optimized::SetApplication(CdbWaveApp* pApp)
{
    m_pApplication = pApp;  // Simple assignment for raw pointer
}

void ViewdbWave_Optimized::load_data()
{
    try
    {
        OutputDebugString(_T("ViewdbWave_Optimized::LoadData - Starting\n"));
        
        CdbWaveDoc* pDoc = GetDocument();
        if (!pDoc)
        {
            OutputDebugString(_T("ViewdbWave_Optimized::LoadData - No document available\n"));
            return;
        }
        
        m_processing = true;
        OutputDebugString(_T("ViewdbWave_Optimized::LoadData - Document available, loading data\n"));
        
        // Load data directly
        load_data_from_document(pDoc);
        
        // Refresh the display after data loading is complete
        if (m_pDataListCtrl && m_pDataListCtrl->GetSafeHwnd())
        {
            OutputDebugString(_T("ViewdbWave_Optimized::LoadData - Refreshing display\n"));
            m_pDataListCtrl->refresh_display();
        }
        else
        {
            OutputDebugString(_T("ViewdbWave_Optimized::LoadData - DataListCtrl not available for refresh\n"));
        }
        
        m_processing = false;
        OutputDebugString(_T("ViewdbWave_Optimized::LoadData - Complete\n"));
    }
    catch (const std::exception& e)
    {
        CString loadExceptionMsg;
        loadExceptionMsg.Format(_T("ViewdbWave_Optimized::LoadData - Exception: %s\n"), CString(e.what()));
        OutputDebugString(loadExceptionMsg);
        m_processing = false;
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::load_data_from_document(CdbWaveDoc* pDoc)
{
    try
    {
        OutputDebugString(_T("ViewdbWave_Optimized::LoadDataFromDocument - Starting\n"));
        
        if (pDoc && m_pDataListCtrl)
        {
            // Clear existing items first (like the original fill_list_box)
            m_pDataListCtrl->DeleteAllItems();
            
            // Get the number of records from the database
            const int n_records = pDoc->db_get_records_count();
            CString recordCountMsg;
        recordCountMsg.Format(_T("ViewdbWave_Optimized::LoadDataFromDocument - Record count: %d\n"), n_records);
        OutputDebugString(recordCountMsg);
            
            // Set the item count for the virtual list control (like SetItemCountEx in original)
            m_pDataListCtrl->SetItemCount(n_records);
            CString itemCountMsg;
            itemCountMsg.Format(_T("ViewdbWave_Optimized::LoadDataFromDocument - Set item count to: %d\n"), n_records);
            OutputDebugString(itemCountMsg);
            
            // Also set row count for the optimized version
            m_pDataListCtrl->set_row_count(n_records);
            CString rowCountMsg;
            rowCountMsg.Format(_T("ViewdbWave_Optimized::LoadDataFromDocument - Set row count to: %d\n"), n_records);
            OutputDebugString(rowCountMsg);
            
            // Force a refresh to trigger the display
            m_pDataListCtrl->Invalidate();
            OutputDebugString(_T("ViewdbWave_Optimized::LoadDataFromDocument - Forced invalidate\n"));
            
            // Update the cache to ensure data is available for display
            if (m_pDataListCtrl->GetSafeHwnd())
            {
                m_pDataListCtrl->refresh_display();
            }
            
            // Update controls to reflect the current database state
            update_controls();
        }
        else
        {
            OutputDebugString(_T("ViewdbWave_Optimized::LoadDataFromDocument - No document or data list control\n"));
        }
    }
    catch (const std::exception& e)
    {
        CString loadDocExceptionMsg;
        loadDocExceptionMsg.Format(_T("ViewdbWave_Optimized::LoadDataFromDocument - Exception: %s\n"), CString(e.what()));
        OutputDebugString(loadDocExceptionMsg);
        throw;
    }
}

void ViewdbWave_Optimized::refresh_display()
{
    try
    {
        if (!m_initialized)
        {
            OutputDebugString(_T("ViewdbWave_Optimized::RefreshDisplay - Not initialized\n"));
            return;
        }
        
        update_display();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::update_display()
{
    try
    {
        // Update control values from configuration
        update_control_values();
        
        // Update the view
        Invalidate();
        UpdateWindow();
        
        // Refresh the data list control if available
        if (m_pDataListCtrl != nullptr && m_pDataListCtrl->GetSafeHwnd())
        {
            m_pDataListCtrl->refresh_display();
        }
        
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::auto_refresh()
{
    if (m_autoRefreshEnabled)
    {
        refresh_display();
    }
}

void ViewdbWave_Optimized::load_configuration()
{
    try
    {
        m_configManager.LoadFromRegistry(_T("ViewdbWave"));
        apply_configuration_to_controls();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::save_configuration()
{
    try
    {
        save_control_values_to_configuration();
        m_configManager.SaveToRegistry(_T("ViewdbWave"));
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::reset_configuration()
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
        
        apply_configuration_to_controls();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
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
    OutputDebugString(_T("=== DEBUG: OnInitialUpdate START ===\n"));
    
    ViewDbTable::OnInitialUpdate();
    
    try
    {
        OutputDebugString(_T("=== DEBUG: OnInitialUpdate - After ViewDbTable::OnInitialUpdate() ===\n"));
        
        initialize();
        
        OutputDebugString(_T("=== DEBUG: OnInitialUpdate - After initialize() ===\n"));
        
        // Load data if document is already available (simplified approach)
        CdbWaveDoc* pDoc = GetDocument();
        CString docMsg;
        docMsg.Format(_T("=== DEBUG: OnInitialUpdate - GetDocument() returned: %p ===\n"), pDoc);
        OutputDebugString(docMsg);
        
        if (pDoc && m_initialized)
        {
            OutputDebugString(_T("=== DEBUG: OnInitialUpdate - Document available and initialized ===\n"));
            
            // Get the current record position BEFORE loading data
            const int current_record_position_before = pDoc->db_get_current_record_position();
            CString beforeMsg;
            beforeMsg.Format(_T("=== DEBUG: OnInitialUpdate - Current record position BEFORE load_data: %d ===\n"), current_record_position_before);
            OutputDebugString(beforeMsg);
            
            load_data();
            
            OutputDebugString(_T("=== DEBUG: OnInitialUpdate - After load_data() ===\n"));
            
            // Get the current record position AFTER loading data
            const int current_record_position_after = pDoc->db_get_current_record_position();
            CString afterMsg;
            afterMsg.Format(_T("=== DEBUG: OnInitialUpdate - Current record position AFTER load_data: %d ===\n"), current_record_position_after);
            OutputDebugString(afterMsg);
            
            if (m_pDataListCtrl && m_pDataListCtrl->GetSafeHwnd())
            {
                OutputDebugString(_T("=== DEBUG: OnInitialUpdate - DataListCtrl is valid ===\n"));
                CString countMsg;
                countMsg.Format(_T("=== DEBUG: OnInitialUpdate - DataListCtrl item count: %d ===\n"), m_pDataListCtrl->GetItemCount());
                OutputDebugString(countMsg);
                
                // Ensure the data list control is properly initialized
                if (m_pDataListCtrl->GetItemCount() > 0)
                {
                    CString selectionMsg;
                    selectionMsg.Format(_T("=== DEBUG: OnInitialUpdate - Setting selection to record: %d ===\n"), current_record_position_after);
                    OutputDebugString(selectionMsg);
                    
                    // Set the current selection
                    m_pDataListCtrl->set_current_selection(current_record_position_after);
                    
                    OutputDebugString(_T("=== DEBUG: OnInitialUpdate - After set_current_selection() ===\n"));
                    
                    // Ensure the item is visible and centered in the viewport
                    m_pDataListCtrl->center_item_in_viewport(current_record_position_after);
                    
                    OutputDebugString(_T("=== DEBUG: OnInitialUpdate - After center_item_in_viewport() ===\n"));
                    
                    // Force a redraw to show the selection
                    m_pDataListCtrl->RedrawItems(current_record_position_after, current_record_position_after);
                    m_pDataListCtrl->UpdateWindow();
                    
                    OutputDebugString(_T("=== DEBUG: OnInitialUpdate - After RedrawItems() and UpdateWindow() ===\n"));
                    
                    // Force a refresh of the image list to ensure rectangles are displayed
                    m_pDataListCtrl->refresh_display();
                    
                    OutputDebugString(_T("=== DEBUG: OnInitialUpdate - After refresh_display() ===\n"));
                    
                    // Mark that initial selection has been set and protect it for a few calls
                    m_initialSelectionSet = true;
                    m_initialSelectionProtectionCount = 5; // Protect for 5 update_controls calls
                    
                    CString focusMsg;
                    focusMsg.Format(_T("=== DEBUG: OnInitialUpdate - Set focus to record: %d ===\n"), current_record_position_after);
                    OutputDebugString(focusMsg);
                }
                else
                {
                    OutputDebugString(_T("=== DEBUG: OnInitialUpdate - Data list control has no items yet ===\n"));
                }
            }
            else
            {
                OutputDebugString(_T("=== DEBUG: OnInitialUpdate - Data list control not available or no window handle ===\n"));
                CString ctrlMsg;
                ctrlMsg.Format(_T("=== DEBUG: OnInitialUpdate - m_pDataListCtrl: %p ===\n"), m_pDataListCtrl.get());
                OutputDebugString(ctrlMsg);
                if (m_pDataListCtrl) {
                    CString handleMsg;
                    handleMsg.Format(_T("=== DEBUG: OnInitialUpdate - DataListCtrl handle: %p ===\n"), m_pDataListCtrl->GetSafeHwnd());
                    OutputDebugString(handleMsg);
                }
            }
        }
        else
        {
            OutputDebugString(_T("=== DEBUG: OnInitialUpdate - No document or not initialized ===\n"));
            CString initMsg;
            initMsg.Format(_T("=== DEBUG: OnInitialUpdate - GetDocument() returned: %p, Initialized: %s ===\n"), 
                  pDoc, m_initialized ? _T("true") : _T("false"));
            OutputDebugString(initMsg);
        }
        
        OutputDebugString(_T("=== DEBUG: OnInitialUpdate END ===\n"));
    }
    catch (const std::exception& e)
    {
        CString exceptionMsg;
        exceptionMsg.Format(_T("=== DEBUG: OnInitialUpdate - Exception: %s ===\n"), CString(e.what()));
        OutputDebugString(exceptionMsg);
        handle_error(CString(e.what()));
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
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::OnDestroy()
{
    try
    {
        // Save column widths before destroying
        if (m_pDataListCtrl)
        {
            m_pDataListCtrl->save_column_widths();
        }
        
        // Save configuration
        save_configuration();
        
        if (m_autoRefreshTimer)
        {
            KillTimer(m_autoRefreshTimer);
            m_autoRefreshTimer = 0;
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
    
    ViewDbTable::OnDestroy();
}

void ViewdbWave_Optimized::OnUpdateViewRefresh(CCmdUI* pCmdUI)
{
    if (pCmdUI)
    {
        pCmdUI->Enable(is_ready());
    }
}

void ViewdbWave_Optimized::OnViewRefresh()
{
    refresh_display();
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

void ViewdbWave_Optimized::on_item_activate_list_ctrl(NMHDR* pNMHDR, LRESULT* pResult)
{
    try
    {
        // Get item clicked and select it
        const auto p_item_activate = reinterpret_cast<NMITEMACTIVATE*>(pNMHDR);
        if (p_item_activate->iItem >= 0)
        {
            CdbWaveDoc* pDoc = GetDocument();
            if (pDoc)
            {
                pDoc->db_set_current_record_position(p_item_activate->iItem);
                
                // Update the selection in the list control
                if (m_pDataListCtrl)
                {
                    m_pDataListCtrl->set_current_selection(p_item_activate->iItem);
                }
                
                // Notify other views about the record change
                pDoc->UpdateAllViews(nullptr, HINT_DOC_MOVE_RECORD, nullptr);
            }
        }
        
        *pResult = 0;
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
        *pResult = 1;
    }
}

void ViewdbWave_Optimized::OnTimer(UINT nIDEvent)
{
    try
    {
        if (nIDEvent == 1) // Auto refresh timer only
        {
            if (m_autoRefreshEnabled)
            {
                auto_refresh();
            }
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
    
    ViewDbTable::OnTimer(nIDEvent);
}

void ViewdbWave_Optimized::OnUpdate(CView* p_sender, const LPARAM l_hint, CObject* p_hint)
{
    CString hintMsg;
    hintMsg.Format(_T("ViewdbWave_Optimized::OnUpdate - Hint: 0x%08X\n"), l_hint);
    OutputDebugString(hintMsg);
    
    if (!m_initialized)
    {
        OutputDebugString(_T("ViewdbWave_Optimized::OnUpdate - Not initialized, returning\n"));
        return;
    }

    switch (LOWORD(l_hint))
    {
    case HINT_REQUERY:
        OutputDebugString(_T("ViewdbWave_Optimized::OnUpdate - HINT_REQUERY received\n"));
        load_data();
        break;
        
    case HINT_DOC_HAS_CHANGED:
        OutputDebugString(_T("ViewdbWave_Optimized::OnUpdate - HINT_DOC_HAS_CHANGED received\n"));
        load_data();
        break;
        
    case HINT_DOC_MOVE_RECORD:
        OutputDebugString(_T("ViewdbWave_Optimized::OnUpdate - HINT_DOC_MOVE_RECORD received\n"));
        update_controls();
        break;
        
    case HINT_REPLACE_VIEW:
        OutputDebugString(_T("ViewdbWave_Optimized::OnUpdate - HINT_REPLACE_VIEW received\n"));
        load_data();
        break;
        
    default:
        OutputDebugString(_T("ViewdbWave_Optimized::OnUpdate - Default case, updating display\n"));
        update_display();
        break;
    }
}

// OnStateChanged method removed - simplified implementation

void ViewdbWave_Optimized::update_control_states()
{
    try
    {
        bool enabled = is_ready();
        
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
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::update_controls()
{
    OutputDebugString(_T("=== DEBUG: update_controls() START ===\n"));
    
    try
    {
        const auto db_wave_doc = GetDocument();
        if (!db_wave_doc)
        {
            OutputDebugString(_T("=== DEBUG: update_controls() - No document available ===\n"));
            return;
        }

        // Get the current record position from the database
        const int current_record_position = db_wave_doc->db_get_current_record_position();
        CString posMsg;
        posMsg.Format(_T("=== DEBUG: update_controls() - Current record position: %d ===\n"), current_record_position);
        OutputDebugString(posMsg);

        // Update the selection in the list control to reflect the database state
        if (m_pDataListCtrl && m_pDataListCtrl->GetSafeHwnd())
        {
            OutputDebugString(_T("=== DEBUG: update_controls() - DataListCtrl is valid ===\n"));
            
            // Only update if the current selection is different to avoid unnecessary updates
            int currentSelection = m_pDataListCtrl->get_current_selection();
            CString selMsg;
            selMsg.Format(_T("=== DEBUG: update_controls() - Current selection in DataListCtrl: %d ===\n"), currentSelection);
            OutputDebugString(selMsg);
            
            if (currentSelection != current_record_position)
            {
                OutputDebugString(_T("=== DEBUG: update_controls() - Selection differs, checking if we should update ===\n"));
                
                // Additional check: don't update if we're in the middle of initialization
                // This prevents update_controls from overriding the initial selection
                if (m_initialized && m_pDataListCtrl->GetItemCount() > 0 && currentSelection != -1 && m_initialSelectionSet)
                {
                    // Check if we're still in the protection period
                    if (m_initialSelectionProtectionCount > 0)
                    {
                        m_initialSelectionProtectionCount--;
                        CString protectionMsg;
                        protectionMsg.Format(_T("=== DEBUG: update_controls() - Skipping update during protection period (count: %d, currentSelection: %d, dbPosition: %d) ===\n"), 
                              m_initialSelectionProtectionCount, currentSelection, current_record_position);
                        OutputDebugString(protectionMsg);
                    }
                    else
                    {
                        CString updateMsg;
                        updateMsg.Format(_T("=== DEBUG: update_controls() - Updating selection from %d to %d ===\n"), currentSelection, current_record_position);
                        OutputDebugString(updateMsg);
                        
                        m_pDataListCtrl->set_current_selection(current_record_position);
                        m_pDataListCtrl->EnsureVisible(current_record_position, FALSE);
                        CString updatedMsg;
                        updatedMsg.Format(_T("=== DEBUG: update_controls() - Updated list control selection to position: %d ===\n"), current_record_position);
                        OutputDebugString(updatedMsg);
                    }
                }
                else
                {
                    CString skipMsg;
                    skipMsg.Format(_T("=== DEBUG: update_controls() - Skipping update during initialization (initialized: %s, itemCount: %d, currentSelection: %d, initialSelectionSet: %s) ===\n"), 
                          m_initialized ? _T("true") : _T("false"), m_pDataListCtrl->GetItemCount(), currentSelection, m_initialSelectionSet ? _T("true") : _T("false"));
                    OutputDebugString(skipMsg);
                }
            }
            else
            {
                CString correctMsg;
                correctMsg.Format(_T("=== DEBUG: update_controls() - Selection already correct at position: %d ===\n"), current_record_position);
                OutputDebugString(correctMsg);
            }
        }
        else
        {
            OutputDebugString(_T("=== DEBUG: update_controls() - No data list control available ===\n"));
        }

        // Update other controls as needed (similar to original update_controls)
        // This could include updating file status, spike list, etc.
        
        OutputDebugString(_T("=== DEBUG: update_controls() END ===\n"));
    }
    catch (const std::exception& e)
    {
        CString exceptionMsg;
        exceptionMsg.Format(_T("=== DEBUG: update_controls() - Exception: %s ===\n"), CString(e.what()));
        OutputDebugString(exceptionMsg);
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::update_control_values()
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
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::validate_configuration()
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
        handle_error(CString(e.what()));
    }
}

// LogPerformanceMetrics method removed - simplified implementation

void ViewdbWave_Optimized::load_control_values_from_configuration()
{
    try
    {
        // Load values from configuration to controls
        update_control_values();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::save_control_values_to_configuration()
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
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::apply_configuration_to_controls()
{
    try
    {
        load_control_values_from_configuration();
        validate_configuration();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

// Complex error handling methods removed - simplified implementation

CdbWaveDoc* ViewdbWave_Optimized::GetDocument() const
{
    // Use MFC framework to get the document
    return static_cast<CdbWaveDoc*>(CDaoRecordView::GetDocument());
}

// Display mode methods - removed duplicate implementation

void ViewdbWave_Optimized::display_data()
{
    try
    {
        OutputDebugString(_T("ViewdbWave_Optimized::DisplayData - Setting display data mode\n"));
        
        // Validate button pointers before using them
        if (m_pDisplayDataButton && m_pDisplayDataButton->GetSafeHwnd())
        {
            m_pDisplayDataButton->SetCheck(BST_CHECKED);
            OutputDebugString(_T("ViewdbWave_Optimized::DisplayData - Set DisplayData button to checked\n"));
        }
        else
        {
            OutputDebugString(_T("ViewdbWave_Optimized::DisplayData - DisplayData button not available\n"));
        }
        
        if (m_pDisplaySpikesButton && m_pDisplaySpikesButton->GetSafeHwnd())
        {
            m_pDisplaySpikesButton->SetCheck(BST_UNCHECKED);
        }
        if (m_pDisplayNothingButton && m_pDisplayNothingButton->GetSafeHwnd())
        {
            m_pDisplayNothingButton->SetCheck(BST_UNCHECKED);
        }
        
        // Enable/disable related controls
        if (m_pRadioAllClassesButton && m_pRadioAllClassesButton->GetSafeHwnd())
        {
            m_pRadioAllClassesButton->EnableWindow(FALSE);
        }
        if (m_pRadioOneClassButton && m_pRadioOneClassButton->GetSafeHwnd())
        {
            m_pRadioOneClassButton->EnableWindow(FALSE);
        }
        if (m_pSpikeClassEdit && m_pSpikeClassEdit->GetSafeHwnd())
        {
            m_pSpikeClassEdit->EnableWindow(FALSE);
        }
        if (m_pFilterCheckButton && m_pFilterCheckButton->GetSafeHwnd())
        {
            m_pFilterCheckButton->EnableWindow(TRUE);
        }
        
        // Set filter check state
        if (m_pFilterCheckButton && m_pFilterCheckButton->GetSafeHwnd())
        {
            m_pFilterCheckButton->SetCheck(m_configManager.GetFilterData() ? BST_CHECKED : BST_UNCHECKED);
        }
        
        // Update data list control
        if (m_pDataListCtrl)
        {
            m_pDataListCtrl->set_display_mode(1); // DISPLAY_MODE_DATA
            m_pDataListCtrl->refresh_display(); // Force refresh to show new display mode
            OutputDebugString(_T("ViewdbWave_Optimized::DisplayData - Data list control display mode set to 1\n"));
        }
        else
        {
            OutputDebugString(_T("ViewdbWave_Optimized::DisplayData - Data list control not available\n"));
        }
        
        // Hide spike tab control if it exists
        if (m_pTabCtrl && m_pTabCtrl->GetSafeHwnd())
        {
            m_pTabCtrl->ShowWindow(SW_HIDE);
        }
        
        OutputDebugString(_T("ViewdbWave_Optimized::DisplayData - Display data mode set successfully\n"));
    }
    catch (const std::exception& e)
    {
        CString displayDataExceptionMsg;
        displayDataExceptionMsg.Format(_T("ViewdbWave_Optimized::DisplayData - Exception: %s\n"), CString(e.what()));
        OutputDebugString(displayDataExceptionMsg);
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::display_spikes()
{
    try
    {
        OutputDebugString(_T("ViewdbWave_Optimized::DisplaySpikes - Setting display spikes mode\n"));
        
        // Validate button pointers before using them
        if (m_pDisplaySpikesButton && m_pDisplaySpikesButton->GetSafeHwnd())
        {
            m_pDisplaySpikesButton->SetCheck(BST_CHECKED);
            OutputDebugString(_T("ViewdbWave_Optimized::DisplaySpikes - Set DisplaySpikes button to checked\n"));
        }
        else
        {
            OutputDebugString(_T("ViewdbWave_Optimized::DisplaySpikes - DisplaySpikes button not available\n"));
        }
        
        if (m_pDisplayDataButton && m_pDisplayDataButton->GetSafeHwnd())
        {
            m_pDisplayDataButton->SetCheck(BST_UNCHECKED);
        }
        if (m_pDisplayNothingButton && m_pDisplayNothingButton->GetSafeHwnd())
        {
            m_pDisplayNothingButton->SetCheck(BST_UNCHECKED);
        }
        
        // Enable/disable related controls
        if (m_pFilterCheckButton && m_pFilterCheckButton->GetSafeHwnd())
        {
            m_pFilterCheckButton->EnableWindow(FALSE);
        }
        if (m_pRadioAllClassesButton && m_pRadioAllClassesButton->GetSafeHwnd())
        {
            m_pRadioAllClassesButton->EnableWindow(TRUE);
        }
        if (m_pRadioOneClassButton && m_pRadioOneClassButton->GetSafeHwnd())
        {
            m_pRadioOneClassButton->EnableWindow(TRUE);
        }
        if (m_pSpikeClassEdit && m_pSpikeClassEdit->GetSafeHwnd())
        {
            m_pSpikeClassEdit->EnableWindow(TRUE);
        }
        
        // Set radio button states based on configuration
        if (m_configManager.GetDisplayAllClasses())
        {
            if (m_pRadioAllClassesButton && m_pRadioAllClassesButton->GetSafeHwnd())
            {
                m_pRadioAllClassesButton->SetCheck(BST_CHECKED);
            }
            if (m_pRadioOneClassButton && m_pRadioOneClassButton->GetSafeHwnd())
            {
                m_pRadioOneClassButton->SetCheck(BST_UNCHECKED);
            }
        }
        else
        {
            if (m_pRadioAllClassesButton && m_pRadioAllClassesButton->GetSafeHwnd())
            {
                m_pRadioAllClassesButton->SetCheck(BST_UNCHECKED);
            }
            if (m_pRadioOneClassButton && m_pRadioOneClassButton->GetSafeHwnd())
            {
                m_pRadioOneClassButton->SetCheck(BST_CHECKED);
            }
        }
        
        // Update data list control
        if (m_pDataListCtrl)
        {
            m_pDataListCtrl->set_display_mode(2); // DISPLAY_MODE_SPIKE
            m_pDataListCtrl->refresh_display(); // Force refresh to show new display mode
            OutputDebugString(_T("ViewdbWave_Optimized::DisplaySpikes - Data list control display mode set to 2\n"));
        }
        else
        {
            OutputDebugString(_T("ViewdbWave_Optimized::DisplaySpikes - Data list control not available\n"));
        }
        
        // Show spike tab control if it exists
        if (m_pTabCtrl && m_pTabCtrl->GetSafeHwnd())
        {
            m_pTabCtrl->ShowWindow(SW_SHOW);
        }
        
        OutputDebugString(_T("ViewdbWave_Optimized::DisplaySpikes - Display spikes mode set successfully\n"));
    }
    catch (const std::exception& e)
    {
        CString displaySpikesExceptionMsg;
        displaySpikesExceptionMsg.Format(_T("ViewdbWave_Optimized::DisplaySpikes - Exception: %s\n"), CString(e.what()));
        OutputDebugString(displaySpikesExceptionMsg);
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::display_nothing()
{
    try
    {
        OutputDebugString(_T("ViewdbWave_Optimized::DisplayNothing - Setting display nothing mode\n"));
        
        // Validate button pointers before using them
        if (m_pDisplayNothingButton && m_pDisplayNothingButton->GetSafeHwnd())
        {
            m_pDisplayNothingButton->SetCheck(BST_CHECKED);
            OutputDebugString(_T("ViewdbWave_Optimized::DisplayNothing - Set DisplayNothing button to checked\n"));
        }
        else
        {
            OutputDebugString(_T("ViewdbWave_Optimized::DisplayNothing - DisplayNothing button not available\n"));
        }
        
        if (m_pDisplayDataButton && m_pDisplayDataButton->GetSafeHwnd())
        {
            m_pDisplayDataButton->SetCheck(BST_UNCHECKED);
        }
        if (m_pDisplaySpikesButton && m_pDisplaySpikesButton->GetSafeHwnd())
        {
            m_pDisplaySpikesButton->SetCheck(BST_UNCHECKED);
        }
        
        // Disable related controls
        if (m_pFilterCheckButton && m_pFilterCheckButton->GetSafeHwnd())
        {
            m_pFilterCheckButton->EnableWindow(FALSE);
        }
        if (m_pRadioAllClassesButton && m_pRadioAllClassesButton->GetSafeHwnd())
        {
            m_pRadioAllClassesButton->EnableWindow(FALSE);
        }
        if (m_pRadioOneClassButton && m_pRadioOneClassButton->GetSafeHwnd())
        {
            m_pRadioOneClassButton->EnableWindow(FALSE);
        }
        if (m_pSpikeClassEdit && m_pSpikeClassEdit->GetSafeHwnd())
        {
            m_pSpikeClassEdit->EnableWindow(FALSE);
        }
        
        // Update data list control
        if (m_pDataListCtrl)
        {
            m_pDataListCtrl->set_display_mode(0); // DISPLAY_MODE_EMPTY
            m_pDataListCtrl->refresh_display(); // Force refresh to show new display mode
            OutputDebugString(_T("ViewdbWave_Optimized::DisplayNothing - Data list control display mode set to 0\n"));
        }
        else
        {
            OutputDebugString(_T("ViewdbWave_Optimized::DisplayNothing - Data list control not available\n"));
        }
        
        // Hide spike tab control if it exists
        if (m_pTabCtrl && m_pTabCtrl->GetSafeHwnd())
        {
            m_pTabCtrl->ShowWindow(SW_HIDE);
        }
        
        OutputDebugString(_T("ViewdbWave_Optimized::DisplayNothing - Display nothing mode set successfully\n"));
    }
    catch (const std::exception& e)
    {
        CString displayNothingExceptionMsg;
        displayNothingExceptionMsg.Format(_T("ViewdbWave_Optimized::DisplayNothing - Exception: %s\n"), CString(e.what()));
        OutputDebugString(displayNothingExceptionMsg);
        handle_error(CString(e.what()));
    }
}

// Simplified error handling methods
void ViewdbWave_Optimized::handle_error(const CString& message)
{
    m_lastError = message;
            CString errorMsg;
        errorMsg.Format(_T("ViewdbWave_Optimized Error: %s\n"), static_cast<LPCTSTR>(message));
        OutputDebugString(errorMsg);
}

void ViewdbWave_Optimized::clear_error()
{
    m_lastError.Empty();
}

void ViewdbWave_Optimized::set_display_mode(int mode)
{
    try
    {
        CString modeMsg;
        modeMsg.Format(_T("ViewdbWave_Optimized::set_display_mode - Setting mode %d\n"), mode);
        OutputDebugString(modeMsg);
        
        switch (mode)
        {
        case 0: // DISPLAY_MODE_EMPTY
            display_nothing();
            break;
        case 1: // DISPLAY_MODE_DATA
            display_data();
            break;
        case 2: // DISPLAY_MODE_SPIKE
            display_spikes();
            break;
        default:
            CString invalidModeMsg;
        invalidModeMsg.Format(_T("ViewdbWave_Optimized::set_display_mode - Invalid mode %d, defaulting to empty\n"), mode);
        OutputDebugString(invalidModeMsg);
            display_nothing();
            break;
        }
        
        if (m_pDataListCtrl)
        {
            m_pDataListCtrl->refresh_display();
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

int ViewdbWave_Optimized::get_display_mode() const
{
    try
    {
        if (m_pDataListCtrl)
        {
            return m_pDataListCtrl->get_display_mode();
        }
        return 0; // Default to empty mode
    }
    catch (const std::exception& e)
    {
        // Return default mode if there's an error
        CString exceptionMsg;
        exceptionMsg.Format(_T("exception %s\n"), CString(e.what()));
        OutputDebugString(exceptionMsg);
        return 0;
    }
}

void ViewdbWave_Optimized::on_bn_clicked_data()
{
    display_data();
    m_pDataListCtrl->refresh_display();
}

void ViewdbWave_Optimized::on_bn_clicked_display_spikes()
{
    display_spikes();
    m_pDataListCtrl->refresh_display();

    // update tab control
    const int n_rows = m_pDataListCtrl->get_row_count();
    if (n_rows > 0)
    {
        // TODO
       /* const auto p_spk_doc = m_pDataListCtrl->get_visible_rows_spike_doc_at(0);
        if (p_spk_doc != nullptr && p_spk_doc->get_spike_list_size() > 1)
        {
            spk_list_tab_ctrl.init_ctrl_tab_from_spike_doc(p_spk_doc);
            spk_list_tab_ctrl.ShowWindow(SW_SHOW);
            spk_list_tab_ctrl.SetCurSel(p_spk_doc->get_index_current_spike_list());
            spk_list_tab_ctrl.Invalidate();
        }*/
    }
    m_pDataListCtrl->refresh_display();
}

void ViewdbWave_Optimized::on_bn_clicked_display_nothing()
{
    display_nothing();
    m_pDataListCtrl->refresh_display();
}

void ViewdbWave_Optimized::on_en_change_time_first()
{
    if (!m_pTimeFirstEdit->m_b_entry_done)
        return;
    TimeSettings& time_settings = m_pConfiguration->get_time_settings();
	float time_first = time_settings.get_time_first();
    m_pTimeFirstEdit->on_en_change(this, time_first, 1.f, -1.f);

    if (time_first > time_settings.get_time_last())
        time_first = 0.f;
    time_settings.set_time_first(time_first);

    m_pDataListCtrl->set_time_intervals(time_settings.get_time_first(), time_settings.get_time_last());
    m_pDataListCtrl->refresh_display();
}

void ViewdbWave_Optimized::on_en_change_time_last()
{
    if (!m_pTimeLastEdit->m_b_entry_done)
        return;
    auto& time_settings = m_pConfiguration->get_time_settings();
    float time_last = time_settings.get_time_last();
    m_pTimeLastEdit->on_en_change(this, time_last, 1.f, -1.f);
    time_settings.set_time_last(time_last);
    m_pDataListCtrl->set_time_intervals(time_settings.get_time_first(), time_settings.get_time_last());
    m_pDataListCtrl->refresh_display();
}

void ViewdbWave_Optimized::on_en_change_amplitude_span()
{
    if (!m_pAmplitudeSpanEdit->m_b_entry_done)
        return;
    auto& amplitude_settings = m_pConfiguration->get_amplitude_settings();
    float amplitude_span = amplitude_settings.get_mv_span();
    m_pAmplitudeSpanEdit->on_en_change(this, amplitude_span, 1.f, -1.f);
    UpdateData(FALSE);
    amplitude_settings.set_mv_span(amplitude_span);
    m_pDataListCtrl->set_amplitude_span(amplitude_span);
    m_pDataListCtrl->refresh_display();
}

void ViewdbWave_Optimized::on_bn_clicked_check_filename()
{
	boolean checked = (IsDlgButtonChecked(IDC_CHECKFILENAME) == BST_CHECKED);
    m_pConfiguration->get_ui_settings().set_display_file_name (checked);
    m_pDataListCtrl->set_display_file_name(checked);
    m_pDataListCtrl->refresh_display();
}

void ViewdbWave_Optimized::on_en_change_spike_class()
{
    if (!m_pSpikeClassEdit->m_b_entry_done)
        return;
    auto& display_settings = m_pConfiguration->get_display_settings();
	int selected_class = display_settings.get_selected_class();
    m_pSpikeClassEdit->on_en_change(this, selected_class, 1, -1);
    display_settings.set_selected_class(selected_class);
    UpdateData(FALSE);
    m_pDataListCtrl->set_spike_plot_mode(selected_class);
    m_pDataListCtrl->refresh_display();
}

