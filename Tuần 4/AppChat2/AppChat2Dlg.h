
// AppChat2Dlg.h : header file
//

#pragma once

#include <map>
#include <vector>

#define WM_APPCHAT_TCP_RECEIVED (WM_APP + 1)
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

	SOCKET m_serverSocket = INVALID_SOCKET;
	HANDLE m_receiveThread = nullptr;
	static DWORD WINAPI ReceiveThread(LPVOID param);
	afx_msg LRESULT OnTcpReceived(WPARAM wParam, LPARAM lParam);

	SOCKET m_udpSocket = INVALID_SOCKET;
	HANDLE m_udpThread = nullptr;
	static DWORD WINAPI UdpReceiveThread(LPVOID param);

	BOOL SendViaTcp(const CString& data);
	BOOL SendViaUdp(const CString& data);

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
};
