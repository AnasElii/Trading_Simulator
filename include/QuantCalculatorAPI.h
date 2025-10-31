#pragma once
#include <QObject>
#include <QDebug>

#include "IQuantCalculatorAPI.h"
#include "QuantOKXCalculator.h"
#include "QuantInputHandler.h"
#include "QuantOrderbook.h"

namespace {
	void SetCalculatorVolatilityEnabled(bool enabled) {
		Quant::QuantOKXCalculator::SetVolatilityEnabled(enabled);
	}
}

namespace Quant
{
	class QuantCalculatorAPI : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(double calculate_volatility_from_orderbook READ CalculateVolatilityFromOrderbook NOTIFY CalculationUpdated)
		Q_PROPERTY(double calculate_fees READ CalculateFees NOTIFY CalculationUpdated)
		Q_PROPERTY(double calculate_market_order_cost READ CalculateMarketOrderCost NOTIFY CalculationUpdated)
		Q_PROPERTY(double calculate_slippage READ CalculateSlippage NOTIFY CalculationUpdated)
		Q_PROPERTY(double calculate_market_impact READ CalculateMarketImpact NOTIFY CalculationUpdated)
		Q_PROPERTY(double calculate_maker_ratio READ CalculateMakerRatio NOTIFY CalculationUpdated)

		Q_PROPERTY(QObject* result READ GetResult CONSTANT)

	public:
		QuantCalculatorAPI(QObject* parent = nullptr);
		~QuantCalculatorAPI();

	public:
		IQuantCalculatorAPI* selectedExchange();

	public:
		void SetInputHandler(QuantInputHandler* input_handler);
		void SetOrderbook(QuantOrderbook* orderbook);

	public:
		double CalculateVolatilityFromOrderbook() const { return m_volatility; }
		double CalculateFees() const { return m_fees; }
		double CalculateMarketOrderCost() const { return m_market_order_cost; }
		double CalculateSlippage() const { return m_slippage; }
		double CalculateMarketImpact() const { return m_market_impact; }
		double CalculateMakerRatio() const { return m_maker_ratio; }

		QObject* GetResult() const { return m_result; }

		double CalculateCryptoForFixedUSD(double usd_amount, const QVariantList& orderbook);

	public slots:
		void Calculate();

	private slots:
		void OnOrderbookUpdated();
		void OnInputChanged();

	signals:
		void CalculationUpdated();

	private:
		double m_volatility = 0.0;
		double m_fees = 0.0;
		double m_market_order_cost = 0.0;
		double m_slippage = 0.0;
		double m_market_impact = 0.0;
		double m_maker_ratio = 0.0;

		QObject* m_result = nullptr;

	private:
		QuantInputHandler* m_input_handler = nullptr;
		QuantOrderbook* m_orderbook = nullptr;

		IQuantCalculatorAPI* m_calculator_interface = nullptr;
	};
}
