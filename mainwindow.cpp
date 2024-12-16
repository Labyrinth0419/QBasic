#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <regex>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUIExitDebugMode();

    connect(ui->btnDebugMode, &QPushButton::clicked, this, &MainWindow::setUIForDebugMode);
    connect(ui->btnExitDebugMode, &QPushButton::clicked, this, &MainWindow::setUIExitDebugMode);
	connect(ui->btnClearCode, &QPushButton::clicked, this, &MainWindow::clearAll);
	connect(ui->btnRunCode, &QPushButton::clicked, this, &MainWindow::Run);
    connect(&programList, &ProgramList::inputMessage, this, &MainWindow::input);
    connect(&programList, &ProgramList::inputOnlyMsg, this, [this](const std::string& name) {
        inputOnly = true;
        input(name);
        });
    connect(&programList, &ProgramList::RenewDisplaySignal, this, [this]() {
		TextBrowserDisplayRenew();
        });
    connect(&programList, &ProgramList::loadSignal, this, &MainWindow::importFile);
    connect(&programList, &ProgramList::exitSignal, this, &QApplication::quit);
	connect(ui->btnLoadCode, &QPushButton::clicked, this, &MainWindow::importFile);
    connect(ui->btnDebugMode, &QPushButton::clicked, this, [this]() {
		programList.setDebugMode(true);
		MonitorDisplayRenew();
		breakPointsDisplayRenew();
        });
	connect(ui->btnExitDebugMode, &QPushButton::clicked, this, [this]() {
		programList.setDebugMode(false);
		});
    connect(&programList, &ProgramList::RenewDebugDisplaySignal, this, [this]() {
		MonitorDisplayRenew();
		breakPointsDisplayRenew();
        });
    connect(ui->btnDebugResume, &QPushButton::clicked, this, [this]() {
        if (programList.isRunning()){
            try{
                programList.step();
                programList.run();
            }
			catch (std::exception& e) {
				programList.stop();
				showErrorMessage(e.what());
			}
        }
        });
    connect(&programList, &ProgramList::pauseAtLineSignal, this, [this](int LineNum) {
		QTextCursor cursor = ui->textBrowser->textCursor();
		cursor.movePosition(QTextCursor::Start);
		ui->textBrowser->setTextCursor(cursor);
        ui->textBrowser->insertHtml("<font color = 'red'>(Program is paused at " 
            + QString::number(LineNum) 
            + ")</font><br>");
        });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearAll()
{
	ui->CodeDisplay->clear();
	ui->monitorDisplay->clear();
	ui->textBrowser->clear();
	ui->breakPointsDisplay->clear();
	ui->treeDisplay->clear();
	programList.clear();
}

void MainWindow::Run()
{
    try{
		TextBrowserDisplayRenew();
        programList.run();
        //TextBrowserDisplayRenew();
    }
	catch (std::exception& e) {
        programList.stop();
		showErrorMessage(e.what());
	}
    RenewAll();
}

void MainWindow::input(const std::string& name)
{
    programList.pause();
	ui->cmdLineEdit->setText("?"); // 设置文本为"?"，并且颜色为绿色
	ui->cmdLineEdit->setStyleSheet("QLineEdit { color: green; }");
    connect(ui->cmdLineEdit, &QLineEdit::textChanged, this, &MainWindow::on_cmdLineEdit_textChanged);
	inputMode = true;
	inputname = name;
}

void MainWindow::codeDisplayRenew()
{
	ui->CodeDisplay->clear();
	ui->CodeDisplay->append(StdStringToQString(programList.CodeDisplay()));
}


void MainWindow::SyntaxTreeDisplayRenew()
{
	ui->treeDisplay->clear();
	ui->treeDisplay->append(StdStringToQString(programList.SyntaxTreeDisplay()));
}

void MainWindow::TextBrowserDisplayRenew()
{
	ui->textBrowser->clear();
	ui->textBrowser->setTextColor(QColor(0, 0, 0));
	ui->textBrowser->append(StdStringToQString(programList.ResultDisplay()));
}

void MainWindow::RenewAll()
{
	codeDisplayRenew();
	MonitorDisplayRenew();
	SyntaxTreeDisplayRenew();
}

void MainWindow::breakPointsDisplayRenew()
{
	ui->breakPointsDisplay->clear();
	ui->breakPointsDisplay->append(StdStringToQString(programList.BreakPointDisplay()));
}

void MainWindow::MonitorDisplayRenew()
{
	ui->monitorDisplay->clear();
	ui->monitorDisplay->append(StdStringToQString(programList.MonitorDisplay()));
}

void MainWindow::on_cmdLineEdit_editingFinished()
{
    if(!inputMode){
        TextBrowserDisplayRenew();
        QString cmd = ui->cmdLineEdit->text();
        std::string cmdStr = QStringToStdString(cmd);
        try {
            programList.SentenseHandle(cmdStr);
        }
		catch (CommandExcep& e) {
            showErrorMessage(e.what());
		}
        catch (std::exception& e) {
            programList.stop();
            showErrorMessage(e.what());
        }
        ui->cmdLineEdit->setText("");
        RenewAll();
    }
    else {
		QString valueQstr = ui->cmdLineEdit->text();
		std::string valueStr = QStringToStdString(valueQstr).substr(1);
        try {
            disconnect(ui->cmdLineEdit, &QLineEdit::textChanged, this, &MainWindow::on_cmdLineEdit_textChanged);
            inputMode = false;
            ui->cmdLineEdit->setText("");
            ui->cmdLineEdit->setStyleSheet("QLineEdit { color: black; }");
            std::regex pattern("^\\s*-?[0-9]+\\s*$");
			if (!std::regex_match(valueStr, pattern)) {
				throw CommandExcep("Input Error: Invalid input at \"" + valueStr + "\"");
			}
            int value = std::stoi(valueStr);
            programList.Input(inputname, value);
		    inputname = "";
            if(inputOnly){               
				inputOnly = false;
            }
            else {
                try {
                    programList.run();
                }
                catch (std::exception& e) {
					throw RunExcep(e.what());
                }
            }
		    RenewAll();
		}
        catch (CommandExcep& e) {
            showErrorMessage(e.what());
        }
        catch (std::exception& e) {
            programList.stop();
            showErrorMessage(e.what());
        }
    }
}


void MainWindow::on_cmdLineEdit_textChanged(const QString& text)
{
    if(inputMode){
        if (text.isEmpty()) {
            ui->cmdLineEdit->setText("? ");
        }
        else if (text[0] != '?') {
            ui->cmdLineEdit->setText("?" + text);
        }
    }
}

void MainWindow::importFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString code = file.readAll();
            file.close();
			std::string codeStr = QStringToStdString(code);
			clearAll();
            try{
                programList.Load(codeStr);
                RenewAll();
				TextBrowserDisplayRenew();
                QMessageBox::information(this, "File Imported", "File has been successfully imported.");
            }
			catch (std::exception& e) {
				showErrorMessage(e.what());
			}
        }
        else {
            QMessageBox::warning(this, "File Error", "Unable to open the file.");
        }
    }
}


void MainWindow::setUIForDebugMode(){
    ui->btnClearCode->setVisible(false);
    ui->btnLoadCode->setVisible(false);
    ui->btnDebugMode->setVisible(false);

    ui->btnExitDebugMode->setVisible(true);
    ui->btnDebugResume->setVisible(true);

    ui->labelSyntaxTree->setVisible(false);
    ui->treeDisplay->setVisible(false);

    ui->labelMonitor->setVisible(true);
    ui->monitorDisplay->setVisible(true);
    ui->labelBreakPoints->setVisible(true);
    ui->breakPointsDisplay->setVisible(true);
}

void MainWindow::setUIExitDebugMode(){
    ui->btnClearCode->setVisible(true);
    ui->btnLoadCode->setVisible(true);
    ui->btnDebugMode->setVisible(true);

    ui->btnExitDebugMode->setVisible(false);
    ui->btnDebugResume->setVisible(false);

    ui->labelSyntaxTree->setVisible(true);
    ui->treeDisplay->setVisible(true);

    ui->labelMonitor->setVisible(false);
    ui->monitorDisplay->setVisible(false);
    ui->labelBreakPoints->setVisible(false);
    ui->breakPointsDisplay->setVisible(false);
}

void MainWindow::showErrorMessage(std::string msg)
{//显示错误信息，颜色为红色
	ui->textBrowser->clear();
    ui->textBrowser->setTextColor(QColor(255, 0, 0));
	ui->textBrowser->append(StdStringToQString(msg));
}
