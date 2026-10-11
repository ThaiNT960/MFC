
// AppChat1Dlg.h : header file
//

#pragma once

#include <map>
#include <vector>

#define WM_APPCHAT_TCP_RECEIVED (WM_APP + 1)
// CAppChat1Dlg dialog
class CAppChat1Dlg : public CDialogEx
{
// Construction
public:
	CAppChat1Dlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APPCHAT1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	SOCKET m_listenSocket = INVALID_SOCKET;
	SOCKET m_clientSocket = INVALID_SOCKET;
	HANDLE m_acceptThread = nullptr;
	static DWORD WINAPI AcceptThread(LPVOID param);

	SOCKET m_udpSocket = INVALID_SOCKET;
	HANDLE m_udpThread = nullptr;
	sockaddr_in m_udpPeerAddress = {};
	BOOL m_hasUdpPeer = FALSE;
	static DWORD WINAPI UdpReceiveThread(LPVOID param);
	
	BOOL SendViaTcp(const CString& data);
	BOOL SendViaUdp(const CString& data);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTcpReceived(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl m_listMessages;
	CTreeCtrl m_treeUsers;
	CEdit m_editMessage;

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
