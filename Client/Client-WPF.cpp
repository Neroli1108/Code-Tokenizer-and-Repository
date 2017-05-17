////////////////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application                //
//          - Demo for CSE 687 Project #4                                //
// ver 2.0                                                               //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015             //
///////////////////////////////////////////////////////////////////////////
/*
*  To run as a Windows Application:
*  - Set Project Properties > Linker > System > Subsystem to Windows
*  - Comment out int main(...) at bottom
*  - Uncomment int _stdcall WinMain() at bottom
*  To run as a Console Application:
*  - Set Project Properties > Linker > System > Subsytem to Console
*  - Uncomment int main(...) at bottom
*  - Comment out int _stdcall WinMain() at bottom
*/

#include "Client-WPF.h"
using namespace CppCliWindows;

WPFCppCliDemo::WPFCppCliDemo()
{
	// set up channel
	ObjectFactory* pObjFact = new ObjectFactory;
	pSendr_ = pObjFact->createSendr();
	pRecvr_ = pObjFact->createRecvr();
	pChann_ = pObjFact->createMockChannel(pSendr_, pRecvr_);
	pChann_->start();
	delete pObjFact;

	// client's receive thread
	recvThread = gcnew Thread(gcnew ThreadStart(this, &WPFCppCliDemo::getMessage));
	recvThread->Start();

	// set event handlers
	this->Loaded +=
		gcnew System::Windows::RoutedEventHandler(this, &WPFCppCliDemo::OnLoaded);
	this->Closing +=
		gcnew CancelEventHandler(this, &WPFCppCliDemo::Unloading);
	hSendButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendMessage);
	hClearButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clear);
	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolder);
	hCheckinButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::checkInAction);
	hCheckinDoneButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::checkInDoneAction);
	hCancelCheckinButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::cancelCheckIn);
	hGetPkgButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::getPkgList);
	hDownloadButton1->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::download1);
	hDownloadButton2->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::download2);
	// set Window properties
	this->Title = "Client WPF";
	this->Width = 800;
	this->Height = 600;
	// attach dock panel to Window
	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);
	// setup Window controls and views
	setUpTabControl();
	setUpStatusBar();
	setUpSendMessageView();
	setUpFileListView();
	setUpConnectionView();
}

WPFCppCliDemo::~WPFCppCliDemo()
{
	delete pChann_;
	delete pSendr_;
	delete pRecvr_;
}

void WPFCppCliDemo::setUpStatusBar()
{
	hStatusBar->Items->Add(hStatusBarItem);
	hStatus->Text = "very important messages will appear here";
	//status->FontWeight = FontWeights::Bold;
	hStatusBarItem->Content = hStatus;
	hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

void WPFCppCliDemo::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hSendMessageTab->Header = "Send Message";
	hFileListTab->Header = "check in";
	hExtractionTab->Header = "extraction";
	hTabControl->Items->Add(hSendMessageTab);
	hTabControl->Items->Add(hFileListTab);
	hTabControl->Items->Add(hExtractionTab);
}

void WPFCppCliDemo::setTextBlockProperties()
{
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hSendMessageGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hTextBlock1;
	hTextBlock1->Padding = Thickness(15);
	hTextBlock1->Text = "";
	hTextBlock1->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hTextBlock1->FontWeight = FontWeights::Bold;
	hTextBlock1->FontSize = 16;
	hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer1->Content = hBorder1;
	hSendMessageGrid->SetRow(hScrollViewer1, 0);
	hSendMessageGrid->Children->Add(hScrollViewer1);
}

void WPFCppCliDemo::setButtonsProperties()
{
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	hSendMessageGrid->RowDefinitions->Add(hRow2Def);
	hSendButton->Content = "Send Message";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hClearButton->Content = "Clear";
	hBorder2->Child = hSendButton;
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hClearButton;
	hStackPanel1->Children->Add(hBorder2);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 10;
	hStackPanel1->Children->Add(hSpacer);
	hStackPanel1->Children->Add(hBorder3);
	hStackPanel1->Orientation = Orientation::Horizontal;
	hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hSendMessageGrid->SetRow(hStackPanel1, 1);
	hSendMessageGrid->Children->Add(hStackPanel1);
}

void WPFCppCliDemo::setUpSendMessageView()
{
	Console::Write("\n  setting up sendMessage view");
	hSendMessageGrid->Margin = Thickness(20);
	hSendMessageTab->Content = hSendMessageGrid;

	setTextBlockProperties();
	setButtonsProperties();
}

std::string WPFCppCliDemo::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
		dst += (char)pStr[i];
	return dst;
}

void WPFCppCliDemo::sendMessage(Object^ obj, RoutedEventArgs^ args)
{
	String^ msg = "message-here is body";
	pSendr_->postMessage(toStdString(msg));
	Console::Write("\n  sent message");
	hStatus->Text = "Sent message";
}

String^ WPFCppCliDemo::toSystemString(std::string& str)
{
	StringBuilder^ pStr = gcnew StringBuilder();
	for (size_t i = 0; i < str.size(); ++i)
		pStr->Append((Char)str[i]);
	return pStr->ToString();
}

void WPFCppCliDemo::addText(String^ msg)
{
	hTextBlock1->Text += msg + "\n\n";
}

void WPFCppCliDemo::getMessage()
{
	// recvThread runs this function

	while (true)
	{
		std::cout << "\n  receive thread calling getMessage()";
		std::string msg = pRecvr_->getMessage();

		String^ sMsg = toSystemString(msg);
		array<String^>^ args = gcnew array<String^>(1);
		//args[0] = sMsg;
		std::cout << "\n  received a message in WPF: " << toStdString(sMsg);
		int mark = sMsg->IndexOf("-");
		String^ command = sMsg;
		String^ data = sMsg; 
		command = command->Remove(mark);
		data = data->Substring(mark + 1);
		args[0] = data;
		if (command == "package list")
		{
			Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addPkgName);
			Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		}
		if (command == "download1")
		{
			String^ msg = "download1-" + data;
			pSendr_->postMessage(toStdString(msg));
		}
		if (command == "message")
		{
			Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addText);
			Dispatcher->Invoke(act, args);  // must call addText on main UI thread

		}
	}

}

void WPFCppCliDemo::clear(Object^ sender, RoutedEventArgs^ args)
{
	//String^ msg = "check in done-package \"Parser\"";
	//Console::Write("\n  ckeck in done");
	//pSendr_->postMessage(toStdString(msg));
	//hStatus->Text = "check in done";
	hTextBlock1->Text = "";
}

void WPFCppCliDemo::checkInAction(Object^ sender, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hListBox->SelectedItems->Count;
	hStatus->Text = "Show Selected Items";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hListBox->SelectedItems)
			items[index++] = item;
	}

	hListBox->Items->Clear();
	if (count > 0) {
		for each (String^ item in items){
			String^ itemname;
			int slsh = item->LastIndexOf("\\");
			itemname = item->Substring(slsh + 1);
			int pt = itemname->LastIndexOf(".");
			itemname = itemname->Remove(pt);
			if (checkinPkg == "")
				checkinPkg = itemname;
			else if (checkinPkg != itemname){
				std::cout << "\n\n files belong to different package " << toStdString(itemname) << std::endl;
				hListBox->Items->Clear();
				hListBox->Items->Add("Please select files belong to one package");
				checkinPkg == "";
				return;
			}
			hListBox->Items->Add(item);		
		}
		//check in there
		for each (String^ item in items){
			String^ msg = "check in-" + item;
			pSendr_->postMessage(toStdString(msg));
			Console::Write("\n  sent message");
			hStatus->Text = "Sent message chech in";
			ckeckin = true;
			checkinPkg = item;
		}
		int slsh = checkinPkg->LastIndexOf("\\");
		checkinPkg = checkinPkg->Substring(slsh + 1);
		int pt = checkinPkg->LastIndexOf(".");
		checkinPkg = checkinPkg->Remove(pt);
	}
}

void WPFCppCliDemo::checkInDoneAction(Object^ sender, RoutedEventArgs^ args) {
	if (ckeckin)
	{
		hListBox->Items->Clear();
		hListBox->Items->Add(" check in done, package: " + checkinPkg);

		String^ msg = "check in done-" + checkinPkg;
		pSendr_->postMessage(toStdString(msg));
		Console::Write("\n  check in done" + checkinPkg);
		hStatus->Text = "Sent message check in done";

		checkinPkg = "";
		ckeckin = false;
	}
	else {
		hListBox->Items->Clear();
		hListBox->Items->Add("Not in check in process");
	}
}

void WPFCppCliDemo::cancelCheckIn(Object^ sender, RoutedEventArgs^ args) {
	if (ckeckin)
	{
		hListBox->Items->Clear();
		hListBox->Items->Add(" cancel check in, package: " + checkinPkg);

		String^ msg = "cancel check in-" + checkinPkg;
		pSendr_->postMessage(toStdString(msg));
		Console::Write("\n  cancel check in" + checkinPkg);
		hStatus->Text = "Sent message cancel check in";

		checkinPkg = "";
		ckeckin = false;
	}
	else {
		hListBox->Items->Clear();
		hListBox->Items->Add("Not in check in process");
	}
}

void WPFCppCliDemo::setUpButtons1() {
	hFolderBrowseButton->Content = "Select Directory";
	hFolderBrowseButton->Height = 30;
	hFolderBrowseButton->Width = 120;
	hFolderBrowseButton->BorderThickness = Thickness(2);
	hFolderBrowseButton->BorderBrush = Brushes::Black;

	hCheckinButton->Content = "check in";
	hCheckinButton->Height = 30;
	hCheckinButton->Width = 120;
	hCheckinButton->BorderThickness = Thickness(2);
	hCheckinButton->BorderBrush = Brushes::Black;

	hCheckinDoneButton->Content = "check in done";
	hCheckinDoneButton->Height = 30;
	hCheckinDoneButton->Width = 120;
	hCheckinDoneButton->BorderThickness = Thickness(2);
	hCheckinDoneButton->BorderBrush = Brushes::Black;

	hCancelCheckinButton->Content = "cancel check in";
	hCancelCheckinButton->Height = 30;
	hCancelCheckinButton->Width = 120;
	hCancelCheckinButton->BorderThickness = Thickness(2);
	hCancelCheckinButton->BorderBrush = Brushes::Black;
}

void WPFCppCliDemo::setUpFileListView()
{
	Console::Write("\n  setting up FileList view");
	hFileListGrid->Margin = Thickness(20);
	hFileListTab->Content = hFileListGrid;

	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hFileListGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hListBox->SelectionMode = SelectionMode::Multiple;
	hBorder1->Child = hListBox;

	hFileListGrid->SetRow(hBorder1, 0);
	hFileListGrid->Children->Add(hBorder1);

	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	hFileListGrid->RowDefinitions->Add(hRow2Def);

	Grid^ hButtonGrid = gcnew Grid();
	hButtonGrid->Margin = Thickness(20);

	ColumnDefinition^ hColDef1 = gcnew ColumnDefinition();//hColDef1->Width = GridLength(140);
	ColumnDefinition^ hColDef2 = gcnew ColumnDefinition();//hColDef2->Width = GridLength(140);
	ColumnDefinition^ hColDef3 = gcnew ColumnDefinition();//hColDef3->Width = GridLength(140);
	ColumnDefinition^ hColDef4 = gcnew ColumnDefinition();//hColDef3->Width = GridLength(140);
	hButtonGrid->ColumnDefinitions->Add(hColDef1);
	hButtonGrid->ColumnDefinitions->Add(hColDef2);
	hButtonGrid->ColumnDefinitions->Add(hColDef3);
	hButtonGrid->ColumnDefinitions->Add(hColDef4);
	setUpButtons1();
	hButtonGrid->SetColumn(hFolderBrowseButton, 0);
	hButtonGrid->Children->Add(hFolderBrowseButton);
	hButtonGrid->SetColumn(hCheckinButton, 1);
	hButtonGrid->Children->Add(hCheckinButton);
	hButtonGrid->SetColumn(hCheckinDoneButton, 2);
	hButtonGrid->Children->Add(hCheckinDoneButton);
	hButtonGrid->SetColumn(hCancelCheckinButton, 3);
	hButtonGrid->Children->Add(hCancelCheckinButton);
	hFileListGrid->SetRow(hButtonGrid, 1);
	hFileListGrid->Children->Add(hButtonGrid);
	hFolderBrowserDialog->ShowNewFolderButton = false;
	hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}

void WPFCppCliDemo::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browsing for folder";
	hListBox->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = hFolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		for (int i = 0; i < files->Length; ++i)
			hListBox->Items->Add(files[i]);
		//array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		//for (int i = 0; i < dirs->Length; ++i)
		//	hListBox->Items->Add(L"<> " + dirs[i]);
	}
}

void WPFCppCliDemo::setUpButtons2() {
	hGetPkgButton->Content = "get package list";
	hGetPkgButton->Height = 30;
	hGetPkgButton->Width = 120;
	hGetPkgButton->BorderThickness = Thickness(2);
	hGetPkgButton->BorderBrush = Brushes::Black;

	hDownloadButton1->Content = "download (no dep)";
	hDownloadButton1->Height = 30;
	hDownloadButton1->Width = 120;
	hDownloadButton1->BorderThickness = Thickness(2);
	hDownloadButton1->BorderBrush = Brushes::Black;

	hDownloadButton2->Content = "download (dep)";
	hDownloadButton2->Height = 30;
	hDownloadButton2->Width = 120;
	hDownloadButton2->BorderThickness = Thickness(2);
	hDownloadButton2->BorderBrush = Brushes::Black;
}

void WPFCppCliDemo::setUpConnectionView()
{
	Console::Write("\n  setting up Connection view");
	hExtractionGrid->Margin = Thickness(20);
	hExtractionTab->Content = hExtractionGrid;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hExtractionGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hExtractionListBox->SelectionMode = SelectionMode::Multiple;
	hBorder1->Child = hExtractionListBox;

	hExtractionGrid->SetRow(hBorder1, 0);
	hExtractionGrid->Children->Add(hBorder1);

	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	hExtractionGrid->RowDefinitions->Add(hRow2Def);

	Grid^ hButtonGrid = gcnew Grid();
	hButtonGrid->Margin = Thickness(20);

	ColumnDefinition^ hColDef1 = gcnew ColumnDefinition();//hColDef1->Width = GridLength(140);
	ColumnDefinition^ hColDef2 = gcnew ColumnDefinition();//hColDef2->Width = GridLength(140);
	ColumnDefinition^ hColDef3 = gcnew ColumnDefinition();//hColDef3->Width = GridLength(140);
	hButtonGrid->ColumnDefinitions->Add(hColDef1);
	hButtonGrid->ColumnDefinitions->Add(hColDef2);
	hButtonGrid->ColumnDefinitions->Add(hColDef3);
	setUpButtons2();
	hButtonGrid->SetColumn(hGetPkgButton, 0);
	hButtonGrid->Children->Add(hGetPkgButton);
	hButtonGrid->SetColumn(hDownloadButton1, 1);
	hButtonGrid->Children->Add(hDownloadButton1);
	hButtonGrid->SetColumn(hDownloadButton2, 2);
	hButtonGrid->Children->Add(hDownloadButton2);
	hExtractionGrid->SetRow(hButtonGrid, 1);
	hExtractionGrid->Children->Add(hButtonGrid);
}

void WPFCppCliDemo::getPkgList(Object^ sender, RoutedEventArgs^ args) {
	Console::Write("\n  click get package button\n");
	hExtractionListBox->Items->Clear();

	String^ msg = "get package list-all";
	pSendr_->postMessage(toStdString(msg));
	hStatus->Text = "Sent message get package list";
}

void WPFCppCliDemo::addPkgName(String^ msg) {
	hExtractionListBox->Items->Add(msg);
}

void WPFCppCliDemo::download1(Object^ sender, RoutedEventArgs^ args) {
	Console::Write("\n  click download with dependency package\n");

	int index = 0;
	int count = hExtractionListBox->SelectedItems->Count;
	hStatus->Text = "download file";
	array<System::String^>^ items = gcnew array<String^>(count);

	if (count > 0) {
		for each (String^ item in hExtractionListBox->SelectedItems)
		{
			items[index++] = item;
		}
	}

	if (count > 0) {
		for each (String^ item in items)
		{
			String^ msg = "download1-" + item;
			pSendr_->postMessage(toStdString(msg));
		}
	}
}

void WPFCppCliDemo::download2(Object^ sender, RoutedEventArgs^ args) {
	Console::Write("\n  click download without dependency package\n");

	int index = 0;
	int count = hExtractionListBox->SelectedItems->Count;
	hStatus->Text = "download file";
	array<System::String^>^ items = gcnew array<String^>(count);

	if (count > 0) {
		for each (String^ item in hExtractionListBox->SelectedItems)
		{
			items[index++] = item;
		}
	}

	if (count > 0) {
		String^ item = items[0];
		String^ msg = "download1-" + item;
		pSendr_->postMessage(toStdString(msg));
		msg = "download2-" + item;
		pSendr_->postMessage(toStdString(msg));
	}
}

void WPFCppCliDemo::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  Window loaded");
}

void WPFCppCliDemo::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
	Console::Write("\n  Window closing");
}

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
	Console::Title = "Client";
	Console::WriteLine(L"\n Starting WPFCppCliDemo");

	Application^ app = gcnew Application();
	app->Run(gcnew WPFCppCliDemo());
	Console::WriteLine(L"\n\n");
}