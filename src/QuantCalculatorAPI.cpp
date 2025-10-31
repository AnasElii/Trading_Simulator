#include "QuantCalculatorAPI.h"

#include <qelapsedtimer.h>

#include "QuantCalculationResults.h"

namespace {
	// Utility
	inline double percentageToUSD(double percentage, double baseAmount) {
		return baseAmount * (percentage / 100.0);
	}
}

namespace Quant
{
	QuantCalculatorAPI::QuantCalculatorAPI(QObject* parent) : QObject(parent)
	{
		// Create results object when calculator is created
		m_result = new QuantCalculationResults(this);
	};

	QuantCalculatorAPI::~QuantCalculatorAPI()
	{
		/**
		 * Since Bocth QuantInputHandler and QuantOrderbook are derived from QObject
		 * it's better to use Qt's memory managment approach
		 **/
		if (m_input_handler)
		{
			m_input_handler->deleteLater();
			m_input_handler = nullptr;
		}

		if (m_orderbook)
		{
			m_orderbook->deleteLater();
			m_orderbook = nullptr;
		}

		if (m_result)
		{
			m_result->deleteLater();
			m_result = nullptr;
		}

		m_calculator_interface = nullptr;
	}

	IQuantCalculatorAPI* QuantCalculatorAPI::selectedExchange()
	{
		if (!m_input_handler)
		{
			qWarning() << "Calculate Engine: missing input handler";
			return nullptr;
		}

		EXCHANGE_API selected_exchange = m_input_handler->SelectedExchange();

		if (selected_exchange == EXCHANGE_API::NONE)
		{
			qDebug() << "No Exchange Selected";
			return nullptr;
		}
	
		if (QuantOKXCalculator::get().isSelected(EXCHANGE_API::OKX))
		{
			qDebug() << "OKX Selected";
			m_calculator_interface = &QuantOKXCalculator::get();
			return m_calculator_interface;
		}

		// Add other exchanges here as needed

		qDebug() << "No valid exchange selected";
		return nullptr;
	}

	void QuantCalculatorAPI::SetInputHandler(QuantInputHandler* input_handler)
	{
		if (m_input_handler != nullptr)
		{
			m_input_handler->deleteLater();
			m_input_handler = nullptr;
		}

		m_input_handler = input_handler;

		QObject::connect(m_input_handler, &QuantInputHandler::SelectedExchangeChanged,this, &QuantCalculatorAPI::OnInputChanged);
		QObject::connect(m_input_handler, &QuantInputHandler::SelectedAssetChanged, this, &QuantCalculatorAPI::OnInputChanged);
		QObject::connect(m_input_handler, &QuantInputHandler::OrderTypeChanged, this, &QuantCalculatorAPI::OnInputChanged);
		QObject::connect(m_input_handler, &QuantInputHandler::FeeTierChanged, this, &QuantCalculatorAPI::OnInputChanged);
		QObject::connect(m_input_handler, &QuantInputHandler::QuantityChanged, this, &QuantCalculatorAPI::OnInputChanged);
		QObject::connect(m_input_handler, &QuantInputHandler::VolatilityChanged, this, &QuantCalculatorAPI::OnInputChanged);

		QObject::connect(m_input_handler, &QuantInputHandler::CalculationPerformed, this, &QuantCalculatorAPI::Calculate);
		QObject::connect(m_input_handler, &QuantInputHandler::VolatilityEnabledChanged, this, &QuantCalculatorAPI::OnInputChanged);
		QObject::connect(m_input_handler, &QuantInputHandler::FeeTierChanged, this, &QuantCalculatorAPI::OnInputChanged);
		QObject::connect(m_input_handler, &QuantInputHandler::QuantityChanged, this, &QuantCalculatorAPI::Calculate);
		QObject::connect(m_input_handler, &QuantInputHandler::USDAmountChanged, this, &QuantCalculatorAPI::Calculate);
	}

	void QuantCalculatorAPI::SetOrderbook(QuantOrderbook* orderbook)
	{
		if (m_orderbook != nullptr)
		{
			m_orderbook->deleteLater();
			orderbook = nullptr;
		}

		m_orderbook = orderbook;

		QObject::connect(m_orderbook, &QuantOrderbook::orderbookUpdated, this, &QuantCalculatorAPI::OnOrderbookUpdated);
	}

	void QuantCalculatorAPI::Calculate()
	{
		if (!m_input_handler || !m_orderbook)
		{
			qWarning() << "Calculate Engine: missing input handler or orderbook input";
			return;
		}

		if (!m_calculator_interface)
		{
			qWarning() << "Calculate Engine: Invalid Calculator";
			return;
		}

		// Start timing
		QElapsedTimer time;
		time.start();

		// Get orderbook data
		auto bids = m_orderbook->getBids();
		auto asks = m_orderbook->getAsks();

		// Get input data
		// TODO: Add xchange check on the OKX calculator
		auto selected_exchagne = m_input_handler->SelectedExchange();
		auto order_type = m_input_handler->OrderType();
		auto fee_tier = m_input_handler->FeeTier();
		auto quantity = m_input_handler->Quantity();
		auto usd_amount = m_input_handler->USDAmount();

		// TODO: Add ORDER_SIDE enum to the QuantInputHandler class
		ORDER_SIDE order_side = ORDER_SIDE::BUY;

		// Calculate volatility from orderbook
		if (QuantOKXCalculator::isVolatilityEnabled())
		{
			m_volatility = m_input_handler->Volatility();
		}
		else
			m_volatility = m_calculator_interface->CalculateVolatilityFromOrderbook(bids, asks);


		// Calculate fees percentage
		double fee_pctg = m_calculator_interface->CalculateFees(usd_amount, fee_tier, true);

		// Calculate available amount after fees
		double available_usd = usd_amount / (1.0 + fee_pctg / 100.0);
		m_fees = usd_amount - available_usd;
		qDebug() << "fee: " << m_fees;
		// Calculate market order cost
		//m_market_order_cost = m_calculator_interface->CalculateMarketOrderCost(usd_amount, bids);

		// Calculate slippage
		double estimated_crypto = CalculateCryptoForFixedUSD(available_usd, order_side == ORDER_SIDE::BUY ? asks : bids);
		double slippage_pctg = m_calculator_interface->CalculateSlippage(estimated_crypto, bids, asks, order_type, order_side);
		//double slippage_pctg = m_calculator_interface->CalculateSlippage(usd_amount, bids, asks, order_type, order_side);
		//m_slippage = percentageToUSD(slippage_pctg, m_market_order_cost);

		// Apply slippage to reduce available USD
		m_slippage = available_usd * (slippage_pctg / 100.0);
		available_usd -= m_slippage;

		// Calculate market impact
		//double impact_pctg = m_calculator_interface->CalculateMarketImpact(usd_amount, m_volatility, bids, asks);
		//m_market_impact = percentageToUSD(impact_pctg, m_market_order_cost);
		double impact_pctg = m_calculator_interface->CalculateMarketImpact(estimated_crypto, m_volatility, bids, asks);
		m_market_impact = available_usd * (impact_pctg / 100.0);
		available_usd -= m_market_impact;

		// Final calculation of crypto amount after all costs
		double final_crypto_amount = CalculateCryptoForFixedUSD(available_usd, order_side == ORDER_SIDE::BUY ? asks : bids);

		qDebug() << "Final Crypto amount" << final_crypto_amount << " Net cost" << available_usd;

		// Set market order cost to original USD amount
		m_market_order_cost = usd_amount - m_fees - m_slippage - m_market_impact;

		// Calculate maker ratio
		m_maker_ratio = m_calculator_interface->CalculateMakerRatio(bids, asks);

		// Measure processing time in milliseconds
		double elapsed_ms = time.elapsed();
		QuantOKXCalculator::SetProcessingTime(elapsed_ms);

		// Update the results object
		QuantCalculationResults* results = qobject_cast<QuantCalculationResults*>(m_result);
		if (results)
		{
			//qDebug() << "Volatility: " << m_volatility;
			//qDebug() << "  Slippage:" << m_slippage;
			//qDebug() << "  Fees:" << m_fees;
			//qDebug() << "  Market Impact:" << m_market_impact;
			//qDebug() << "  Market Order Cost:" << m_market_order_cost;
			//qDebug() << "  Elapsed Time: " << elapsed_ms;
			//qDebug() << "  Net Cost:" <<  m_slippage + m_fees + m_market_impact;
			
			results->SetSlippage(m_slippage);
			results->SetFees(m_fees);
			results->SetMarketImpact(m_market_impact);
			results->SetNetCost(available_usd);
			results->SetCryptoAmount(final_crypto_amount);
			results->SetMakerRation(m_maker_ratio);
			results->SetVolatility(m_volatility);
			results->SetProcessingTime(elapsed_ms);
		}

		// Notify UI
		emit CalculationUpdated();
	}

	// New helper method to calculate crypto amount for a fixed USD amount
	double QuantCalculatorAPI::CalculateCryptoForFixedUSD(double usd_amount, const QVariantList& orderbook)
	{
		double remaining_usd = usd_amount;
		double total_crypto = 0.0;

		// Walk through orderbook entries until USD is spent
		for (int i = 0; i < orderbook.size() && remaining_usd > 0; i++)
		{
			QVariantMap level = orderbook[i].toMap();
			double price = level["price"].toDouble();
			double available_amount = level["amount"].toDouble();

			if (price <= 0.0) continue; // Protect against bad data

			double max_spendable_at_level = price * available_amount;
			double amount_to_spend = qMin(remaining_usd, max_spendable_at_level);
			double crypto_acquired = amount_to_spend / price;

			total_crypto += crypto_acquired;
			remaining_usd -= amount_to_spend;
		}

		return total_crypto;
	}

	void QuantCalculatorAPI::OnOrderbookUpdated()
	{
		Calculate();
	}

	void QuantCalculatorAPI::OnInputChanged()
	{
		Calculate();
	}

}
