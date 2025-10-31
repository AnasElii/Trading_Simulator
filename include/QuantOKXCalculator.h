#pragma once
#include "IQuantCalculatorAPI.h"
#include <QPair>
#include <QMap>


namespace Quant
{
	class QuantOKXCalculator : public IQuantCalculatorAPI
	{
	private:
		typedef QPair<double, double> price_amount_pair;
		typedef QMap<FEE_TIER, price_amount_pair> fee_rate_map;

		fee_rate_map m_fee_rates;
		State m_state;

		static bool m_is_volatility_enabled;
		static double m_process_time_ms;


	public:
		bool isSelected(EXCHANGE_API) const override;

	public:
		double CalculateVolatilityFromOrderbook(const QVariantList& bids, const QVariantList& asks) override;
		double CalculateFees(double order_amount, FEE_TIER tier, bool is_taker) override;
		double CalculateMarketOrderCost(double quantity, const QVariantList& orderbook) override;
		double CalculateSlippage(double quantity, const QVariantList& bids, const QVariantList& asks, ORDER_TYPE order_type, ORDER_SIDE side) override;
		double CalculateMarketImpact(double quantity, double volatility, const QVariantList& bids, const QVariantList& asks) override;
		double CalculateMakerRatio(const QVariantList& bids, const QVariantList& asks) override;

	public:
		static bool isVolatilityEnabled() { return m_is_volatility_enabled; }
		static double GetProcessingTime() { return m_process_time_ms; }

		static void SetVolatilityEnabled(bool enabled) { m_is_volatility_enabled = enabled; }
		static void SetProcessingTime(double elapsed_ms) { m_process_time_ms = elapsed_ms; }

	private:
		void InitializeFeeRates() override;
		QString GetExchangeName() const override { return "OKX"; }
		EXCHANGE_API GetAPIType() const override { return EXCHANGE_API::OKX; }
		State GetState() const override { return m_state; }

	public:
		static QuantOKXCalculator& get();

	private:
		QuantOKXCalculator() { InitializeFeeRates(); }
		price_amount_pair GetFeeRates(FEE_TIER tier) const;

	};
}