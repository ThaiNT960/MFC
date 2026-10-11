#pragma once
#include <vector>


struct ChatMessage
{
	CString time;
	CString sender;
	CString content;
};

struct ChatData
{
	CString username;
	std::vector<ChatMessage> messages;
};
// CChatDlg dialog

class CChatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChatDlg)

public:
	CChatDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CChatDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_treeUsers;
	CListCtrl m_listMessages;
	CEdit m_editMessage;
	std::vector<ChatData> m_chats;
	afx_msg void OnBnClickedBtnSend();
	CStatic m_staticCurrentUser;

	CString m_currentUser;
	afx_msg void OnTvnSelchangedTreeUsers(NMHDR* pNMHDR, LRESULT* pResult);

	void DisplayMessage(const ChatMessage& message);
};
