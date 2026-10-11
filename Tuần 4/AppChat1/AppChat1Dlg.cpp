
// AppChat1Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "AppChat1.h"
#include "AppChat1Dlg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAppChat1Dlg dialog



CAppChat1Dlg::CAppChat1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APPCHAT1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAppChat1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MESSAGES, m_listMessages);
	DDX_Control(pDX, IDC_TREE_USERS, m_treeUsers);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_editMessage);
}

BEGIN_MESSAGE_MAP(CAppChat1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_USERS, &CAppChat1Dlg::OnTvnSelchangedTree1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MESSAGES, &CAppChat1Dlg::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BTN_SEND, &CAppChat1Dlg::OnBnClickedBtnSend)
	ON_MESSAGE(WM_APPCHAT_TCP_RECEIVED, &CAppChat1Dlg::OnTcpReceived)
END_MESSAGE_MAP()


// CAppChat1Dlg message handlers

BOOL CAppChat1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetWindowText(_T("AppChat1"));

	m_listMessages.SetExtendedStyle(
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_listMessages.InsertColumn(0, _T("Thời gian"), LVCFMT_LEFT, 120);
	m_listMessages.InsertColumn(1, _T("Người gửi"), LVCFMT_LEFT, 130);

	CRect rectList;
	m_listMessages.GetClientRect(&rectList);
	int widthContent = rectList.Width()
		- m_listMessages.GetColumnWidth(0)
		- m_listMessages.GetColumnWidth(1);

	m_listMessages.InsertColumn(2, _T("Nội dung"), LVCFMT_LEFT, widthContent);

	HTREEITEM hUsers = m_treeUsers.InsertItem(_T("Bạn bè"));
	m_treeUsers.InsertItem(_T("Thái"), hUsers);
	m_treeUsers.InsertItem(_T("Trường"), hUsers);
	m_treeUsers.InsertItem(_T("Nguyễn"), hUsers);
	m_treeUsers.Expand(hUsers, TVE_EXPAND);

	m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_listenSocket == INVALID_SOCKET)
	{
		MessageBox(_T("Không tạo được TCP socket"));
		return TRUE;
	}
	sockaddr_in address = {};
	address.sin_family = AF_INET;
	address.sin_port = htons(5000);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(m_listenSocket, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		MessageBox(_T("Không mở được cổng 5000"));
		closesocket(m_listenSocket);
		m_listenSocket = INVALID_SOCKET;
		return TRUE;
	}

	if (listen(m_listenSocket, 1) == SOCKET_ERROR)
	{
		MessageBox(_T("Không chờ được kết nối TCP"));
		closesocket(m_listenSocket);
		m_listenSocket = INVALID_SOCKET;
		return TRUE;
	}

	m_acceptThread = CreateThread(nullptr, 0, AcceptThread, this, 0, nullptr);
	if (m_acceptThread == nullptr)
	{
		MessageBox(_T("Không tạo được luồng chờ kết nối"));
		closesocket(m_listenSocket);
		m_listenSocket = INVALID_SOCKET;
		return TRUE;
	}


	m_udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_udpSocket == INVALID_SOCKET)
	{
		MessageBox(_T("Không tạo được UDP socket"));
		return TRUE;
	}

	sockaddr_in udpAddress = {};
	udpAddress.sin_family = AF_INET;
	udpAddress.sin_port = htons(5000);
	udpAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(m_udpSocket, (sockaddr*)&udpAddress, sizeof(udpAddress)) == SOCKET_ERROR)
	{
		MessageBox(_T("Không mở được cổng UDP 5000"));
		closesocket(m_udpSocket);
		m_udpSocket = INVALID_SOCKET;
		return TRUE;
	}

	m_udpThread = CreateThread(nullptr, 0, UdpReceiveThread, this, 0, nullptr);
	if (m_udpThread == nullptr)
	{
		MessageBox(_T("Không tạo được luồng nhận UDP"));
		closesocket(m_udpSocket);
		m_udpSocket = INVALID_SOCKET;
		return TRUE;
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAppChat1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAppChat1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAppChat1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAppChat1Dlg::OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	HTREEITEM hItem = m_treeUsers.GetSelectedItem();

	if (hItem != NULL && m_treeUsers.GetParentItem(hItem) != NULL)
		m_selectedUser = m_treeUsers.GetItemText(hItem);
	else
		m_selectedUser.Empty();

	m_listMessages.DeleteAllItems();

	if (!m_selectedUser.IsEmpty())
	{
		int count = (int)m_chatHistory[m_selectedUser].size();

		for (int i = 0; i < count; i++)
		{
			ChatMessage msg = m_chatHistory[m_selectedUser][i];

			int row = m_listMessages.InsertItem(
				m_listMessages.GetItemCount(), msg.time);

			m_listMessages.SetItemText(row, 1, msg.sender);
			m_listMessages.SetItemText(row, 2, msg.content);
		}
	}

	*pResult = 0;
}


void CAppChat1Dlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

static bool SendAll(SOCKET socket, const char* data, int size)
{
	int total = 0;
	while (total < size)
	{
		int sent = send(socket, data + total, size - total, 0);
		if (sent <= 0)
			return false;
		total += sent;
	}
	return true;
}

static bool RecvAll(SOCKET s, char* data, int size)
{
	int total = 0;
	while (total < size)
	{
		int received = recv(s, data + total, size - total, 0);
		if (received <= 0)
			return false;
		total += received;
	}
	return true;
}

DWORD WINAPI CAppChat1Dlg::AcceptThread(LPVOID param)
{
	CAppChat1Dlg* dlg = (CAppChat1Dlg*)param;
	dlg->m_clientSocket = accept(dlg->m_listenSocket, nullptr, nullptr);
	if (dlg->m_clientSocket == INVALID_SOCKET)
		return 0;

	while (true)
	{
		u_long networkLength = 0;
		if (!RecvAll(dlg->m_clientSocket,
			(char*)&networkLength, sizeof(networkLength)))
			break;

		u_long contentBytes = ntohl(networkLength);
		if (contentBytes == 0 ||
			contentBytes % sizeof(wchar_t) != 0 ||
			contentBytes > 1024 * 1024)
			break;

		int charCount = (int)(contentBytes / sizeof(wchar_t));
		wchar_t* text = new wchar_t[charCount + 1];

		if (!RecvAll(dlg->m_clientSocket, (char*)text, (int)contentBytes))
		{
			delete[] text;
			break;
		}

		text[charCount] = L'\0';
		if (!::PostMessageW(dlg->m_hWnd, WM_APPCHAT_TCP_RECEIVED,
			0, (LPARAM)text))
		{
			delete[] text;
			break;
		}
	}

	closesocket(dlg->m_clientSocket);
	dlg->m_clientSocket = INVALID_SOCKET;
	return 0;
}

DWORD WINAPI CAppChat1Dlg::UdpReceiveThread(LPVOID param)
{
	CAppChat1Dlg* dlg = (CAppChat1Dlg*)param;
	char buffer[65536];

	while (true)
	{
		sockaddr_in senderAddress = {};
		int senderSize = sizeof(senderAddress);

		int received = recvfrom(dlg->m_udpSocket, buffer, sizeof(buffer), 0,
			(sockaddr*)&senderAddress, &senderSize);

		if (received == SOCKET_ERROR)
			break;
		if (received == 0 || received % sizeof(wchar_t) != 0)
			continue;

		int charCount = received / sizeof(wchar_t);
		wchar_t* text = new wchar_t[charCount + 1];
		CopyMemory(text, buffer, received);
		text[charCount] = L'\0';

		sockaddr_in* peer = new sockaddr_in;
		*peer = senderAddress;

		if (!::PostMessageW(dlg->m_hWnd, WM_APPCHAT_TCP_RECEIVED,
			(WPARAM)peer, (LPARAM)text))
		{
			delete peer;
			delete[] text;
			break;
		}
	}
	return 0;
}

LRESULT CAppChat1Dlg::OnTcpReceived(WPARAM wParam, LPARAM lParam)
{    
	if (wParam != 0)
	{
		sockaddr_in* peer = (sockaddr_in*)wParam;
		m_udpPeerAddress = *peer;
		m_hasUdpPeer = TRUE;
		delete peer;
	}

	wchar_t* text = (wchar_t*)lParam;
	CString data(text);
	delete[] text;

	int first = data.Find(_T('|'));
	int second = data.Find(_T('|'), first + 1);
	if (first < 0 || second < 0)
		return 0;

	CString user = data.Left(first);
	CString time = data.Mid(first + 1, second - first - 1);
	CString content = data.Mid(second + 1);

	ChatMessage newMessage;
	newMessage.time = time;
	newMessage.sender = _T("AppChat2");
	newMessage.content = content;
	m_chatHistory[user].push_back(newMessage);

	if (user == m_selectedUser)
	{
		int row = m_listMessages.InsertItem(m_listMessages.GetItemCount(), time);
		m_listMessages.SetItemText(row, 1, newMessage.sender);
		m_listMessages.SetItemText(row, 2, content);
	}
	return 0;
}

void CAppChat1Dlg::OnBnClickedBtnSend()
{
	CString message;
	m_editMessage.GetWindowText(message);

	if (m_selectedUser.IsEmpty())
	{
		MessageBox(_T("Hãy chọn một người trong danh sách"));
		return;
	}

	if (message.IsEmpty())
	{
		MessageBox(_T("Hãy nhập nội dung tin nhắn"));
		return;
	}

	SYSTEMTIME now;
	GetLocalTime(&now);
	CString time;
	time.Format(_T("%02d:%02d:%02d"),now.wHour, now.wMinute, now.wSecond);
	CString data = m_selectedUser + _T("|") + time + _T("|") + message;

	//BOOL sent = SendViaTcp(data);
	BOOL sent = SendViaUdp(data);
	if (!sent)
		return;

	ChatMessage newMessage;
	newMessage.time = time;
	newMessage.sender = _T("Tôi");
	newMessage.content = message;
	m_chatHistory[m_selectedUser].push_back(newMessage);

	int row = m_listMessages.InsertItem(m_listMessages.GetItemCount(), time);
	m_listMessages.SetItemText(row, 1, _T("Tôi"));
	m_listMessages.SetItemText(row, 2, message);
	m_editMessage.SetWindowText(_T(""));
}

BOOL CAppChat1Dlg::SendViaTcp(const CString& data)
{
	if (m_clientSocket == INVALID_SOCKET)
	{
		MessageBox(_T("AppChat2 chưa kết nối"));
		return FALSE;
	}

	int contentBytes = data.GetLength() * sizeof(wchar_t);
	u_long networkLength = htonl((u_long)contentBytes);

	if (!SendAll(m_clientSocket, (const char*)&networkLength, sizeof(networkLength)) ||
		!SendAll(m_clientSocket, (const char*)data.GetString(), contentBytes))
	{
		MessageBox(_T("Gửi TCP thất bại"));
		return FALSE;
	}
	return TRUE;
}

BOOL CAppChat1Dlg::SendViaUdp(const CString& data)
{
	if (m_udpSocket == INVALID_SOCKET)
	{
		MessageBox(_T("UDP socket chưa sẵn sàng"));
		return FALSE;
	}

	if (!m_hasUdpPeer)
	{
		MessageBox(_T("AppChat2 cần gửi tin trước"));
		return FALSE;
	}

	sockaddr_in destination = m_udpPeerAddress;

	int contentBytes = data.GetLength() * sizeof(wchar_t);
	if (contentBytes > 65507)
	{
		MessageBox(_T("Tin nhắn quá dài cho một gói UDP"));
		return FALSE;
	}

	int sent = sendto(m_udpSocket, (const char*)data.GetString(),
		contentBytes, 0, (sockaddr*)&destination, sizeof(destination));

	if (sent != contentBytes)
	{
		MessageBox(_T("Gửi UDP thất bại"));
		return FALSE;
	}
	return TRUE;
}