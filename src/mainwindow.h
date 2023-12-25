#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include <QMap>

class QTcpServer;
class QTcpSocket;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

private:
	enum eCodes {
		eNo,
		eMessage,
		eFile,
		eConnected,
		eDisconnected,
		eTyping,
	};

	QTcpServer* m_server;
	quint16		m_blockSize;
	QString filePath;
	quint16 fileSize;
	quint16 sizeReceivedData;
	int		m_port;
	Ui::MainWindowClass ui;
	QMap<int, QTcpSocket*> m_socketMap;

	void sendToClient(QTcpSocket*, const QString&, quint16, quint16);

public slots:
	virtual void slotNewConnection();
	void slotClientDisconnected();
	void slotReadClient();

};
