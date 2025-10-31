#include "QuantInputHandler.h"

#include <QDebug>

#include "QuantOKXCalculator.h"

namespace Quant
{
	// Enum converter implementation
	QString EnumConverter::OrderTypeToString(ORDER_TYPE type)
	{
		switch (type)
		{
		case ORDER_TYPE::MARKET: return "market";
		case ORDER_TYPE::LIMIT: return "limit";
		case ORDER_TYPE::STOP_LOSS: return "stop_loss";
		case ORDER_TYPE::TAKE_PROFIT: return "take_profit";
		case ORDER_TYPE::TRAILING_STOP_MARKET: return "trailing_stop_market";
		case ORDER_TYPE::TRAILING_STOP_LIMIT: return "trailing_stop_limit";
		default: return "Unknown";
		}
	}

	ORDER_TYPE EnumConverter::StringToOrderType(const QString& str)
	{
		if (str == "limit") return ORDER_TYPE::LIMIT;
		if (str == "stop_loss") return ORDER_TYPE::STOP_LOSS;
		if (str == "take_profit") return ORDER_TYPE::TAKE_PROFIT;
		if (str == "trailing_stop_market") return ORDER_TYPE::TRAILING_STOP_MARKET;
		if (str == "trailing_stop_limit") return ORDER_TYPE::TRAILING_STOP_LIMIT;
		return ORDER_TYPE::MARKET; // Default case
	}

	QString EnumConverter::FeeTierToString(FEE_TIER tier)
	{
		switch (tier)
		{
		case FEE_TIER::VIP_0: return "VIP 0";
		case FEE_TIER::VIP_1: return "VIP 1";
		case FEE_TIER::VIP_2: return "VIP 2";
		case FEE_TIER::VIP_3: return "VIP 3";
		case FEE_TIER::VIP_4: return "VIP 4";
		case FEE_TIER::VIP_5: return "VIP 5";
		case FEE_TIER::VIP_6: return "VIP 6";
		case FEE_TIER::VIP_7: return "VIP 7";
		case FEE_TIER::VIP_8: return "VIP 8";
		case FEE_TIER::VIP_9: return "VIP 9";
		default: return "Unknown";
		}
	}

	FEE_TIER EnumConverter::StringToFeeTier(const QString& str)
	{
		if (str == "VIP 1") return FEE_TIER::VIP_1;
		if (str == "VIP 2") return FEE_TIER::VIP_2;
		if (str == "VIP 3") return FEE_TIER::VIP_3;
		if (str == "VIP 4") return FEE_TIER::VIP_4;
		if (str == "VIP 5") return FEE_TIER::VIP_5;
		if (str == "VIP 6") return FEE_TIER::VIP_6;
		if (str == "VIP 7") return FEE_TIER::VIP_7;
		if (str == "VIP 8") return FEE_TIER::VIP_8;
		if (str == "VIP 9") return FEE_TIER::VIP_9;
		return FEE_TIER::VIP_0;
	}

	QString EnumConverter::SpotAssetToString(SPOT_ASSET asset)
	{
		switch (asset)
		{
		case SPOT_ASSET::BTC_USDT_SWAP: return "BTC-USDT-SWAP";
		case SPOT_ASSET::ETH_USDT_SWAP: return "ETH-USDT-SWAP";
		case SPOT_ASSET::SOL_USDT_SWAP: return "SOL-USDT-SWAP";
		case SPOT_ASSET::XRP_USDT_SWAP: return "XRP-USDT-SWAP";
		case SPOT_ASSET::ADA_USDT_SWAP: return "ADA-USDT-SWAP";
		case SPOT_ASSET::DOT_USDT_SWAP: return "DOT-USDT-SWAP";
		default: return "Unknown";
		}
	}

	SPOT_ASSET EnumConverter::StringToSpotAsset(const QString& str)
	{
		if (str == "ETH-USDT-SWAP") return SPOT_ASSET::ETH_USDT_SWAP;
		if (str == "SOL-USDT-SWAP") return SPOT_ASSET::SOL_USDT_SWAP;
		if (str == "XRP-USDT-SWAP") return  SPOT_ASSET::XRP_USDT_SWAP;
		if (str == "ADA-USDT-SWAP") return  SPOT_ASSET::ADA_USDT_SWAP;
		if (str == "DOT-USDT-SWAP") return  SPOT_ASSET::DOT_USDT_SWAP;
		return SPOT_ASSET::BTC_USDT_SWAP;
	}

	QString EnumConverter::ExchangeToString(EXCHANGE_API exchange)
	{
		switch (exchange)
		{
		case EXCHANGE_API::OKX: return "OKX";
		case EXCHANGE_API::BINANCE: return "BINANCE";
		case EXCHANGE_API::COINBASE: return "COINBASE";
		case EXCHANGE_API::MEXC: return "MEXC";
		default: return "Unknown";
		}
	}

	EXCHANGE_API EnumConverter::StringToExchange(const QString& str)
	{
		if (str == "BINANCE") return EXCHANGE_API::BINANCE;
		if (str == "COINBASE") return EXCHANGE_API::COINBASE;
		if (str == "MEXC") return EXCHANGE_API::MEXC;
		return EXCHANGE_API::OKX;
	}

	QStringList EnumConverter::GetAllExchanges()
	{
		return QStringList() << "OKX" << "BINANCE" << "COINBASE" << "MEXC";
	}

	QStringList EnumConverter::GetAllOrderType()
	{
		return QStringList() << "market" << "limit" << "stop_loss" << "take_profit" << "trailing_stop_market" << "trailing_stop_limit";
	}

	QStringList EnumConverter::GetAllSpotAssets()
	{
		return QStringList() << "BTC-USDT-SWAP" << "ETH-USDT-SWAP" << "SOL-USDT-SWAP"
			<< "XRP-USDT-SWAP" << "ADA-USDT-SWAP" << "DOT-USDT-SWAP";
	}

	QStringList EnumConverter::GetAllFeeTierType()
	{
		return QStringList() << "VIP 0" << "VIP 1" << "VIP 2" << "VIP 3" << "VIP 4" << "VIP 5";
	}

	// QuantInputHandler implementation
	QuantInputHandler::QuantInputHandler(QObject* parent) : QObject(parent)
	{
	}

	void QuantInputHandler::SetSelectedExchange(EXCHANGE_API exchange)
	{
		if (m_selected_exchange == exchange)
			return;

		m_selected_exchange = exchange;
		emit SelectedExchangeChanged();
	}

	void QuantInputHandler::SetSelectedExchangeString(const QString& exchange)
	{
		SetSelectedExchange(EnumConverter::StringToExchange(exchange));
	}

	void QuantInputHandler::SetSelectedAsset(SPOT_ASSET asset)
	{
		if (m_selected_asset == asset)
			return;

		m_selected_asset = asset;
		emit SelectedAssetChanged();
	}

	void QuantInputHandler::SetSelectedAssetString(const QString& asset)
	{
		SetSelectedAsset(EnumConverter::StringToSpotAsset(asset));
	}

	void QuantInputHandler::SetOrderType(ORDER_TYPE order_type)
	{
		if (order_type == m_order_type)
			return;

		m_order_type = order_type;
		emit OrderTypeChanged();
	}

	void QuantInputHandler::SetOrderTypeString(const QString& order_type)
	{
		SetOrderType(EnumConverter::StringToOrderType(order_type));
	}

	void QuantInputHandler::SetFeeTier(FEE_TIER fee_tier)
	{
		if (fee_tier == m_fee_tier)
			return;

		m_fee_tier = fee_tier;
		qDebug() << EnumConverter::FeeTierToString(m_fee_tier);

		emit FeeTierChanged();
	}

	void QuantInputHandler::SetFeeTierString(const QString& fee_tier)
	{
		SetFeeTier(EnumConverter::StringToFeeTier(fee_tier));
	}

	void QuantInputHandler::SetQuantity(double quantity)
	{
		if (quantity == m_quantity)
			return;

		m_quantity = quantity;
		emit QuantityChanged();
	}

	void QuantInputHandler::SetUSDAmount(double usd_amount)
	{
		if (usd_amount == m_usd_amount)
			return;

		m_usd_amount = usd_amount;
		emit USDAmountChanged();
	}

	void QuantInputHandler::SetVolatility(double volatility)
	{
		if (volatility == m_volatility)
			return;

		m_volatility = volatility;
		emit VolatilityChanged();
	}

	void QuantInputHandler::SetVolatilityEnabled(bool enabled)
	{
		if (enabled == m_volatility_enabled)
			return;

		qDebug() << "Volatility state: " << enabled;
		m_volatility_enabled = enabled;
		QuantOKXCalculator::SetVolatilityEnabled(enabled);

		emit VolatilityEnabledChanged();
	}


	void QuantInputHandler::CalculateOutput()
	{
		emit CalculationPerformed();
	}

}
