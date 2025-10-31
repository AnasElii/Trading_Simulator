#include "QuantWebSocket.h"

#include <QtConcurrent/QtConcurrent>

namespace
{
	struct MessageType
	{
		const char* orderbook = "orderbook";
		const char* ticker = "ticker";
		const char* trades = "trades";
		const char* orderbook_update = "orderbook_update";
	};

	struct OrderbookUpdateType
	{
		static constexpr const char* bids = "bids";
		static constexpr const char* asks = "asks";
	};

	bool processDocument(const QJsonDocument json_doc, QJsonArray& bids, QJsonArray& asks)
	{
		if (json_doc.isNull())
		{
			return false;
		}
		if (!json_doc.isObject())
		{
			return false;
		}

		QJsonObject json_obj = json_doc.object();
		bids = json_obj[OrderbookUpdateType::bids].toArray();
		asks = json_obj[OrderbookUpdateType::asks].toArray();
	
		return !(asks.isEmpty() || bids.isEmpty());
	}
}

namespace Quant
{
	QuantWebSocket::QuantWebSocket(QObject* parent): QObject(parent), m_is_connected(false) 
	{
		// Connect websocket signal to slots
		QObject::connect(&m_webSocket, &QWebSocket::connected, this, &QuantWebSocket::onConnected);
		QObject::connect(&m_webSocket, &QWebSocket::disconnected, this, &QuantWebSocket::onDisconnected);
		QObject::connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &QuantWebSocket::onTextMessageReceived);
		QObject::connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), this, &QuantWebSocket::onError);
	}

	QuantWebSocket::~QuantWebSocket()
	{
		if (m_is_connected)
		{
			m_webSocket.close();
		}
	}

	void QuantWebSocket::connect(const QString& url)
	{
		//qDebug() << "Connecting to WebSocket endpoint:" << url;
		m_webSocket.open(QUrl(url));
	}

	void QuantWebSocket::disconnect()
	{
		if (m_is_connected)
		{
			m_webSocket.close();
		}
	}

	bool QuantWebSocket::isConnected() const
	{
		return m_is_connected;
	}

	void QuantWebSocket::onConnected()
	{
		m_is_connected = true;
		qDebug() << "WebSocket connected";
		emit connected();
	}
 
	void QuantWebSocket::onDisconnected()
	{
		m_is_connected = false;
		qDebug() << "WebSocket disconnected";
		emit disconnected();
	}

	void QuantWebSocket::onTextMessageReceived(const QString& message)
	{
		// Parsing JSON on the Working Thread
		QtConcurrent::run([this, message_copy = message]
			{

				// Parse the JSON response
				QByteArray message_buffer = message_copy.toUtf8();
				QJsonParseError parse_error;

				QJsonDocument json_doc = QJsonDocument::fromJson(message_buffer, &parse_error);
				if (m_parse_error.error != QJsonParseError::NoError)
				{
					// Thread-safe error handling
					QMetaObject::invokeMethod(this, [this]()
						{
							emit error("Invalid JSON response (not an object)");
						}, Qt::QueuedConnection);
					return;
				}

				// Update on the main thread
				QMetaObject::invokeMethod(this, [this, doc = std::move(json_doc)]()
					{
						QJsonArray bids, asks;
						if (!processDocument(doc, bids, asks))
						{
							emit error("Invalid JSON response (missing bids/asks)");
							return;
						}

						// Forward data to listeners
						emit orderbookUpdated(bids, asks);
					}, Qt::QueuedConnection);


			});

		
		

		

	}

	void QuantWebSocket::onError(QAbstractSocket::SocketError error)
	{
		qDebug() << "WebSocket error: " << m_webSocket.errorString();
		emit this->error(m_webSocket.errorString());
	}

}