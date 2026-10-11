
// Tuan1Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Tuan1.h"
#include "Tuan1Dlg.h"
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


// CTuan1Dlg dialog



CTuan1Dlg::CTuan1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TUAN1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTuan1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CONTENT, m_editContent);
	DDX_Control(pDX, IDC_EDIT_PATH, m_editPath);
}

BEGIN_MESSAGE_MAP(CTuan1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CTuan1Dlg::OnBnClickedBtnBrowser)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CTuan1Dlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CTuan1Dlg::OnBnClickedBtnLoad)
END_MESSAGE_MAP()


// CTuan1Dlg message handlers

BOOL CTuan1Dlg::OnInitDialog()
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTuan1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTuan1Dlg::OnPaint()
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
HCURSOR CTuan1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CTuan1Dlg::OnBnClickedBtnBrowser()
{
	CFileDialog dlg(TRUE, _T("txt"), nullptr, OFN_FILEMUSTEXIST, _T("Text File (*.txt)|*.txt||"));
	if (dlg.DoModal() == IDOK) {
		CString path = dlg.GetPathName();
		m_editPath.SetWindowText(path);
	}
}


void CTuan1Dlg::OnBnClickedBtnSave()
{
	CString filePath, content;
	m_editPath.GetWindowText(filePath);
	m_editContent.GetWindowText(content);

	if (filePath.IsEmpty()) {
		MessageBox(_T("Bạn chưa chọn file nào"));
		return;
	}

	HANDLE hFile = CreateFile(filePath, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(_T("Không thể tạo file"));
		return;
	}

	DWORD bytesToWrite = content.GetLength() * sizeof(WCHAR);
	DWORD bytesWritten = 0;

	BOOL ok = WriteFile(hFile, (LPCWSTR)content,
		bytesToWrite, &bytesWritten, NULL);

	CloseHandle(hFile);

	if (ok && bytesWritten == bytesToWrite)
		MessageBox(_T("Lưu file thành công"));
	else
		MessageBox(_T("Ghi file thất bại"));

}


void CTuan1Dlg::OnBnClickedBtnLoad()
{
	CString filePath;
	m_editPath.GetWindowText(filePath);

	if (filePath.IsEmpty()) {
		MessageBox(_T("Bạn chưa chọn file"));
		return;
	}

	HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(_T("Không thể đọc file"));
		return;
	}

	CString content;
	WCHAR buffer[1025];
	DWORD bytesRead = 0;

	while (TRUE) {
		BOOL ok = ReadFile(hFile, buffer, 1024 * sizeof(WCHAR),
			&bytesRead, NULL);

		if (!ok || bytesRead % sizeof(WCHAR) != 0) {
			CloseHandle(hFile);
			MessageBox(_T("Đọc file thất bại"));
			return;
		}

		if (bytesRead == 0)
			break;

		buffer[bytesRead / sizeof(WCHAR)] = 0;
		content += buffer;
	}

	CloseHandle(hFile);
	m_editContent.SetWindowText(content);
}
