
// Tuan2Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Tuan2.h"
#include "Tuan2Dlg.h"
#include "afxdialogex.h"
#include "CChatDlg.h"

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


// CTuan2Dlg dialog



CTuan2Dlg::CTuan2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TUAN2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTuan2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_editUsername);
	DDX_Control(pDX, IDC_EDIT2, m_editPassword);
}

BEGIN_MESSAGE_MAP(CTuan2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CTuan2Dlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_REGISTER, &CTuan2Dlg::OnBnClickedBtnRegister)
END_MESSAGE_MAP()


// CTuan2Dlg message handlers

BOOL CTuan2Dlg::OnInitDialog()
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
	SetWindowText(_T("Đăng ký/Đăng nhập"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTuan2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTuan2Dlg::OnPaint()
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
HCURSOR CTuan2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTuan2Dlg::OnBnClickedBtnLogin()
{
	CString username;
	CString password;

	m_editUsername.GetWindowText(username);
	m_editPassword.GetWindowText(password);

	if (username.IsEmpty() || password.IsEmpty()) {
		MessageBox(_T("Vui lòng nhập đầy đủ thông tin"));
		return;
	}
	for (int i = 0; i < m_users.size(); i++) {
		if (m_users[i].username == username && m_users[i].password == password) {
			MessageBox(_T("Đăng nhập thành công"));

			EndDialog(IDOK);
			return;
		}
	}
	MessageBox(_T("Sai username hoặc password"));
}



void CTuan2Dlg::OnBnClickedBtnRegister()
{
	CString username;
	CString password;

	m_editUsername.GetWindowText(username);
	m_editPassword.GetWindowText(password);

	if (username.IsEmpty() || password.IsEmpty()) {
		MessageBox(_T("Vui lòng nhập đầy đủ thông tin"));
		return;
	}
	for (int i = 0; i < m_users.size(); i++) {
		if (m_users[i].username == username ) {
			MessageBox(_T("Tên tài khoản đã tồn tại"));
			return;
		}
	}
	User newUser;
	newUser.username = username;
	newUser.password = password;

	m_users.push_back(newUser);

	MessageBox(_T("Đăng ký thành công"));

	m_editUsername.SetWindowText(_T(""));
	m_editPassword.SetWindowText(_T(""));

}
