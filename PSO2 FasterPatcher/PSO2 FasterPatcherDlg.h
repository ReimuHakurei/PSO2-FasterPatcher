
// PSO2 FasterPatcherDlg.h : header file
//

#pragma once


// CPSO2FasterPatcherDlg dialog
class CPSO2FasterPatcherDlg : public CDialogEx
{
// Construction
public:
	CPSO2FasterPatcherDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PSO2FASTERPATCHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPatch();
};
