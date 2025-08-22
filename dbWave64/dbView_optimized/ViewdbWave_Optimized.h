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
class data_list_ctrl_configuration;
class CdbTableMain;
class CSpikeDoc;

// Main ViewdbWave_Optimized class - Simplified
class ViewdbWave_Optimized : public ViewDbTable
{
    DECLARE_DYNCREATE(ViewdbWave_Optimized)

    enum { IDD = IDD_VIEWDBWAVE };

public:
    ViewdbWave_Optimized();
    ~ViewdbWave_Optimized() override;

    // Initialization and setup
    void initialize();
    void initialize_configuration();
    void initialize_controls();
    void initialize_data_list_control();
    void make_controls_stretchable();

    // Data operations
    void load_data();
    void load_data_from_document(CdbWaveDoc* pDoc);
    void refresh_display();
    void update_display();
    void update_controls();
    void auto_refresh();
    
    // Configuration management
    void load_configuration();
    void save_configuration();
    void reset_configuration();
    
    // Simple state management
    bool is_ready() const { return m_initialized && m_pDocument != nullptr; }
    bool is_processing() const { return m_processing; }
    bool has_error() const { return !m_lastError.IsEmpty(); }
    
    // Error handling
    void handle_error(const CString& message);
    void clear_error();
    CString GetLastErrorMessage() const { return m_lastError; }

protected:
    // MFC overrides
    BOOL PreCreateWindow(CREATESTRUCT& cs) override;
    void OnInitialUpdate() override;
    void DoDataExchange(CDataExchange* pDX) override;
    void OnUpdate(CView* p_sender, const LPARAM l_hint, CObject* p_hint) override;
    void OnActivateView(BOOL b_activate, CView* p_activate_view, CView* p_deactive_view) override;

private:
    // Core components
    std::unique_ptr<DataListCtrl_Optimized> m_pDataListCtrl;
    std::unique_ptr<::data_list_ctrl_configuration> m_pConfiguration;
    
    // Simplified configuration management
    ViewdbWaveConfiguration m_configManager;
    
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
    
    // Simple state variables
    bool m_initialized;
    bool m_processing;
    bool m_autoRefreshEnabled;
    UINT_PTR m_autoRefreshTimer;
    CString m_lastError;
    bool m_initialSelectionSet;  // Flag to prevent update_controls from overriding initial selection
    int m_initialSelectionProtectionCount;  // Counter to protect initial selection for a few calls
    
    // Private helper methods
    void update_control_states();
    void update_control_values();
    void validate_configuration();
    
    // Configuration helpers
    void load_control_values_from_configuration();
    void save_control_values_to_configuration();
    void apply_configuration_to_controls();
    
    // Display mode methods
    void display_data();
    void display_spikes();
    void display_nothing();

    // Display mode management
    void set_display_mode(int mode);
    int get_display_mode();

protected:
    DECLARE_MESSAGE_MAP()

    // Message map functions
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnUpdateViewRefresh(CCmdUI* pCmdUI);
    afx_msg void OnViewRefresh();
    afx_msg void OnUpdateViewAutoRefresh(CCmdUI* pCmdUI);
    afx_msg void OnViewAutoRefresh();

    afx_msg void on_item_activate_list_ctrl(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void on_record_page_up();
    afx_msg void on_record_page_down();
    afx_msg void on_click_median_filter();
    afx_msg void on_bn_clicked_data();
    afx_msg void on_bn_clicked_display_spikes();
    afx_msg void on_bn_clicked_display_nothing();
    afx_msg void on_bn_clicked_check_filename();
    afx_msg void on_dbl_clk_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result);

    afx_msg void on_en_change_time_first();
    afx_msg void on_en_change_time_last();
    afx_msg void on_en_change_amplitude_span();
    afx_msg void on_en_change_spike_class();
};
