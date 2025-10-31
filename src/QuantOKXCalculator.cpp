#include "QuantOKXCalculator.h"



namespace Quant
{
	bool QuantOKXCalculator::m_is_volatility_enabled = false;
	double QuantOKXCalculator::m_process_time_ms = 0.0;

	namespace
	{
		int orderbook_depth_num = 10; // Number of levels to consider for depth calculation

		struct ItemType
		{
			static constexpr const char* price = "price";
			static constexpr const char* amount = "amount";
		};

		void BestEntries(QVariantList& entries, bool is_bids = true)
		{
			// Sort the entries based on price
			std::sort(entries.begin(), entries.end(),
				[is_bids](const QVariant& a, const QVariant& b)
				{
					double price_a = a.toMap()[ItemType::price].toDouble();
					double price_b = b.toMap()[ItemType::price].toDouble();

					// Sort by price in descending order for bids and ascending for asks
					return (is_bids) ? (price_a > price_b) : (price_a < price_b);
				});
		}

		void BestEntryPrice(const QVariantList& entries, double& best_price, bool is_bid = true)
		{
			for (const QVariant entry : entries)
			{
				double entry_price = entry.toMap()[ItemType::price].toDouble();
				if (is_bid)
				{
					if (entry_price > best_price)
						best_price = entry_price;
				}
				else
				{
					if (entry_price < best_price)
						best_price = entry_price;
				}

			}
		}
	}

	bool QuantOKXCalculator::isSelected(EXCHANGE_API selected_exchange) const
	{
		return selected_exchange == EXCHANGE_API::OKX;
	}

	void QuantOKXCalculator::InitializeFeeRates()
	{
		// Initialize OKX fee rates based on documentation (maker, taker) in percentage
		m_fee_rates[FEE_TIER::VIP_0] = qMakePair(0.0800, 0.1000);
		m_fee_rates[FEE_TIER::VIP_1] = qMakePair(0.0700, 0.0900);
		m_fee_rates[FEE_TIER::VIP_2] = qMakePair(0.0650, 0.0800);
		m_fee_rates[FEE_TIER::VIP_3] = qMakePair(0.0600, 0.0750);
		m_fee_rates[FEE_TIER::VIP_4] = qMakePair(0.0550, 0.0700);
		m_fee_rates[FEE_TIER::VIP_5] = qMakePair(0.0500, 0.0600);
		m_fee_rates[FEE_TIER::VIP_6] = qMakePair(0.0450, 0.0550);
		m_fee_rates[FEE_TIER::VIP_7] = qMakePair(0.0400, 0.0500);
		m_fee_rates[FEE_TIER::VIP_8] = qMakePair(0.0350, 0.0450);
		m_fee_rates[FEE_TIER::VIP_9] = qMakePair(0.0300, 0.0400);
	}

	QuantOKXCalculator::price_amount_pair QuantOKXCalculator::GetFeeRates(FEE_TIER tier) const
	{
		return m_fee_rates.value(tier, m_fee_rates[FEE_TIER::VIP_0]);
	}

	/**
	 * Micro‑Intraday Volatility (Order Book Only)
	 * volatility = (spread * 2.0 + imbalance * 1.5) * 100.0
	 *
	 * This simplified volatility metric is derived solely from live order‑book snapshots.
	 * It addresses the core quantitative trading requirements:
	 *  - Expected slippage estimation
	 *  - Market‑impact modeling
	 *  - Maker vs. taker order proportion analysis
	 *
	 * Calculation formula:
	 *   volatility = (spread * 2.0 + imbalance * 1.5) * 100.0
	 *
	 * Rationale:
	 *   Traditional volatility measures (like return-based or Parkinson's) require time-series
	 *   data over multiple periods. Since this implementation only has a single orderbook
	 *   snapshot, it provides an alternative estimate derived from current market microstructure.
	 *
	 * Note: In a full production setting, you’d typically augment this with:
	 *  - Historical volatility
	 *  - Implied volatility
	 *  - Traditional intraday volatility
	 *
	 * Those broader measures improve risk assessment, signal trading opportunities,
	 * and capture overall market uncertainty beyond what order‑book data alone can show.
	 *
	 * @param bids  The bids order list.
	 * @param asks  The bids order list..
	 * @return      The applicable volatility (as a decimal, e.g. 0.0010 for 0.10%).
	*/
	double QuantOKXCalculator::CalculateVolatilityFromOrderbook(const QVariantList& bids, const QVariantList& asks)
	{
		if (bids.isEmpty() || asks.isEmpty())
			return 0.0;

		auto new_bids = bids;
		auto new_asks = asks;

		// Sort bids and asks to get the best prices
		BestEntries(new_bids, true);
		BestEntries(new_asks, false);

		// get best bid and ask prices
		double best_bid = new_bids.isEmpty() ? 0.0 : new_bids.first().toMap()[ItemType::price].toDouble();
		double best_ask = new_asks.isEmpty() ? std::numeric_limits<double>::max() : new_asks.first().toMap()[ItemType::price].toDouble();

		if (best_bid <= 0 || best_ask <= 0 || best_ask == std::numeric_limits<double>::max())
			return 0.0;

		// Calculate mid price
		double mid_price = (best_bid + best_ask) / 2.0;

		// Calculate bid-ask spread as percentage of mid-price (implicit cost )
		double spread = (best_ask - best_bid) / mid_price;

		// Calculate order-book depth (sum of top 10 bids and asks)
		double bid_depth = 0.0;
		double ask_depth = 0.0;

		int levels = qMin(orderbook_depth_num, new_bids.size());
		for (int i = 0; i < levels; i++)
		{
			bid_depth += new_bids[i].toMap()[ItemType::amount].toDouble();
		}

		levels = qMin(orderbook_depth_num, new_asks.size());
		for (int i = 0; i < levels; i++)
		{
			ask_depth += new_asks[i].toMap()[ItemType::amount].toDouble();
		}

		/**
		* Measure order books imbalance supply-demand asymmetry
		* Measure the relative difference between the buying pressure (bid_depth) and selling pressure (ask_depth)
		* The result always between 0 and 1.
		*	- 0 means prefectly balanced order book (bid_depth == ask_depth)
		*	- 1 means all orders are on one side (bid_depth == 0 or ask_depth == 0)
		* High imbalancec often correlates with directional price movement and volatility
		*
		* When the imbalance value is closer to 0:
		*	•	This generally indicates a stable, liquid market with good price consensus
		*
		* When the imbalance value is closer to 1:
		*	•	The order book is heavily one-sided, which can indicate potential volatility
		*	•	Two scenarios can cause this:
		*		1.	If bid_depth ≈ 0: Few buy orders remain, suggesting either strong bearish sentiment or that buyers have withdrawn from the market
		*		2.	If ask_depth ≈ 0: Few sell orders remain, suggesting either strong bullish sentiment or that sellers have withdrawn from the market
		*/
		// Calculate order imbalance (measures supply/demand asymmetry)
		double imbalance = qAbs(bid_depth - ask_depth) / (bid_depth + ask_depth);

		// Combine factors for volatility estimate
		double volatility = (spread * 2.0 + imbalance * 1.5) * 100.0; // Convert to percentage

		return volatility;
	}

	/**
	 * Simulated Fee Calculator for OKX
	 *
	 * Calculates the maker or taker fee rate based on a simplified 30‑day volume tier.
	 * In production, you would:
	 *  - Analyze the user’s actual trading volume over the past 30 days
	 *  - Determine their fee tier from real data
	 *
	 * Fee tiers (30‑day BTC volume):
	 * ┌────────┬────────────┬────────────┬──────────────────────┐
	 * │ Tier   │ Taker Fee  │ Maker Fee  │ 30d Volume (BTC)     │
	 * ├────────┼────────────┼────────────┼──────────────────────┤
	 * │ VIP 0  │ 0.10%      │ 0.08%      │ < 5                  │
	 * │ VIP 1  │ 0.08%      │ 0.06%      │ ≥ 5                  │
	 * │ VIP 2  │ 0.07%      │ 0.05%      │ ≥ 20                 │
	 * │ VIP 3  │ 0.06%      │ 0.04%      │ ≥ 50                 │
	 * │ VIP 4  │ 0.05%      │ 0.03%      │ ≥ 200                │
	 * │ VIP 5  │ 0.04%      │ 0.02%      │ ≥ 500                │
	 * └────────┴────────────┴────────────┴──────────────────────┘
	 *
	 * @param order_amount  The size of the order (unused in this simulation).
	 * @param tier          The fee tier (VIP 0–5) based on 30‑day volume.
	 * @param is_taker      True for taker fees, false for maker fees.
	 * @return              The applicable fee rate (as a decimal, e.g. 0.0010 for 0.10%).
	 */
	double QuantOKXCalculator::CalculateFees(double order_amount, FEE_TIER tier, bool is_taker)
	{
		// Get fee rate
		price_amount_pair rates = GetFeeRates(tier);

		// Apply Maker/Taker fee rate 
		double fee_rate = is_taker ? rates.second : rates.first;

		// Return fee as percentage
		return fee_rate;
	}

	/**
	 * Volume-Weighted Average Price (VWAP) Market Order Cost Simulator
	 *
	 * Simulates the execution of a market order against the orderbook and calculates
	 * the average execution price per unit, considering order book depth limitations.
	 * This implements a price-time priority order matching algorithm similar to real exchanges.
	 *
	 * Algorithm:
	 *  1. Start with the full order quantity to be executed
	 *  2. Walk through each level of the orderbook from best price to worst
	 *  3. At each level:
	 *     - Execute as much as possible given available liquidity
	 *     - Calculate cost for execution at this level (price × executed amount)
	 *     - Accumulate total cost and reduce remaining quantity
	 *  4. Continue until the entire order is filled or orderbook is exhausted
	 *  5. Return the average cost per unit (total cost / requested quantity)
	 *
	 * Note: In real markets, this calculation represents the expected execution price
	 * before placing the order. Actual execution may vary due to:
	 *  - Market dynamics (orderbook changes during execution)
	 *  - Other traders competing for same liquidity
	 *  - Hidden orders not visible in the public orderbook
	 *
	 * @param quantity 
	 * @param orderbook 
	 * @return 
	 */
	double QuantOKXCalculator::CalculateMarketOrderCost(double quantity, const QVariantList& orderbook)
	{
		double total_cost = 0.0;
		double remaining_quantity = quantity;

		// Loop through the orderbook levels until it filled
		for (int i = 0; i<orderbook.size() && remaining_quantity > 0; i++)
		{
			QVariantMap level = orderbook[i].toMap();
			double price = level[ItemType::price].toDouble();
			double available_amount = level[ItemType::amount].toDouble();

			double executed_amount = qMin(remaining_quantity, available_amount);
			double cost_at_this_level = executed_amount * price;

			total_cost += cost_at_this_level;
			remaining_quantity -= executed_amount;
		}

		// Return cost per unit
		return (quantity > 0) ? total_cost / quantity : 0.0;
	}

	/**
	 * Market Order Slippage Calculator
	 *
	 * Calculates the price slippage for a market order - the price difference between
	 * the expected execution price and the best available price, expressed as a percentage.
	 * Slippage represents the implicit cost of large orders consuming multiple price levels.
	 *
	 * Algorithm:
	 *  1. For market orders only (returns 0 for limit orders)
	 *  2. Determine which side of the book to consider (asks for buys, bids for sells)
	 *  3. Calculate the actual weighted average execution price using CalculateMarketOrderCost
	 *  4. Determine the reference price (best available price on relevant side)
	 *  5. Calculate percentage difference between execution price and reference price
	 *  6. For buy orders: slippage = ((execution_price - best_ask) / best_ask) * 100%
	 *     For sell orders: slippage = ((best_bid - execution_price) / best_bid) * 100%
	 *
	 * A positive slippage percentage indicates adverse price movement:
	 *  - For buys: paying more than the best ask
	 *  - For sells: receiving less than the best bid
	 * 
	 * @param quantity 
	 * @param bids 
	 * @param asks 
	 * @param order_type 
	 * @param side 
	 * @return 
	 */
	double QuantOKXCalculator::CalculateSlippage(double quantity, const QVariantList& bids, const QVariantList& asks, ORDER_TYPE order_type, ORDER_SIDE side)
	{
		if (order_type != ORDER_TYPE::MARKET)
			return 0.0;

		// Calculate theoretical execution price
		// When buying, we consider the ask side. when selling, we consider the bid side. 
		double market_cost = CalculateMarketOrderCost(quantity, (side == ORDER_SIDE::BUY) ? asks : bids);

		// Get reference price (best bid or ask)
		double reference_price = (side == ORDER_SIDE::BUY) ? asks.first().toMap()[ItemType::price].toDouble() : bids.first().toMap()[ItemType::price].toDouble();

		// Calculate slippage as percentage
		double slippage = 0.0;
		if (side == ORDER_SIDE::BUY)
			slippage = ((market_cost - reference_price) / reference_price) * 100.0;
		else
			slippage = ((reference_price - market_cost) / reference_price) * 100.0;

		return qMax(0.0, slippage);
	}

	/**
	 * Almgren-Chriss Market Impact Model
	 *
	 * Calculates the expected price impact of an order using a simplified version of
	 * the Almgren-Chriss model, which considers volatility, size, and market liquidity.
	 * This predicts how much a trader's own order will move the market against them.
	 *
	 * Formula:
	 *   impact = c * σ * sqrt(Q/V) * (Q/ADV)
	 *   where:
	 *     - c: market impact coefficient (0.1 in this implementation)
	 *     - σ: market volatility (decimal form, e.g. 0.02 for 2%)
	 *     - Q: order quantity
	 *     - V: current market depth
	 *     - ADV: average daily volume
	 *
	 * Model characteristics:
	 *  - Scales with square root of quantity (reflecting diminishing impact)
	 *  - Linear relationship with volatility (more volatile markets = more impact)
	 *  - Considers current market depth for liquidity assessment
	 *  - Compares order size to average daily volume for relative size assessment
	 *
	 * Simplifications in this implementation:
	 *  - Uses instantaneous market depth as a liquidity proxy
	 *  - Estimates ADV as 24× current market depth (in production, use historical data)
	 *  - Fixed impact coefficient (in production, calibrate based on market data)
	 *
	 * Without even having a look at the documentation the code clear for anybody to understand the logic immediately
	 * 
	 * @param quantity 
	 * @param volatility 
	 * @param bids 
	 * @param asks 
	 * @return 
	 */
	double QuantOKXCalculator::CalculateMarketImpact(double quantity, double volatility, const QVariantList& bids, const QVariantList& asks)
	{
		// When user enable the volatility slider, use the provided value
		// Otherwise, calculate the volatility from the orderbook
		double effective_volatility = volatility;
		if (!m_is_volatility_enabled)
		{
			effective_volatility = CalculateVolatilityFromOrderbook(bids, asks);
		}

		// Simplified Almgre-chriss market impact model
		double sigma = effective_volatility / 100.0; // Convert percentage to decimal

		// Calculate total market depth volume
		double market_depth = 0.0;
		for (const QVariant& item : asks)
			market_depth += item.toMap()[ItemType::amount].toDouble();

		// Placeholder for average daily volume (ADV)- this should be replaced with actual ADV data
		// In a real implementation, this would be fetched from a reliable source with historical data
		double average_daily_volume = market_depth * 24.0; // Assuming 24 hours of trading

		// Impact coefficient
		const double c = 0.1; // This is a constant that can be adjusted based on empirical data

		// Calculate impact
		double impact = 0.0;
		if (market_depth > 0 && average_daily_volume > 0)
			impact = c * sigma * qSqrt(quantity / market_depth) * (quantity / average_daily_volume);

		return impact;
	}

	/**
	 * Apply logistic regression formula
	 *
	 * Logistic Function:
	 *	•	When x is large positive → result approaches 1.0 (high maker probability)
	 *	•	When x is large negative → result approaches 0.0 (low maker probability)
	 *	•	When x = 0 → result equals exactly 0.5 (50/50 maker/taker conditions)
	 *
	 *		1.0 |     -------x
	 *		    |    /
	 *		    |   /
	 *		0.5 |  /
	 *		    | /
	 *		    |/
	 *		0.0 +----------------
	 *		   -5  -2  0  2   5
	 *
	 * This S-shaped curve is what makes logistic regression excellent for binary classification problems (like "is this a maker-favorable market or not?").
	 *
	 * @param bids 
	 * @param asks 
	 * @return 
	 */
	double QuantOKXCalculator::CalculateMakerRatio(const QVariantList& bids, const QVariantList& asks)
	{
		if (bids.isEmpty() || asks.isEmpty())
			return 0.0;

		auto new_bids = bids;
		auto new_asks = asks;

		// Sort bids and asks to get the best prices
		BestEntries(new_bids, true);
		BestEntries(new_asks, false);

		// get best bid and ask prices
		double best_bid = new_bids.isEmpty() ? 0.0 : new_bids.first().toMap()[ItemType::price].toDouble();
		double best_ask = new_asks.isEmpty() ? std::numeric_limits<double>::max() : new_asks.first().toMap()[ItemType::price].toDouble();

		if (best_bid <= 0 || best_ask <= 0 || best_ask == std::numeric_limits<double>::max())
			return 0.0;

		// Calculate mid price
		double mid_price = (best_bid + best_ask) / 2.0;

		// Calculate bid-ask spread as percentage of mid-price (implicit cost )
		double spread = (best_ask - best_bid) / mid_price;

		// Calculate order-book depth (sum of top 10 bids and asks)
		double bid_depth = 0.0;
		double ask_depth = 0.0;

		for (const auto entity : new_bids)
		{
			bid_depth += entity.toMap()[ItemType::amount].toDouble();
		}

		for (const auto entity : new_asks)
		{
			ask_depth += entity.toMap()[ItemType::amount].toDouble();
		}

		double imbalance = qAbs(bid_depth - ask_depth) / (bid_depth + ask_depth);

		/**
		 * Logistic function: 1 / (1 + e^(-x)) <-- ()
		 * where x = β₀ + β₁*spread + β₂*imbalance <-- ()
		 *
		 * Model interpretation:
		 *  - β₀ = 0.5 (intercept/base probability)
		 *  - β₁ = -2.0 (spread coefficient, negative because wider spreads favor taker orders)
		 *  - β₂ = 1.5 (imbalance coefficient, positive because imbalance may create maker opportunities)
		 *
		 * Output probability interpretation:
		 *  - Values near 1.0: Strong conditions for maker orders
		 *  - Values near 0.5: Neutral conditions (either strategy viable)
		 *  - Values near 0.0: Strong conditions for taker orders
		 * 
		 * These coefficients (β₀, β₁, β₂) are placeholders and should be replaced with actual values from a trained model.
		 * Here we're using arbitrary values base on domain knowledge for demonstration purposes.
		 **/
		const double beta_0 = 0.5; // Intercept - the base level of maker ratio
		const double beta_1 = -2.0; // Spread coefficient - negative because higher spread should lower the maker ratio
		const double beta_2 = 1.5; // Imbalance coefficient - positive because higher imbalance should increase the maker ratio

		double x = beta_0 + (beta_1 * spread) + (beta_2 * imbalance);
		double maker_probability = 1.0 / (1.0 + std::exp(-x)); // Logistic function

		return maker_probability;
	}

	QuantOKXCalculator& QuantOKXCalculator::get()
	{
		static QuantOKXCalculator instance; // Stack allocated, automatically destroyed
		return instance;
	}

}