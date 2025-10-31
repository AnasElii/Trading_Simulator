#pragma once
#include <QString>
#include <QStringList>
#include <QVariantList>

namespace Quant
{
	enum class ORDER_TYPE
	{
		MARKET,
		LIMIT,
		STOP_LOSS,
		TAKE_PROFIT,
		TRAILING_STOP_MARKET,
		TRAILING_STOP_LIMIT,
	};

	enum class ORDER_SIDE
	{
		BUY,
		SELL,
	};

	enum class FEE_TIER
	{
		VIP_0,
		VIP_1,
		VIP_2,
		VIP_3,
		VIP_4,
		VIP_5,
		VIP_6,
		VIP_7,
		VIP_8,
		VIP_9
	};

	enum class EXCHANGE_API
	{
		NONE,
		OKX,
		BINANCE,
		COINBASE,
		MEXC
	};

	class IQuantCalculatorAPI
	{
	public:
		struct State
		{
			bool connected = false;
			bool authenticated = false;
			QString last_error = QString();

			State() = default;
		};
		
	public:
		virtual ~IQuantCalculatorAPI() = default;

	public:
		virtual bool isSelected(EXCHANGE_API) const { return true; }

	public:
		virtual double CalculateVolatilityFromOrderbook(const QVariantList& bids, const QVariantList& asks) { return 0.0; };
		virtual double CalculateFees(double order_amount, FEE_TIER tier, bool is_taker) { return 0.0; };
		virtual double CalculateMarketOrderCost(double quantity, const QVariantList& orderbook) { return 0.0; };
		virtual double CalculateSlippage(double quantity, const QVariantList& bids, const QVariantList& asks, ORDER_TYPE order_type, ORDER_SIDE side) { return 0.0; };
		virtual double CalculateMarketImpact(double quantity, double volatility, const QVariantList& bids, const QVariantList& asks) { return 0.0; };
		virtual double CalculateMakerRatio(const QVariantList& bids, const QVariantList& asks) { return 0.0; };

	private:
		virtual void InitializeFeeRates() {};
		virtual QString GetExchangeName() const { return ""; };
		virtual EXCHANGE_API GetAPIType() const { return EXCHANGE_API::NONE; };
		virtual State GetState() const { return {}; }
	};

	// Factory functions for getting the right API
	EXCHANGE_API GetCurrentExchangeAPI();
	bool SetExchangeAPI(EXCHANGE_API api);
	bool SetExchangeAPI(IQuantCalculatorAPI* api);

	IQuantCalculatorAPI& GetAPIInterface();
	IQuantCalculatorAPI::State GetAPIState();
}