#pragma once
#include <QString>

namespace Quant
{
	struct QuantConstants
	{
		/**
		*Sample response format:
		*  {
		*	 "timestamp": "2025-05-04T10:39:13Z",
		*	 "exchange": "OKX",
		*	 "symbol": "BTC-USDT-SWAP",
		*	 "asks": [
		*	   ["95445.5", "9.06"],
		*	   ["95448", "2.05"],
		*	   // ... more ask levels ...
		*	 ],
		*	 "bids": [
		*	   ["95445.4", "1104.23"],
		*	   ["95445.3", "0.02"],
		*	   // ... more bid levels ...
		*	 ]
		*  }
		*
		**/
		static constexpr const char* SOCKET_ENDPOINT = ""; // Endpoint ws

		// API Keys, Secrets, and Passphrases
		static QString GetApiKey();
		static QString GetApiSecret();
		static QString GetApiPassphrase();
	};
}