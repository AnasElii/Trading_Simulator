import QtQuick
import QtQuick.Layouts

Window {
    id: mainScreen
    width: 1200
    height: 700

    visible: true
    title: "OKX Order Analytics"

    // Data context - Shared between components
    property QtObject data_context: QtObject {
        // Input Parameters
        property string selected_exchange: QuantInputModel.selected_exchange
        property string selected_asset: QuantInputModel.selected_asset
        property string order_type: QuantInputModel.order_type
        property double quantity: QuantInputModel.quantity
        property double usd_amount: QuantInputModel.usd_amount
        property double volatility: QuantInputModel.volatility
        property string fee_tier: QuantInputModel.fee_tier
        property bool volatility_enabled: QuantInputModel.volatility_enabled

        // Output parameters
        property double expected_splippage: 0.0
        property double expected_fees: 0.0
        property double expected_market_impact: 0.0 // TODO: Almgren-Chriss model
        property double net_cost: 0.0
        property double crypto_amount: 0.0
        property double maker_taker_ration: 0.0
        property double processing_time: 0.0

        // TODO: Function to calculate output
        function calculateOutput() {
            QuantInputModel.CalculateOutput()
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 10

        // Left panel: Input parameters
        QuantInputParameter {
            id: inputParameterPanel
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.25
            dataContext: mainScreen.data_context
        }

        // Middle panel: Orderbook
        QuantOrderbook {
            id: orderbookPanel
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.5
            dataContext: mainScreen.data_context
        }

        // Left panel: Input parameters
        QuantOutputParameter {
            id: outputParameterPanel
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.25
            dataContext: QuantResultsModel
        }
    }
}
