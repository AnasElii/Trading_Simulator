#pragma once
#include <QObject>

namespace Quant
{
	class QuantCalculationResults : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(double expected_slippage READ Slippage WRITE SetSlippage NOTIFY ResultsChanged)
		Q_PROPERTY(double expected_fees READ Fees WRITE SetFees NOTIFY ResultsChanged)
		Q_PROPERTY(double expected_market_impact READ MarketImpact WRITE SetMarketImpact NOTIFY ResultsChanged)
		Q_PROPERTY(double net_cost READ NetCost WRITE SetNetCost NOTIFY ResultsChanged)
		Q_PROPERTY(double crypto_amount READ CryptoAmount WRITE SetCryptoAmount NOTIFY ResultsChanged)
		Q_PROPERTY(double maker_ratio READ MakerRation WRITE SetMakerRation NOTIFY ResultsChanged)
		Q_PROPERTY(double volatility READ Volatility WRITE SetVolatility NOTIFY ResultsChanged)
		Q_PROPERTY(double processing_time READ ProcessingTime WRITE SetProcessingTime NOTIFY ResultsChanged)

	public:
		QuantCalculationResults(QObject* parent = nullptr);

	public:
		void SetSlippage(double slippage);
		void SetFees(double fees);
		void SetMarketImpact(double market_impact);
		void SetNetCost(double net_cost);
		void SetCryptoAmount(double crypto_amount);
		void SetMakerRation(double maker_taker_ratio);
		void SetVolatility(double volatility);
		void SetProcessingTime(double processing_time);

	public:
		double Slippage() const { return m_slippage; }
		double Fees() const { return m_fees; }
		double MarketImpact() const { return m_market_impact; }
		double NetCost() const { return m_net_cost; }
		double CryptoAmount() const { return m_crypto_amount; }
		double MakerRation() const { return m_maker_ratio; }
		double Volatility() const { return m_volatility; }
		double ProcessingTime() const { return m_processing_time; }

	signals:
		void ResultsChanged();

	private:
		double m_slippage = 0.0;
		double m_fees = 0.0;
		double m_market_impact = 0.0;
		double m_net_cost = 0.0;
		double m_crypto_amount = 0.0;
		double m_maker_ratio = 0.0;
		double m_volatility = 0.0;
		double m_processing_time = 0.0;
	};
}