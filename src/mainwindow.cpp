#include "mainwindow.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>
#include <QMap>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_server = new QTcpServer(this);
	m_blockSize = 0;
	m_port = 1111;

	if (!m_server->listen(QHostAddress::Any, m_port))
	{
		qDebug("Server Error");
		m_server->close();
		return;
	}

	connect(m_server, &QTcpServer::newConnection, this, &MainWindow::slotNewConnection);
}

MainWindow::~MainWindow()
{
	m_server->close();
}

void MainWindow::sendToClient(QTcpSocket* socket, const QString& str, quint16 fromID, quint16 code)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_3);
	out << quint16(0) << QTime::currentTime() << fromID << code << str;
	out.device()->seek(0);
	out << quint16(arrBlock.size() - sizeof(quint16));
	socket->write(arrBlock);
}

void MainWindow::slotNewConnection()
{
	QTcpSocket* clientSocket = m_server->nextPendingConnection();
	connect(clientSocket, &QTcpSocket::disconnected, this, &MainWindow::slotClientDisconnected);
	connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QObject::deleteLater);
	connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::slotReadClient);
}

void MainWindow::slotClientDisconnected()
{
	QTcpSocket* pClient = static_cast<QTcpSocket*>(QObject::sender());
	if (m_socketMap.size() > 0) {
		quint16 num = m_socketMap.key(pClient);
		m_socketMap.remove(num);
		for (const auto & socket : m_socketMap) {
			sendToClient(socket, "", num, eDisconnected);
		}
	}
}

void MainWindow::slotReadClient()
{
	QTcpSocket* clientSocket = (QTcpSocket*)sender();
	QDataStream in(clientSocket);
	in.setVersion(QDataStream::Qt_5_3);
	for (;;) {
		if (!m_blockSize) {
			if (clientSocket->bytesAvailable() < sizeof(quint16)) {
				break;
			}
			in >> m_blockSize;
		}

		int a = clientSocket->bytesAvailable();

		if (clientSocket->bytesAvailable() < m_blockSize) {
			break;
		}

		QTime time;
		QString str;
		quint16 fromID, toID, code;
		in >> time >> fromID >> toID >> code >> str;

		if (0 == toID) {
			m_socketMap[fromID] = clientSocket;
			ui.textEdit->append("Connected " + QString::number(fromID));
			for (const auto & socket : m_socketMap) {
				sendToClient(socket, "", fromID, eConnected);
				sendToClient(m_socketMap[fromID], "", m_socketMap.key(socket), eConnected);
			}
		}
		else {
			if (m_socketMap.find(toID) != m_socketMap.end()) {
				QString strMessage = str;
				sendToClient(m_socketMap.value(toID), strMessage, fromID, code);
			}
		}

		m_blockSize = 0;
	}
}