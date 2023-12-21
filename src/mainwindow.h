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
		eConnected = 1,
		eDisconnected,
		eTyping,
	};

	QTcpServer* m_server;
	quint16		m_blockSize;
	int		m_port;
	Ui::MainWindowClass ui;
	QMap<int, QTcpSocket*> m_socketMap;

	void sendToClient(QTcpSocket*, const QString&, quint16, quint16);

public slots:
	virtual void slotNewConnection();
	void slotClientDisconnected();
	// почему virtual?
	void slotReadClient();

};
