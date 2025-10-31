#pragma once
#include <QtWebSockets/QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

namespace Quant
{
	class QuantWebSocket : public QObject
	{
		Q_OBJECT

	public:
		explicit QuantWebSocket(QObject* parent = nullptr);
		~QuantWebSocket();

	public:
		void connect(const QString& url);
		void disconnect();
		bool isConnected() const;

	signals:
		void connected();
		void disconnected();
		void orderbookUpdated(const QJsonArray& bids, const QJsonArray& asks);
		void error(const QString& error_message);

	private slots:
		void onConnected();
		void onDisconnected();
		void onTextMessageReceived(const QString& message);
		void onError(QAbstractSocket::SocketError error);

	private:
		QJsonParseError m_parse_error;
		QByteArray m_message_buffer;

	private:
		QWebSocket m_webSocket;
		bool m_is_connected;
	};
}