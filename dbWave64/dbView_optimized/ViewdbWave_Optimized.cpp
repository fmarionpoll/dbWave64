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
    ON_WM_DESTROY()
    ON_UPDATE_COMMAND_UI(ViewdbWaveConstants::ID_VIEW_REFRESH, OnUpdateViewRefresh)
    ON_COMMAND(ViewdbWaveConstants::ID_VIEW_REFRESH, OnViewRefresh)
    ON_UPDATE_COMMAND_UI(ViewdbWaveConstants::ID_VIEW_AUTO_REFRESH, OnUpdateViewAutoRefresh)
    ON_COMMAND(ViewdbWaveConstants::ID_VIEW_AUTO_REFRESH, OnViewAutoRefresh)
    ON_COMMAND(ID_RECORD_PAGE_UP, &ViewdbWave_Optimized::on_record_page_up)
    ON_COMMAND(ID_RECORD_PAGE_DOWN, &ViewdbWave_Optimized::on_record_page_down)

    ON_BN_CLICKED(ViewdbWaveConstants::VW_IDC_DISPLAYDATA, &ViewdbWave_Optimized::on_bn_clicked_data)
    ON_BN_CLICKED(ViewdbWaveConstants::VW_IDC_DISPLAY_SPIKES, &ViewdbWave_Optimized::on_bn_clicked_display_spikes)
    ON_BN_CLICKED(ViewdbWaveConstants::VW_IDC_DISPLAY_NOTHING, &ViewdbWave_Optimized::on_bn_clicked_display_nothing)
    ON_EN_CHANGE(ViewdbWaveConstants::VW_IDC_TIMEFIRST, &ViewdbWave_Optimized::on_en_change_time_first)
    ON_EN_CHANGE(ViewdbWaveConstants::VW_IDC_TIMELAST, &ViewdbWave_Optimized::on_en_change_time_last)
    ON_EN_CHANGE(ViewdbWaveConstants::VW_IDC_AMPLITUDESPAN, &ViewdbWave_Optimized::on_en_change_amplitude_span)
    ON_EN_CHANGE(ViewdbWaveConstants::VW_IDC_SPIKECLASS, &ViewdbWave_Optimized::on_en_change_spike_class)
    ON_BN_CLICKED(ViewdbWaveConstants::VW_IDC_CHECKFILENAME, &ViewdbWave_Optimized::on_bn_clicked_check_filename)
    ON_BN_CLICKED(ViewdbWaveConstants::VW_IDC_FILTERCHECK, &ViewdbWave_Optimized::on_click_median_filter)

    ON_WM_TIMER()
    ON_NOTIFY(LVN_ITEMACTIVATE, ViewdbWaveConstants::VW_IDC_LISTCTRL, &ViewdbWave_Optimized::on_item_activate_list_ctrl)
    ON_NOTIFY(NM_DBLCLK, ViewdbWaveConstants::VW_IDC_LISTCTRL, &ViewdbWave_Optimized::on_dbl_clk_list_ctrl)

END_MESSAGE_MAP()

ViewdbWave_Optimized::ViewdbWave_Optimized()
    : ViewDbTable(IDD)
    , m_pDocument(nullptr)
    , m_pApplication(nullptr)
    , m_pDataListCtrl(std::make_unique<DataListCtrl_Optimized>())
    , m_pConfiguration(std::make_unique<::data_list_ctrl_configuration>())
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
            return;
        }

        initialize_controls();
        initialize_data_list_control();
        make_controls_stretchable();
        initialize_configuration();
        
        m_initialized = true;
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
        m_configManager.LoadFromRegistry(_T("ViewdbWave"));
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
        }
        if (m_pDisplaySpikesButton && m_pDisplaySpikesButton->GetSafeHwnd()) 
        {
            m_pDisplaySpikesButton->EnableWindow(TRUE);
        }
        if (m_pDisplayNothingButton && m_pDisplayNothingButton->GetSafeHwnd()) 
        {
            m_pDisplayNothingButton->EnableWindow(TRUE);
        }
        
        // Set default display mode to "no display" (grey rectangle) and select the button
        set_display_mode(0); // DISPLAY_MODE_EMPTY
        
        // Set the default radio button selection
        if (m_pDisplayNothingButton && m_pDisplayNothingButton->GetSafeHwnd())
        {
            m_pDisplayNothingButton->SetCheck(BST_CHECKED);
        }
        if (m_pDisplayDataButton && m_pDisplayDataButton->GetSafeHwnd())
        {
            m_pDisplayDataButton->SetCheck(BST_UNCHECKED);
        }
        if (m_pDisplaySpikesButton && m_pDisplaySpikesButton->GetSafeHwnd())
        {
            m_pDisplaySpikesButton->SetCheck(BST_UNCHECKED);
        }

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
    m_pDocument = pDoc;
    // Load data immediately if initialized (simplified approach for local database)
    if (pDoc && m_initialized)
    {
        load_data();
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
        CdbWaveDoc* pDoc = GetDocument();
        if (!pDoc)
        {
            return;
        }
        m_processing = true;        
        load_data_from_document(pDoc);
        
        // Refresh the display after data loading is complete
        if (m_pDataListCtrl && m_pDataListCtrl->GetSafeHwnd())
        {
            m_pDataListCtrl->refresh_display();
        }
        m_processing = false;
    }
    catch (const std::exception& e)
    {
        m_processing = false;
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::load_data_from_document(CdbWaveDoc* pDoc)
{
    try
    {
        if (pDoc && m_pDataListCtrl)
        {

            m_pDataListCtrl->DeleteAllItems();
            const int n_records = pDoc->db_get_records_count();
            // Set the item count for the virtual list control (like SetItemCountEx in original)
            m_pDataListCtrl->SetItemCount(n_records); // TODO: useful?
            m_pDataListCtrl->set_row_count(n_records); 
            
            // Force a refresh to trigger the display
            m_pDataListCtrl->Invalidate();

            // Update the cache to ensure data is available for display
            if (m_pDataListCtrl->GetSafeHwnd())
            {
                m_pDataListCtrl->refresh_display();
            }

            update_controls();
        }
    }
    catch (const std::exception& e)
    {
        //throw;
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::refresh_display()
{
    try
    {
        if (!m_initialized)
        {
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
        update_control_values();
        Invalidate();
        UpdateWindow();
        if (m_pDataListCtrl != nullptr && m_pDataListCtrl->GetSafeHwnd())
            m_pDataListCtrl->refresh_display();
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
        m_configManager.SetDisplayMode(DataListCtrl_ConfigConstants::DISPLAY_MODE_EMPTY);
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
    ViewDbTable::OnInitialUpdate();
    
    try
    {
		initialize();
        
        // Load data if document is already available 
        CdbWaveDoc* pDoc = GetDocument();
        if (pDoc && m_initialized)
        {

            const int current_record_position_before = pDoc->db_get_current_record_position();
			load_data();
            const int current_record_position_after = pDoc->db_get_current_record_position();
            
            if (m_pDataListCtrl && m_pDataListCtrl->GetSafeHwnd())
            {
               // Ensure the data list control is properly initialized
                if (m_pDataListCtrl->GetItemCount() > 0)
                {
                    m_pDataListCtrl->set_current_selection(current_record_position_after);
                    m_pDataListCtrl->center_item_in_viewport(current_record_position_after);
                    m_pDataListCtrl->RedrawItems(current_record_position_after, current_record_position_after);
                    m_pDataListCtrl->UpdateWindow();
                    m_pDataListCtrl->refresh_display();
                    
                    // Mark that initial selection has been set and protect it for a few calls
                    m_initialSelectionSet = true;
                    m_initialSelectionProtectionCount = 5; // Protect for 5 update_controls calls
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

// Message map functions
void ViewdbWave_Optimized::OnSize(UINT nType, int cx, int cy)
{
    ViewDbTable::OnSize(nType, cx, cy);
    try
    {
        if (m_pDataListCtrl && IsWindow(m_pDataListCtrl->m_hWnd))
        {
            CRect rect;
            m_pDataListCtrl->GetClientRect(&rect);
            m_pDataListCtrl->fit_columns_to_size(rect.Width());
        }
    }
    catch (const std::exception& e) {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::OnDestroy()
{
    try
    {
        if (m_pDataListCtrl)
            m_pDataListCtrl->save_column_widths();

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
    if (!m_initialized)
    {
        return;
    }

    switch (LOWORD(l_hint))
    {
    case HINT_REQUERY:
        load_data();
        break;
        
    case HINT_DOC_HAS_CHANGED:
        load_data();
        break;
        
    case HINT_DOC_MOVE_RECORD:
        update_controls();
        break;
        
    case HINT_REPLACE_VIEW:
        load_data();
        break;
        
    default:
       update_display();
        break;
    }
}

// OnStateChanged method removed - simplified implementation

void ViewdbWave_Optimized::update_control_states()
{
    try
    {
        const bool enabled = is_ready();
        
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
    try
    {
        const auto db_wave_doc = GetDocument();
        if (!db_wave_doc)
        {
            return;
        }

        const int current_record_position = db_wave_doc->db_get_current_record_position();
        // Update the selection in the list control to reflect the database state
        if (m_pDataListCtrl && m_pDataListCtrl->GetSafeHwnd())
        {
            // Only update if the current selection is different to avoid unnecessary updates
            int currentSelection = m_pDataListCtrl->get_current_selection();
            if (currentSelection != current_record_position)
            {
                // Additional check: don't update if we're in the middle of initialization
                // This prevents update_controls from overriding the initial selection
                if (m_initialized && m_pDataListCtrl->GetItemCount() > 0 && currentSelection != -1 && m_initialSelectionSet)
                {
                    // Check if we're still in the protection period
                    if (m_initialSelectionProtectionCount > 0)
                    {
                        m_initialSelectionProtectionCount--;
                    }
                    else
                    {
                       m_pDataListCtrl->set_current_selection(current_record_position);
                        m_pDataListCtrl->EnsureVisible(current_record_position, FALSE);
                    }
                }
            }
        }
        

        // Update other controls as needed (similar to original update_controls)
        // This could include updating file status, spike list, etc.

    }
    catch (const std::exception& e)
    {
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
    return static_cast<CdbWaveDoc*>(CDaoRecordView::GetDocument());
}

// Display mode methods - removed duplicate implementation

void ViewdbWave_Optimized::display_data()
{
    try
    {
		// Validate button pointers before using them
        if (m_pDisplayDataButton && m_pDisplayDataButton->GetSafeHwnd())
        {
            m_pDisplayDataButton->SetCheck(BST_CHECKED);
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
        }
        
        // Hide spike tab control if it exists
        if (m_pTabCtrl && m_pTabCtrl->GetSafeHwnd())
        {
            m_pTabCtrl->ShowWindow(SW_HIDE);
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::display_spikes()
{
    try
    {
        // Validate button pointers before using them
        if (m_pDisplaySpikesButton && m_pDisplaySpikesButton->GetSafeHwnd())
        {
            m_pDisplaySpikesButton->SetCheck(BST_CHECKED);
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
        }
        
        // Show spike tab control if it exists
        if (m_pTabCtrl && m_pTabCtrl->GetSafeHwnd())
        {
            m_pTabCtrl->ShowWindow(SW_SHOW);
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void ViewdbWave_Optimized::display_nothing()
{
    try
    {
        // Validate button pointers before using them
        if (m_pDisplayNothingButton && m_pDisplayNothingButton->GetSafeHwnd())
        {
            m_pDisplayNothingButton->SetCheck(BST_CHECKED);
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
        }
        
        // Hide spike tab control if it exists
        if (m_pTabCtrl && m_pTabCtrl->GetSafeHwnd())
        {
            m_pTabCtrl->ShowWindow(SW_HIDE);
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

// Simplified error handling methods
void ViewdbWave_Optimized::handle_error(const CString& message)
{
    m_lastError = message;
    CString errorMsg;
    errorMsg.Format(_T("ViewdbWave_Optimized Error: %s\n"), static_cast<LPCTSTR>(message));
    TRACE(errorMsg);
}

void ViewdbWave_Optimized::clear_error()
{
    m_lastError.Empty();
}

void ViewdbWave_Optimized::set_display_mode(int mode)
{
    try
    {
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

int ViewdbWave_Optimized::get_display_mode() 
{
    try
    {
        if (m_pDataListCtrl)
        {
            return m_pDataListCtrl->get_display_mode();
        }
        return 0; 
    }
    catch (const std::exception& e)
    {
        handle_error( CString(e.what()));
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
    time_settings& time_settings = m_pConfiguration->get_time_settings();
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

void ViewdbWave_Optimized::on_record_page_up()
{
    m_pDataListCtrl->SendMessage(WM_VSCROLL, SB_PAGEUP, NULL);
}

void ViewdbWave_Optimized::on_record_page_down()
{
    m_pDataListCtrl->SendMessage(WM_VSCROLL, SB_PAGEDOWN, NULL);
}

void ViewdbWave_Optimized::on_dbl_clk_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result)
{
    *p_result = 0;
    // quit the current view and switch to spike detection view
    GetParent()->PostMessage(WM_COMMAND, static_cast<WPARAM>(ID_VIEW_SPIKE_DETECTION), static_cast<LPARAM>(NULL));
}

void ViewdbWave_Optimized::on_click_median_filter()
{
    auto& display_settings = m_pConfiguration->get_display_settings();
	boolean flag = display_settings.get_data_transform() > 0 ? true : false;
    if (flag == static_cast<CButton*>(GetDlgItem(IDC_FILTERCHECK))->GetCheck())
        return;

    flag = static_cast<CButton*>(GetDlgItem(IDC_FILTERCHECK))->GetCheck();
    m_pDataListCtrl->set_data_transform(flag? 13:0);
    m_pDataListCtrl->refresh_display();
}


