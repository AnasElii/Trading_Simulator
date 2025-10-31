#pragma once
#include <QObject>
#include "IQuantCalculatorAPI.h"

namespace Quant
{
	Q_NAMESPACE
	
	Q_ENUM_NS(ORDER_TYPE)
	Q_ENUM_NS(FEE_TIER)

	enum class SPOT_ASSET
	{
		BTC_USDT_SWAP,
		ETH_USDT_SWAP,
		SOL_USDT_SWAP,
		XRP_USDT_SWAP,
		ADA_USDT_SWAP,
		DOT_USDT_SWAP,
	};
	Q_ENUM_NS(SPOT_ASSET)
	Q_ENUM_NS(EXCHANGE_API)

	// Helper class for enum conversion
	class EnumConverter
	{
	public:
		static QString OrderTypeToString(ORDER_TYPE type);
		static ORDER_TYPE StringToOrderType(const QString& str);
	public:
		static QString FeeTierToString(FEE_TIER tier);
		static FEE_TIER StringToFeeTier(const QString& str);
	public:
		static QString SpotAssetToString(SPOT_ASSET asset);
		static SPOT_ASSET StringToSpotAsset(const QString& str);
	public:
		static QString ExchangeToString(EXCHANGE_API exchange);
		static EXCHANGE_API StringToExchange(const QString& str);
	public:
		static QStringList GetAllOrderType();
		static QStringList GetAllFeeTierType();
		static QStringList GetAllSpotAssets();
		static QStringList GetAllExchanges();
	};

	class QuantInputHandler : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QString selected_exchange READ SelectedExchangeString WRITE SetSelectedExchangeString NOTIFY SelectedExchangeChanged)
		Q_PROPERTY(QString selected_asset READ SelectedAssetString WRITE SetSelectedAssetString NOTIFY SelectedAssetChanged)
		Q_PROPERTY(QString order_type READ OrderTypeString WRITE SetOrderTypeString NOTIFY OrderTypeChanged)
		Q_PROPERTY(QString fee_tier READ FeeTierString WRITE SetFeeTierString NOTIFY FeeTierChanged)
		Q_PROPERTY(double quantity READ Quantity WRITE SetQuantity NOTIFY QuantityChanged)
		Q_PROPERTY(double usd_amount READ USDAmount WRITE SetUSDAmount NOTIFY USDAmountChanged)
		Q_PROPERTY(double volatility READ Volatility WRITE SetVolatility NOTIFY VolatilityChanged)
		Q_PROPERTY(bool volatility_enabled READ VolatilityEnabled WRITE SetVolatilityEnabled NOTIFY VolatilityEnabledChanged)

		// Expose the enum types to QML
		Q_PROPERTY(QStringList available_order_types READ AvailableOrderTypes CONSTANT)
		Q_PROPERTY(QStringList available_fee_tiers READ AvailableFeeTiers CONSTANT)
		Q_PROPERTY(QStringList available_spot_assets READ AvailableSpotAssets CONSTANT)
		Q_PROPERTY(QStringList available_exchanges READ AvailableExchanges CONSTANT)

	public:
		explicit QuantInputHandler(QObject* parent = nullptr);

	public:
		QString SelectedExchangeString() const { return EnumConverter::ExchangeToString(m_selected_exchange); }
		QString SelectedAssetString() const { return EnumConverter::SpotAssetToString(m_selected_asset); }
		QString OrderTypeString() const { return EnumConverter::OrderTypeToString(m_order_type); }
		QString FeeTierString() const { return EnumConverter::FeeTierToString(m_fee_tier); }

	public:
		EXCHANGE_API SelectedExchange() const { return m_selected_exchange; }
		SPOT_ASSET SelectedAsset() const { return m_selected_asset; }
		ORDER_TYPE OrderType() const { return m_order_type; }
		FEE_TIER FeeTier() const { return m_fee_tier; }

	public:
		double Quantity() const { return m_quantity; }
		double USDAmount() const { return m_usd_amount; }
		double Volatility() const { return m_volatility; }
		bool VolatilityEnabled() const { return m_volatility_enabled; }

	public:
		QStringList AvailableExchanges() const { return EnumConverter::GetAllExchanges(); }
		QStringList AvailableSpotAssets() const { return EnumConverter::GetAllSpotAssets(); }
		QStringList AvailableOrderTypes() const { return EnumConverter::GetAllOrderType(); }
		QStringList AvailableFeeTiers() const { return EnumConverter::GetAllFeeTierType(); }

	public:
		void SetSelectedExchange(EXCHANGE_API exchange);
		void SetSelectedAsset(SPOT_ASSET asset);
		void SetOrderType(ORDER_TYPE order_type);
		void SetFeeTier(FEE_TIER fee_tier);
		void SetVolatilityEnabled(bool enabled);

	public:
		void SetQuantity(double quantity);
		void SetUSDAmount(double usd_amount);
		void SetVolatility(double volatility);

	public:
		void SetSelectedExchangeString(const QString& exchange);
		void SetSelectedAssetString(const QString& asset);
		void SetOrderTypeString(const QString& order_type);
		void SetFeeTierString(const QString& fee_tier);

	public slots:
		void CalculateOutput();

	signals:
		void SelectedExchangeChanged();
		void SelectedAssetChanged();
		void OrderTypeChanged();
		void QuantityChanged();
		void USDAmountChanged();
		void VolatilityChanged();
		void FeeTierChanged();
		void VolatilityEnabledChanged();

	signals:
		void CalculationPerformed();

	private:
		EXCHANGE_API m_selected_exchange = EXCHANGE_API::OKX;
		SPOT_ASSET m_selected_asset = SPOT_ASSET::BTC_USDT_SWAP;
		ORDER_TYPE m_order_type = ORDER_TYPE::MARKET;
		double m_quantity = 1.0;
		double m_usd_amount = 100.0;
		double m_volatility = 0.01;
		FEE_TIER m_fee_tier = FEE_TIER::VIP_0;
		bool m_volatility_enabled = false;
	};
}