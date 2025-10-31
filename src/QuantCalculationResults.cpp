#include "QuantCalculationResults.h"
#include <QDebug>

namespace Quant
{
	QuantCalculationResults::QuantCalculationResults(QObject* parent) : QObject(parent) {}

	void QuantCalculationResults::SetSlippage(double slippage)
	{
		if (slippage == m_slippage)
			return;

		m_slippage = slippage;
		emit ResultsChanged();
	}

	void QuantCalculationResults::SetFees(double fees)
	{
		if (fees == m_fees)
			return;

		m_fees = fees;
		emit ResultsChanged();
	}

	void QuantCalculationResults::SetMarketImpact(double market_impact)
	{
		if (market_impact == m_market_impact)
			return;

		m_market_impact = market_impact;
		emit ResultsChanged();
	}

	void QuantCalculationResults::SetNetCost(double net_cost)
	{
		if (net_cost == m_net_cost)
			return;

		m_net_cost = net_cost;
		emit ResultsChanged();
	}

	void QuantCalculationResults::SetCryptoAmount(double crypto_amount)
	{
		if (crypto_amount == m_crypto_amount)
			return;

		m_crypto_amount = crypto_amount;
		emit ResultsChanged();
	}

	void QuantCalculationResults::SetMakerRation(double maker_taker_ratio)
	{
		if (maker_taker_ratio == m_maker_ratio)
			return;

		m_maker_ratio = maker_taker_ratio;
		emit ResultsChanged();
	}

	void QuantCalculationResults::SetVolatility(double volatility)
	{
		if (volatility == m_volatility)
			return;

		m_volatility = volatility;
		emit ResultsChanged();
	}

	void QuantCalculationResults::SetProcessingTime(double processing_time)
	{
		if (processing_time == m_processing_time)
			return;

		m_processing_time = processing_time;
		emit ResultsChanged();
	}
}