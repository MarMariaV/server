#include "mainwindow.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>
#include <QMap>
#include <QFile>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_server = new QTcpServer(this);
	m_blockSize = 0;
	m_port = 1111;
	sizeReceivedData = 0;

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
	QDataStream stream(socket);
	stream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
	auto a = m_socketMap.key(socket);
	stream << QTime::currentTime() << fromID << code << str;
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
	in.setVersion(QDataStream::Qt_DefaultCompiledVersion);

	while (!in.atEnd())
	{
		QTime time;
		QString str;
		quint16 fromID, toID, code;
		in >> time >> fromID >> toID >> code ;

		if (0 == toID && fromID != 0) {
			m_socketMap[fromID] = clientSocket;
			ui.textEdit->append("Connected " + QString::number(fromID));
			for (const auto & socket : m_socketMap) {
				sendToClient(socket, "", fromID, eConnected);
				sendToClient(m_socketMap[fromID], "", m_socketMap.key(socket), eConnected);
			}
		}
		else {
			if (m_socketMap.find(toID) != m_socketMap.end()) {
				switch (code)
				{
				case eMessage: case eTyping: {
					QString strMessage;
					in >> strMessage;
					sendToClient(m_socketMap.value(toID), strMessage, fromID, code);
					break;
				}
				case eFile: {
					in >> filePath >> fileSize;

					if (sizeReceivedData < fileSize)
					{
						auto splitName = filePath.split(".");
						QString str = splitName.first() + QTime::currentTime().toString() + "." + splitName.last();
						QFile file(str.replace(":", "_"));
						file.open(QIODevice::WriteOnly | QFile::Append);
						m_socketMap.value(fromID)->waitForReadyRead(1000);

						QByteArray tmpBlock;
						in >> tmpBlock;
						qint64 toFile = file.write(tmpBlock);
						sizeReceivedData += toFile;
						tmpBlock.clear();

						file.close();
					}
					filePath.clear();
					fileSize = 0;
					sizeReceivedData = 0;
					break;
				}
				default: break;
				}
			}
		}
	}

}