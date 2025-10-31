#include "QuantOrderBook.h"

namespace
{
    int levels_num = 50;
	constexpr int preallocated_entries = 50;

}

namespace Quant
{
    QuantOrderbook::QuantOrderbook(QObject *parent) : QObject(parent)
    {
        // Reserve capacity in containers during construction
        m_bid_entries.reserve(preallocated_entries);
		m_ask_entries.reserve(preallocated_entries);
    }

    void QuantOrderbook::updateOrderbook(const QJsonArray &bids, const QJsonArray &asks)
    {
        // Input validation
        if (bids.isEmpty())
			qWarning() << "Empty bids array received";

        if (asks.isEmpty())
			qWarning() << "Empty asks array received";

    	// Clear previous data
        m_bid_entries.clear();
        m_ask_entries.clear();

        // Process bids
        for (unsigned int idx = 0; idx < qMin(levels_num, bids.size()); idx++)
        {
            const QJsonValue bid_value = bids.at(idx);
            if (!bid_value.isArray())
            {
                qWarning() << "Invalid bid entry format at index " << idx;
                continue;
			}

            const QJsonArray bid_entry = bid_value.toArray();
            if (bid_entry.size() < 2)
            {
	            qWarning() << "Invalid bid entry size at index " << idx;
				continue;
            }


            OrderEntry item;
            item.price = bid_entry[PRICE].toString();
            item.amount = bid_entry[AMOUNT].toString();
            m_bid_entries.append(item);
        }

        // Process Asks
        for (unsigned int idx = 0; idx < qMin(levels_num, asks.size()); idx++)
        {
            const QJsonValue ask_value = asks.at(idx);
            if (!ask_value.isArray())
            {
                qWarning() << "Invalid ask entry format at index " << idx;
                continue;
            }

            const QJsonArray ask_entry = ask_value.toArray();
            if (ask_entry.size() < 2)
            {
                qWarning() << "Invalid ask entry size at index " << idx;
                continue;
			}

            OrderEntry item;
             item.price = ask_entry[PRICE].toString();
             item.amount = ask_entry[AMOUNT].toString();
            m_ask_entries.append(item);
        }

        emit orderbookUpdated();
    }

    QVariantList QuantOrderbook::getBids() const
    {
        if (m_bid_entries.empty())
			qWarning() << "Bids are empty";

        return entriesAsVariantList(m_bid_entries);
    }

    QVariantList QuantOrderbook::getAsks() const
    {
        if (m_ask_entries.empty())
            qWarning() << "Asks are empty";

        return entriesAsVariantList(m_ask_entries);
    }

    QVariantList QuantOrderbook::entriesAsVariantList(const QVector<OrderEntry>& entries, bool reverse) const
    {
        QVariantList result;
        result.reserve(entries.size());

    	if (reverse)
    	{

    		for (auto it = entries.rbegin(); it != entries.rend(); ++it)
		    {
	            QVariantMap item;
				item["price"] = it->price;
	            item["amount"] = it->amount;
				result.append(item);
	        }

        }
        else
            for (const auto& entry : entries)
            {
                QVariantMap item;
                item["price"] = entry.price;
                item["amount"] = entry.amount;
                result.append(item);
            }

        return result;
    }


}