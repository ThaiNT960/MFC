// CChatDlg.cpp : implementation file
//

#include "pch.h"
#include "Tuan2.h"
#include "CChatDlg.h"
#include "afxdialogex.h"


// CChatDlg dialog

IMPLEMENT_DYNAMIC(CChatDlg, CDialogEx)

CChatDlg::CChatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHAT_DIALOG, pParent)
{

}

CChatDlg::~CChatDlg()
{
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_USERS, m_treeUsers);
	DDX_Control(pDX, IDC_LIST_MESSAGES, m_listMessages);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_editMessage);
	DDX_Control(pDX, IDC_STATIC_CURRENT_USER, m_staticCurrentUser);
}


BEGIN_MESSAGE_MAP(CChatDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SEND, &CChatDlg::OnBnClickedBtnSend)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_USERS, &CChatDlg::OnTvnSelchangedTreeUsers)
END_MESSAGE_MAP()


// CChatDlg message handlers

BOOL CChatDlg::OnInitDialog(){
	CDialogEx::OnInitDialog();

	SetWindowText(_T("Chat"));

	HTREEITEM hFriends = m_treeUsers.InsertItem(_T("Bạn bè"));
	m_treeUsers.InsertItem(_T("Thái"), hFriends);
	m_treeUsers.InsertItem(_T("Trường"), hFriends);
	m_treeUsers.InsertItem(_T("Nguyễn"), hFriends);
	m_treeUsers.Expand(hFriends, TVE_EXPAND);

	m_staticCurrentUser.SetWindowText(_T("Chưa chọn người chat"));

	m_listMessages.SetExtendedStyle(
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_listMessages.InsertColumn(0, _T("Thời gian"), LVCFMT_LEFT, 110);
	m_listMessages.InsertColumn(1, _T("Người gửi"), LVCFMT_LEFT, 130);

	CRect rect;
	m_listMessages.GetClientRect(&rect);
	int contentWidth = rect.Width()- m_listMessages.GetColumnWidth(0)- m_listMessages.GetColumnWidth(1);

	m_listMessages.InsertColumn(2, _T("Nội dung"),LVCFMT_LEFT, contentWidth);

	ChatMessage msg;

	ChatData chatThai;
	chatThai.username = _T("Thái");

	msg.time = _T("15:10");
	msg.sender = _T("Thái");
	msg.content = _T("Xin chào");
	chatThai.messages.push_back(msg);

	msg.time = _T("15:11");
	msg.sender = _T("Tôi");
	msg.content = _T("Chào Thái");
	chatThai.messages.push_back(msg);

	m_chats.push_back(chatThai);

	ChatData chatTruong;
	chatTruong.username = _T("Trường");

	msg.time = _T("14:20");
	msg.sender = _T("Trường");
	msg.content = _T("Bạn khỏe không?");
	chatTruong.messages.push_back(msg);

	msg.time = _T("14:21");
	msg.sender = _T("Tôi");
	msg.content = _T("Mình khỏe");
	chatTruong.messages.push_back(msg);

	m_chats.push_back(chatTruong);

	ChatData chatNguyen;
	chatNguyen.username = _T("Nguyễn");

	msg.time = _T("10:30");
	msg.sender = _T("Nguyễn");
	msg.content = _T("Đang làm gì đó?");
	chatNguyen.messages.push_back(msg);

	msg.time = _T("10:32");
	msg.sender = _T("Tôi");
	msg.content = _T("Đang chơi");
	chatNguyen.messages.push_back(msg);

	m_chats.push_back(chatNguyen);

	return TRUE;
}


void CChatDlg::DisplayMessage(const ChatMessage& message)
{
	int maxWidth = m_listMessages.GetColumnWidth(2) - 16;
	if (maxWidth < 30)
		maxWidth = 30;

	CString line;
	BOOL firstRow = TRUE;

	for (int i = 0; i < message.content.GetLength(); i++)
	{
		CString testLine = line;
		testLine += message.content[i];
		if (!line.IsEmpty() && m_listMessages.GetStringWidth(testLine) > maxWidth)
		{
			int row = m_listMessages.InsertItem(
				m_listMessages.GetItemCount(), _T(""));
			if (firstRow)
			{
				m_listMessages.SetItemText(row, 0, message.time);
				m_listMessages.SetItemText(row, 1, message.sender);
			}
			m_listMessages.SetItemText(row, 2, line);
			firstRow = FALSE;
			line.Empty();
			line += message.content[i];
		}
		else
		{
			line = testLine;
		}
	}
	if (!line.IsEmpty() || firstRow)
	{
		int row = m_listMessages.InsertItem(
			m_listMessages.GetItemCount(), _T(""));
		if (firstRow)
		{
			m_listMessages.SetItemText(row, 0, message.time);
			m_listMessages.SetItemText(row, 1, message.sender);
		}
		m_listMessages.SetItemText(row, 2, line);
	}
}

void CChatDlg::OnBnClickedBtnSend()
{
	if (m_currentUser.IsEmpty()) {
		MessageBox(_T("Vui lòng chọn người muốn chat"));
		return;
	}

	CString message;
	m_editMessage.GetWindowText(message);

	if (message.IsEmpty())
		return;

	ChatMessage newMessage;
	newMessage.time = CTime::GetCurrentTime().Format(_T("%H:%M"));
	newMessage.sender = _T("Tôi");
	newMessage.content = message;

	for (int i = 0; i < m_chats.size(); i++) {
		if (m_chats[i].username == m_currentUser) {
			m_chats[i].messages.push_back(newMessage);
			DisplayMessage(newMessage);
			break;
		}
	}

	m_editMessage.SetWindowText(_T(""));
	m_editMessage.SetFocus();
}


void CChatDlg::OnTvnSelchangedTreeUsers(NMHDR* pNMHDR, LRESULT* pResult)
{
	HTREEITEM hSelected = m_treeUsers.GetSelectedItem();

	if (hSelected != NULL) {
		HTREEITEM hParent = m_treeUsers.GetParentItem(hSelected);

		if (hParent != NULL) {
			m_currentUser = m_treeUsers.GetItemText(hSelected);

			CString title = _T("Đang chat với: ") + m_currentUser;
			m_staticCurrentUser.SetWindowText(title);

			m_listMessages.DeleteAllItems();

			for (int i = 0; i < m_chats.size(); i++) {
				if (m_chats[i].username == m_currentUser) {
					for (int j = 0; j < m_chats[i].messages.size(); j++) {
						DisplayMessage(m_chats[i].messages[j]);
					}
					break;
				}
			}
		}
	}

	*pResult = 0;
}
