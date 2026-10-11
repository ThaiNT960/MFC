
// AppChat2Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "AppChat2.h"
#include "AppChat2Dlg.h"
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


// CAppChat2Dlg dialog



CAppChat2Dlg::CAppChat2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APPCHAT2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAppChat2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_USERS, m_treeUsers);
	DDX_Control(pDX, IDC_LIST_MESSAGES, m_listMessages);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_editMessage);
}

BEGIN_MESSAGE_MAP(CAppChat2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_USERS, &CAppChat2Dlg::OnTvnSelchangedTreeUsers)
	ON_BN_CLICKED(IDC_BTN_SEND, &CAppChat2Dlg::OnBnClickedBtnSend)
	ON_MESSAGE(WM_APPCHAT_TCP_RECEIVED, &CAppChat2Dlg::OnTcpReceived)
END_MESSAGE_MAP()


// CAppChat2Dlg message handlers

BOOL CAppChat2Dlg::OnInitDialog()
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
	SetWindowText(_T("AppChat2"));

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

	m_udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_udpSocket == INVALID_SOCKET)
	{
		MessageBox(_T("Không tạo được UDP socket"));
		return TRUE;
	}


	sockaddr_in udpAddress = {};
	udpAddress.sin_family = AF_INET;
	udpAddress.sin_port = htons(5001);
	udpAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(m_udpSocket, (sockaddr*)&udpAddress, sizeof(udpAddress)) == SOCKET_ERROR)
	{
		MessageBox(_T("Không mở được cổng UDP 5001"));
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

void CAppChat2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAppChat2Dlg::OnPaint()
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
HCURSOR CAppChat2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAppChat2Dlg::OnTvnSelchangedTreeUsers(NMHDR* pNMHDR, LRESULT* pResult)
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

static bool SendAll(SOCKET s, const char* data, int size)
{
	int total = 0;
	while (total < size)
	{
		int sent = send(s, data + total, size - total, 0);
		if (sent <= 0) return false;
		total += sent;
	}
	return true;
}

static bool RecvAll(SOCKET socket, char* data, int size)
{
	int total = 0;
	while (total < size)
	{
		int received = recv(socket, data + total, size - total, 0);
		if (received <= 0)
			return false;
		total += received;
	}
	return true;
}

DWORD WINAPI CAppChat2Dlg::ReceiveThread(LPVOID param)
{
	CAppChat2Dlg* dlg = (CAppChat2Dlg*)param;

	while (true)
	{
		u_long networkLength = 0;
		if (!RecvAll(dlg->m_serverSocket,
			(char*)&networkLength, sizeof(networkLength)))
			break;

		u_long contentBytes = ntohl(networkLength);
		if (contentBytes == 0 ||
			contentBytes % sizeof(wchar_t) != 0 ||
			contentBytes > 1024 * 1024)
			break;

		int charCount = (int)(contentBytes / sizeof(wchar_t));
		wchar_t* text = new wchar_t[charCount + 1];

		if (!RecvAll(dlg->m_serverSocket, (char*)text, (int)contentBytes))
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

	closesocket(dlg->m_serverSocket);
	dlg->m_serverSocket = INVALID_SOCKET;
	return 0;
}

DWORD WINAPI CAppChat2Dlg::UdpReceiveThread(LPVOID param)
{
	CAppChat2Dlg* dlg = (CAppChat2Dlg*)param;
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

		if (!::PostMessageW(dlg->m_hWnd, WM_APPCHAT_TCP_RECEIVED,
			0, (LPARAM)text))
		{
			delete[] text;
			break;
		}
	}
	return 0;
}

LRESULT CAppChat2Dlg::OnTcpReceived(WPARAM, LPARAM lParam)
{
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
	newMessage.sender = _T("AppChat1");
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

void CAppChat2Dlg::OnBnClickedBtnSend()
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

BOOL CAppChat2Dlg::SendViaTcp(const CString& data)
{
	if (m_serverSocket == INVALID_SOCKET)
	{
		CString peerIP;
		GetDlgItemText(IDC_EDIT_PEER_IP, peerIP);

		sockaddr_in serverAddress = {};
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(5000);

		if (InetPtonW(AF_INET, peerIP.GetString(), &serverAddress.sin_addr) != 1)
		{
			MessageBox(_T("IP máy kia không hợp lệ"));
			return FALSE;
		}

		m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_serverSocket == INVALID_SOCKET)
		{
			MessageBox(_T("Không tạo được TCP socket"));
			return FALSE;
		}

		if (connect(m_serverSocket, (sockaddr*)&serverAddress,
			sizeof(serverAddress)) == SOCKET_ERROR)
		{
			MessageBox(_T("Không kết nối được AppChat1"));
			closesocket(m_serverSocket);
			m_serverSocket = INVALID_SOCKET;
			return FALSE;
		}

		m_receiveThread = CreateThread(
			nullptr, 0, ReceiveThread, this, 0, nullptr);
		if (m_receiveThread == nullptr)
		{
			MessageBox(_T("Không tạo được luồng nhận TCP"));
			closesocket(m_serverSocket);
			m_serverSocket = INVALID_SOCKET;
			return FALSE;
		}
	}

	int contentBytes = data.GetLength() * sizeof(wchar_t);
	u_long networkLength = htonl((u_long)contentBytes);

	if (!SendAll(m_serverSocket, (const char*)&networkLength, sizeof(networkLength)) ||
		!SendAll(m_serverSocket, (const char*)data.GetString(), contentBytes))
	{
		MessageBox(_T("Gửi TCP thất bại"));
		closesocket(m_serverSocket);
		m_serverSocket = INVALID_SOCKET;
		return FALSE;
	}
	return TRUE;
}

BOOL CAppChat2Dlg::SendViaUdp(const CString& data)
{
	if (m_udpSocket == INVALID_SOCKET)
	{
		MessageBox(_T("UDP socket chưa sẵn sàng"));
		return FALSE;
	}

	CString peerIP;
	GetDlgItemText(IDC_EDIT_PEER_IP, peerIP);

	sockaddr_in destination = {};
	destination.sin_family = AF_INET;
	destination.sin_port = htons(5000); // Cổng UDP của AppChat1

	if (InetPtonW(AF_INET, peerIP.GetString(), &destination.sin_addr) != 1)
	{
		MessageBox(_T("IP máy kia không hợp lệ"));
		return FALSE;
	}

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