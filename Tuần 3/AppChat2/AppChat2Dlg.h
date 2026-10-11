
// AppChat2Dlg.h : header file
//

#pragma once

#include <map>
#include <vector>

// CAppChat2Dlg dialog
class CAppChat2Dlg : public CDialogEx
{
// Construction
public:
	CAppChat2Dlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APPCHAT2_DIALOG };
#endif

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
	CTreeCtrl m_treeUsers;
	CListCtrl m_listMessages;
	CEdit m_editMessage;
	afx_msg void OnTvnSelchangedTreeUsers(NMHDR* pNMHDR, LRESULT* pResult);

	CString m_selectedUser;
	afx_msg void OnBnClickedBtnSend();

	struct ChatMessage
	{
		CString time;
		CString sender;
		CString content;
	};

	std::map<CString, std::vector<ChatMessage>> m_chatHistory;

	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

	BOOL SendViaWindowsMessage(const CString& data);
	BOOL SendViaNamedPipe(const CString& data);

	static DWORD WINAPI PipeServerThread(LPVOID pParam);
	afx_msg LRESULT OnPipeMessage(WPARAM wParam, LPARAM lParam);
};
