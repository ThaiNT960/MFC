
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

#define WM_PIPE_MESSAGE (WM_USER + 1)
#define PIPE_NAME_SELF _T("\\\\.\\pipe\\AppChat1Pipe")

#define PIPE_NAME_TARGET _T("\\\\.\\pipe\\AppChat2Pipe")

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
	ON_WM_COPYDATA()
	ON_MESSAGE(WM_PIPE_MESSAGE, &CAppChat1Dlg::OnPipeMessage)
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

	HANDLE hThread = CreateThread(NULL, 0, PipeServerThread, (LPVOID)GetSafeHwnd(), 0, NULL);
	if (hThread == NULL)
		MessageBox(_T("Không tạo được thread nhận Pipe"));
	else
		CloseHandle(hThread);

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

	CString time = CTime::GetCurrentTime().Format(_T("%H:%M:%S"));

	CString data = m_selectedUser + _T("|") + time + _T("|") + message;

	BOOL sent = SendViaWindowsMessage(data);
	//BOOL sent = SendViaNamedPipe(data);

	if (!sent)
		return;

	ChatMessage newMessage;
	newMessage.time = time;
	newMessage.sender = _T("Tôi");
	newMessage.content = message;

	m_chatHistory[m_selectedUser].push_back(newMessage);

	int row = m_listMessages.InsertItem(
		m_listMessages.GetItemCount(), time);

	m_listMessages.SetItemText(row, 1, _T("Tôi"));
	m_listMessages.SetItemText(row, 2, message);

	m_editMessage.SetWindowText(_T(""));
}

BOOL CAppChat1Dlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (pCopyDataStruct->dwData != 1 || pCopyDataStruct->lpData == NULL)
		return FALSE;

	CString data = (LPCTSTR)pCopyDataStruct->lpData;

	int first = data.Find(_T("|"));
	int second = data.Find(_T("|"), first + 1);

	if (first < 0 || second < 0)
		return FALSE;

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
		int row = m_listMessages.InsertItem(
			m_listMessages.GetItemCount(), time);

		m_listMessages.SetItemText(row, 1, _T("AppChat2"));
		m_listMessages.SetItemText(row, 2, content);
	}

	return TRUE;
}


BOOL CAppChat1Dlg::SendViaWindowsMessage(const CString& data)
{
	HWND hAppChat2 = ::FindWindow(NULL, _T("AppChat2"));

	if (hAppChat2 == NULL)
	{
		MessageBox(_T("Chưa mở AppChat2"));
		return FALSE;
	}

	COPYDATASTRUCT cds;
	cds.dwData = 1;
	cds.cbData = (data.GetLength() + 1) * sizeof(TCHAR);
	cds.lpData = (PVOID)(LPCTSTR)data;

	return (BOOL)::SendMessage(
		hAppChat2, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
}

BOOL CAppChat1Dlg::SendViaNamedPipe(const CString& data)
{
	HANDLE hPipe = CreateFile(
		PIPE_NAME_TARGET,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		MessageBox(_T("Không kết nối được tới Pipe của AppChat2"));
		return FALSE;
	}

	DWORD bytesToWrite = (data.GetLength() + 1) * sizeof(TCHAR);
	DWORD bytesWritten = 0;

	BOOL result = WriteFile(
		hPipe,
		(LPCTSTR)data,
		bytesToWrite,
		&bytesWritten,
		NULL);

	CloseHandle(hPipe);

	if (!result || bytesWritten != bytesToWrite)
	{
		MessageBox(_T("Gửi tin qua Pipe thất bại"));
		return FALSE;
	}

	return TRUE;
}

DWORD WINAPI CAppChat1Dlg::PipeServerThread(LPVOID pParam)
{
	HWND hWnd = (HWND)pParam;

	while (true)
	{
		HANDLE hPipe = CreateNamedPipe(
			PIPE_NAME_SELF,
			PIPE_ACCESS_INBOUND,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			4096, 4096, 0, NULL);

		if (hPipe == INVALID_HANDLE_VALUE)
			return 0;

		BOOL connected = ConnectNamedPipe(hPipe, NULL);

		if (connected || GetLastError() == ERROR_PIPE_CONNECTED)
		{
			TCHAR buffer[4096];
			DWORD bytesRead = 0;

			if (ReadFile(hPipe, buffer,
				sizeof(buffer) - sizeof(TCHAR),
				&bytesRead, NULL) && bytesRead > 0)
			{
				buffer[bytesRead / sizeof(TCHAR)] = 0;

				CString* data = new CString(buffer);
				if (!::PostMessage(hWnd, WM_PIPE_MESSAGE, (WPARAM)data, 0))
					delete data;
			}
		}

		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);
	}

	return 0;
}

LRESULT CAppChat1Dlg::OnPipeMessage(WPARAM wParam, LPARAM lParam)
{
	CString* dataPtr = (CString*)wParam;
	CString data = *dataPtr;
	delete dataPtr;

	int first = data.Find(_T("|"));
	int second = data.Find(_T("|"), first + 1);

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
		int row = m_listMessages.InsertItem(
			m_listMessages.GetItemCount(), time);

		m_listMessages.SetItemText(row, 1, _T("AppChat2"));
		m_listMessages.SetItemText(row, 2, content);
	}

	return 0;
}
