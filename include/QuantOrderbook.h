#pragma once
#include <QJsonArray>
#include <QObject>

namespace Quant
{
    class QuantOrderbook : public QObject
    {
        Q_OBJECT

    	// TODO: Update the code to work with Q_PROPERTYs
        //Q_PROPERTY(QVariantList bids READ getBids NOTIFY orderbookUpdated)
        //Q_PROPERTY(QVariantList asks READ getAsks NOTIFY orderbookUpdated)
    public:
        explicit QuantOrderbook(QObject *parent = nullptr);

        // Methods to update data
        void updateOrderbook(const QJsonArray &bids, const QJsonArray &asks);

        // Methods to expose data to QML
        Q_INVOKABLE QVariantList getBids() const;
        Q_INVOKABLE QVariantList getAsks() const;

    signals:
        void orderbookUpdated();

    private:
        enum ITEM_TYPE
        {
            PRICE = 0,
            AMOUNT = 1,
            COUNT = 2
		};

        struct OrderEntry
        {
            QString price;
            QString amount;
        };

        QVector<OrderEntry> m_bid_entries;
        QVector<OrderEntry> m_ask_entries;

    private:
        QVariantList entriesAsVariantList(const QVector<OrderEntry>& entries, bool reverse = false) const;
    };

}
