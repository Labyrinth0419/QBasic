#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ProgramList.h"
#include <QByteArray>
#include <QKeyEvent>
#include <qfiledialog.h>
#include <qmessagebox.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_cmdLineEdit_editingFinished();
    void clearAll();
	void Run();

private:
	bool inputMode = false;
	bool inputOnly = false;
	std::string inputname = "";
private slots:
	void input(const std::string& name);
	void on_cmdLineEdit_textChanged(const QString& text);
	void importFile();

private:
    Ui::MainWindow *ui;
	ProgramList programList;

    void setUIForDebugMode();
    void setUIExitDebugMode();

private:
	void showErrorMessage(std::string msg);

private:
	void codeDisplayRenew();
	void SyntaxTreeDisplayRenew();
	void TextBrowserDisplayRenew();
	void RenewAll();
	void breakPointsDisplayRenew();
	void MonitorDisplayRenew();

private://解决编码问题
	std::string QStringToStdString(QString str) {
		QByteArray ba = str.toUtf8();
		return ba.constData();
	}
	QString StdStringToQString(std::string stdStr) {
		QByteArray byteArray = QByteArray::fromRawData(stdStr.c_str(), stdStr.size());
		return QString::fromUtf8(byteArray);
	}
};
#endif // MAINWINDOW_H
