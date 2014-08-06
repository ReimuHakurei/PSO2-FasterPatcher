
// PSO2 FasterPatcherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PSO2 FasterPatcher.h"
#include "PSO2 FasterPatcherDlg.h"
#include "afxdialogex.h"

// hashlib++
#include <hl_md5wrapper.cpp>
#include <hl_md5.cpp>
hashwrapper *md5Wrapper = new md5wrapper();

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Prototypes
UINT startPatching(LPVOID pParam);
char* genHumanReadableSize(uint64_t bytes);
uint64_t downloadFile(CHttpConnection *pConnection, CString FilePath, char* fileName, int currLine, int filesToDownload, uint64_t totalDownload, uint64_t downloadedSoFar, CString downloadingWhat);

// Shared functions for hashing.
// 
// strtolower and strtoupper are shamelessly copy-pasted from this GitHub
// project, as of commit c8fd8f3f8fcc4cafb3db78d0d2a8445ef90e1d26:
// https://github.com/png85/dsncompat/
// 
// Copyright (c) 2012, #das-system Networks
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and / or other materials provided with the distribution.
// * Neither the name of the copyright holders nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
char* strtoupper(char* s) {
	assert(s != NULL);

	char* p = s;
	while (*p != '\0') {
		*p = toupper(*p);
		p++;
	}

	return s;
}

char* strtolower(char* s) {
	assert(s != NULL);

	char* p = s;
	while (*p != '\0') {
		*p = tolower(*p);
		p++;
	}

	return s;
}

// This function generates a human-readable size from a number.
char* genHumanReadableSize(uint64_t bytes) {
	char humanReadableSize[127];

	if ((bytes / 1000) > 1000000) {
		sprintf(humanReadableSize, "%.1f GB", (float)bytes / 1000000000);
	}
	else if ((bytes / 1000) > 1000) {
		sprintf(humanReadableSize, "%.1f MB", (float)bytes / 1000000);
	}
	else if (bytes > 1000) {
		sprintf(humanReadableSize, "%.0f kB", (float)bytes / 1000);
	}
	else {
		sprintf(humanReadableSize, "%.0f bytes", (float)bytes);
	}

	// We're immediately strcpy()ing this, so this is perfectly safe. So we shut up the compiler warnings.
	#pragma warning( suppress : 4172 )
	return humanReadableSize;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPSO2FasterPatcherDlg dialog



CPSO2FasterPatcherDlg::CPSO2FasterPatcherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPSO2FasterPatcherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPSO2FasterPatcherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPSO2FasterPatcherDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PATCH, &CPSO2FasterPatcherDlg::OnBnClickedPatch)
END_MESSAGE_MAP()


// CPSO2FasterPatcherDlg message handlers

BOOL CPSO2FasterPatcherDlg::OnInitDialog()	
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

void CPSO2FasterPatcherDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPSO2FasterPatcherDlg::OnPaint()
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
HCURSOR CPSO2FasterPatcherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CPSO2FasterPatcherDlg::OnBnClickedPatch()
{
	// This runs on the main thread, which is bad, so we spawn another thread to do all of our dirty work. :3
	CWnd* pwnd = AfxGetMainWnd(); // Pointer to main window
	CWinThread* pStartPatching = AfxBeginThread(startPatching, pwnd, THREAD_PRIORITY_NORMAL);
}

UINT startPatching(LPVOID pParam) {
	// This function literally does everything related to patching.		

	// Pointer to main window
	CWnd* pwnd = AfxGetMainWnd(); 

	// Disable all of the buttons.
	//
	CButton* bStartPatching;
	bStartPatching = (CButton*)pwnd->GetDlgItem(IDC_PATCH);
	bStartPatching->EnableWindow(FALSE);
	//
	CButton* bOptions;
	bOptions = (CButton*)pwnd->GetDlgItem(IDC_OPTIONS);
	bOptions->EnableWindow(FALSE);
	//
	CButton* bLaunch;
	bLaunch = (CButton*)pwnd->GetDlgItem(IDC_LAUNCH);
	bLaunch->EnableWindow(FALSE);

	// Get a pointer to our progress bar.
	CProgressCtrl* m_progBar = (CProgressCtrl*)pwnd->GetDlgItem(IDC_PROGRESS);

	// CStrings for our text fields
	CString BottomText;
	CString TopText;

	// Open an object for a connection to the patch server.
	CInternetSession Session;
	CHttpConnection *pConnection = Session.GetHttpConnection(L"download.pso2.jp", INTERNET_FLAG_RELOAD, 80);

	m_progBar->SetMarquee(true,1000);

	uint64_t patchDownloadAmt;
	// Time to download patch lists...
	patchDownloadAmt = downloadFile(pConnection, L"/patch_prod/patches/launcherlist.txt", "patchlist1.txt", 1, 4, NULL, 0, L"patch list");
	patchDownloadAmt = downloadFile(pConnection, L"/patch_prod/patches/patchlist.txt", "patchlist2.txt", 2, 4, NULL, patchDownloadAmt, L"patch list");
	patchDownloadAmt = downloadFile(pConnection, L"/patch_prod/patches_old/patchlist.txt", "patchlist3.txt", 3, 4, NULL, patchDownloadAmt, L"patch list");
	patchDownloadAmt = downloadFile(pConnection, L"/patch_prod/patches/version.ver", "version.ver", 4, 4, NULL, patchDownloadAmt, L"patch list");

	// What these are:
	// line stores the current line when we're going through patchlist checking files. 
	// However, due to 3 AM coding, for some reason it's trimming off data from this.
	// So, as a workaround, we duplicate it before working on it.
	char line[8192];
	char output[8192];

	FILE * patchList;
	
	patchList = _wfopen(L"patchlist.txt", L"w+");

	FILE * mergeFiles;

	// Merge the files together.
	mergeFiles = _wfopen(L"patchlist1.txt", L"r");
	while (fgets(line, 8192, mergeFiles)) {

		fputs(line, patchList);
	}
	fclose(mergeFiles);
	_unlink("patchlist1.txt");
	mergeFiles = _wfopen(L"patchlist2.txt", L"r");
	while (fgets(line, 8192, mergeFiles)) {

		fputs(line, patchList);
	}
	fclose(mergeFiles);
	_unlink("patchlist2.txt");
	mergeFiles = _wfopen(L"patchlist3.txt", L"r");
	while (fgets(line, 8192, mergeFiles)) {
		fputs(line, patchList);
	}
	fclose(mergeFiles);
	_unlink("patchlist3.txt");

	fseek(patchList, 0, SEEK_SET);

	// This is a temporary file used to store the files needing to be downloaded.
	// We open it with w+ to avoid having to reopen it later, we can just fseek
	// back to the start when we're done generating it, and use it from there.
	FILE * downloadList = _wfopen(L"queue.txt", L"w+");

	// Check how many lines it has, then seek back to zero.
	int c = 0; while (fgets(line, 8192, patchList))c++;

	fseek(patchList, 0, SEEK_SET);

	m_progBar->SetMarquee(false,1000);

	// Set the maximum range of our progress bar to the number of lines in the file.
	m_progBar->SetRange32(0, c);

	// These store each line of the patchlist, broken up.
	char fileName[255];
	char fileSize[255];
	char fileChecksum[255];
	
	// We store the result from hashlib++ here.
	char existingFileChecksum[255];

	// Basic counters. This is the file currently being checked.
	int currLine = 0;

	// These are stats to be displayed in the interface. They serve no purpose other than
	// letting the user know this information.
	int filesOK = 0;
	int filesMismatch = 0;
	int filesMissing = 0;
	int filesToDownload = 0;

	// totalDownload stores the total size of files to be downloaded, in bytes.
	// currFileSize stores the size of the current file, as an integer.
	// downloadedSoFar stores how much has been received as of yet. This is used later, during patching.
	// These are uint64_t because it would overflow if a normal int was used.
	uint64_t totalDownload = 0;
	uint64_t currFileSize = 0;
	uint64_t downloadedSoFar = 0;

	// The total download amount, as a human-readable number.
	char sizeHumanReadable[127];

	while (fgets(line, 8192, patchList)) {
		currLine++;

		strcpy(output, line);

		strcpy(fileName, strtok(line, "\t"));
		strcpy(fileSize, strtok(NULL, "\t"));
		strcpy(fileChecksum, strtok(NULL, "\r\n"));

		// The filenames have a ".pat" at the end, we need to trim this.
		fileName[strlen(fileName) - 4] = '\0';

		// The PSO2 patchlists have the checksums in CAPITALS, but hashlib++ gives ones in lower-case.
		// We make both lowercase so we can strcmp.
		strcpy(fileChecksum, strtolower(fileChecksum));

		// Convert the filesize-as-string to a 64-bit integer.
		currFileSize = _atoi64(fileSize);

		try {
			// Calculate the checksum.
			strcpy(existingFileChecksum, md5Wrapper->getHashFromFile(fileName).c_str());

			// If the checksum matches, no need to do anything. Else, add it to our queue.
			//
			if (!strcmp(fileChecksum, existingFileChecksum)) {
				filesOK++;
			} else {
				filesMismatch++;
				fputs(output, downloadList);
				totalDownload += currFileSize;
			}
		}
		catch (hlException &e) {
			if (e.error_number() == HL_FILE_READ_ERROR) {
				// hashlib++ throws this exception if the file could not be opened. This will
				// almost always be because it is missing, so we will assume it is missing and
				// queue it for download.
				//
				filesMissing++;
				fputs(output, downloadList);
				totalDownload += currFileSize;
			}
		}

		// Generate a human-readable value for how much needs to be downloaded. 
		strcpy(sizeHumanReadable, genHumanReadableSize(totalDownload));

		// Set the status indicators on the main window, and update the progress bar.
		TopText.Format(L"Checking files... %d of %d checked.", currLine, c);
		pwnd->SetDlgItemTextW(IDC_TEXT_TOP, TopText);
		BottomText.Format(L"%d OK, %d missing, %d mismatch. %s to download.", filesOK, filesMissing, filesMismatch, CString(sizeHumanReadable));
		pwnd->SetDlgItemTextW(IDC_TEXT_BOTTOM, BottomText);
		m_progBar->SetPos(currLine);
	}

	// Close file handle for patchlist, we're done with it.
	fclose(patchList);

	// fseek() back to the start of our queue, we're re-using that file handle to do the patching part!
	fseek(downloadList, 0, SEEK_SET);

	// No need to re-calculate the size of the queue, we can just add the missing file count to the 
	// invalid file count to get the same number.
	filesToDownload = filesMissing + filesMismatch;

	// Preparations complete, time to reset the user interface!
	// 
	// Reset our progress bar back to zero, and set the range to the number of kB to download.
	// MFC only offers up to SetRange32, which means 32-bit, but PSO2 patches on a bad day can
	// be well over 2GB, so if we divide by 1024 and use kB instead, we can not have to worry
	// about that.
	m_progBar->SetRange32(0, (int)(totalDownload / 1024));
	m_progBar->SetPos(0);

	TopText.Format(L"Preparing to download files, please wait...");
	pwnd->SetDlgItemTextW(IDC_TEXT_TOP, TopText);
	BottomText.Format(L"");
	pwnd->SetDlgItemTextW(IDC_TEXT_BOTTOM, BottomText);

	currLine = 0;

	while (fgets(line, 8192, downloadList)) {
		currLine++;

		strcpy(fileName, strtok(line, "\t"));
		strcpy(fileSize, strtok(NULL, "\t"));
		strcpy(fileChecksum, strtok(NULL, "\n"));

		// The filenames have a ".pat" at the end, we need to trim this.
		fileName[strlen(fileName) - 4] = '\0';

		currFileSize = _atoi64(fileSize);


		CString FilePath;

		// The .pat is in the URL so we re-add it here, and add our leading /.
		FilePath.Format(L"/patch_prod/patches/%s.pat", CString(fileName));

		downloadedSoFar = downloadFile(pConnection, FilePath, fileName, currLine, filesToDownload, totalDownload, downloadedSoFar, L"file");
	}

	return 0;
}

uint64_t downloadFile(CHttpConnection *pConnection, CString FilePath, char* fileName, int currLine, int filesToDownload, uint64_t totalDownload, uint64_t downloadedSoFar, CString downloadingWhat) {
	CHttpFile *pFile;

	// The total download amount, as a human-readable number.
	char sizeHumanReadable[127];
	// The total amount downloaded so far, as a human-readable number.
	char downHumanReadable[127];
	// The current speed, as a human-readable number.
	char speedHumanReadable[127];

	// Generate a human-readable value for the total size.
	if (totalDownload) {
		strcpy(sizeHumanReadable, genHumanReadableSize(totalDownload));
	}
	else {
		strcpy(sizeHumanReadable, "unknown");
	}

	CWnd* pwnd = AfxGetMainWnd();
	CProgressCtrl* m_progBar = (CProgressCtrl*)pwnd->GetDlgItem(IDC_PROGRESS);

	CString BottomText;
	CString TopText;

	pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, FilePath, NULL, 1, NULL, L"1.1", INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE);
	pFile->AddRequestHeaders(L"User-Agent: AQUA_HTTP\r\n", HTTP_ADDREQ_FLAG_ADD_IF_NEW);

	pFile->SendRequest();
	DWORD dwStatusCode;
	pFile->QueryInfoStatusCode(dwStatusCode);

	if (dwStatusCode == 200) {
		char cBuffer[8192];

		// The current speed, in bytes/sec. This will not be displayed if the value is zero.
		int currSpeed = 0;
		// The time of the current sample, in milliseconds.
		uint64_t currSpeedCurrSampleTime = GetTickCount64();
		// The last time a sample was taken, in milliseconds.
		uint64_t currSpeedLastSampleTime = GetTickCount64();
		// The amount of data downloaded since the last sample.
		int dataSinceLastSample = 0;
		// We will take 8 samples, but we will display a speed estimate using however many are availiable.
		int currSpeedSamples[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		// The index that should be used for the next sample.
		uint64_t nextSampleIndex = 0;

		if (pFile != NULL) {
			// Open the file we're writing to...
			FILE * outFile = fopen(fileName, "wb");
			/// ...and check if we were successful. If so...
			if (outFile != NULL) {
				// pFile is a CHttpFile, which can be used as a CFile.
				// You cannot convert between a CFile and a FILE *, so we just read it using the associated Read function.
				
				while (int bytesRead = pFile->Read(cBuffer, 8192)) {
					int bytesWritten = fwrite(cBuffer, 1, bytesRead, outFile);
					if (bytesWritten != bytesRead) {
						AfxMessageBox(L"An error occurred while writing a file! Is your hard disk full?\r\n\r\nThe program will now close.");
						exit(1);
					}

					downloadedSoFar += bytesRead;

					// Here, we calculate the current speed.
					//
					// Update the amount downloaded since the last sample.
					dataSinceLastSample += bytesRead;
					// We only want to take samples every 500ms. Check if it has been at least 500ms since the last sample.
					if ((GetTickCount64() - currSpeedCurrSampleTime >= 500)) {
						// If it has been, we drop the current sample time back to last sample time and set the current one to the current time.
						currSpeedLastSampleTime = currSpeedCurrSampleTime;
						currSpeedCurrSampleTime = GetTickCount64();

						// We get the time between samples as a floating-point number.
						float timeBetweenSamples = (float)(currSpeedCurrSampleTime - currSpeedLastSampleTime) / 1000;

						// Calculate the current speed sample.
						currSpeedSamples[nextSampleIndex] = (int)((float)dataSinceLastSample / timeBetweenSamples);

						// Reset current speed back to zero, iterate through all samples, calculate average of all nonzero members.
						currSpeed = 0;
						int numValidSamples = 0;
						for (int c = 0; c < 16; c++) {
							if (currSpeedSamples[c]) {
								currSpeed += currSpeedSamples[c];
								numValidSamples++;
							}
						}
						currSpeed = currSpeed / numValidSamples;

						// Let the sample pool overflow.
						if (nextSampleIndex == 15) {
							nextSampleIndex = 0;
						}

						dataSinceLastSample = 0;
					}

					// Generate a human-readable value for how much has been downloaded. 
					strcpy(downHumanReadable, genHumanReadableSize(downloadedSoFar));
					// Generate a human-readable value for the current speed.
					strcpy(speedHumanReadable, genHumanReadableSize(currSpeed));

					if (totalDownload) {
						m_progBar->SetPos((int)(downloadedSoFar / 1024));
					}

					// Update the interface.
					TopText.Format(L"Downloading %s %d of %d.", downloadingWhat, currLine, filesToDownload);
					pwnd->SetDlgItemTextW(IDC_TEXT_TOP, TopText);
					BottomText.Format(L"Downloaded %s of %s at %s/sec", CString(downHumanReadable), CString(sizeHumanReadable), CString(speedHumanReadable));
					pwnd->SetDlgItemTextW(IDC_TEXT_BOTTOM, BottomText);
				}
				fclose(outFile);
			}
		}
		else {
			CString Message;
			Message.Format(L"Error downloading file! The server returned status code %d. This file will be skipped.", dwStatusCode);
			AfxMessageBox(Message);
		}
	}

	return downloadedSoFar;
}