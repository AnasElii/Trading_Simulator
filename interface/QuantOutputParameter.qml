import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: outputPanel
    width: parent.width
    height: parent.height
    color: "#2a2a2a"

    // Reference to shared data context
    property var dataContext

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15

        Label {
            text: "Output Parameters"
            font.bold: true
            font.pixelSize: 18
            color: "#ffffff"
        }

        // Expected Slippage
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Expected Slippage:"
                color: "#ffffff"
            }
            Label {
                text: dataContext ? dataContext.expected_slippage.toFixed(4) + " USD" : "0.0000 USD"
                color: "#aaffaa"
            }
        }

        // Expected Fees
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Expected Fees:"
                color: "#ffffff"
            }
            Label {
                text: dataContext ? dataContext.expected_fees.toFixed(2) + " USD" : "0.00 USD"
                color: "#aaffaa"
            }
        }

        //Expected Market Impact
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Market Impact:"
                color: "#ffffff"
            }
            Label {
                text: dataContext ? dataContext.expected_market_impact.toFixed(4) + " USD" : "0.00 USD"
                color: "#aaffaa"
            }
        }

        // Crypto Amount
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Crypto Amount:"
                font.bold: true
                color: "#ffffff"
            }
            Label {
                text: QuantResultsModel ? QuantResultsModel.crypto_amount.toFixed(6) + " BTC" : "0.00 BTC"
                font.bold: true
                color: "#aaffaa"
            }
        }

        // Net Cost
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Net Cost:"
                font.bold: true
                color: "#ffffff"
            }
            Label {
                text: QuantResultsModel ? QuantResultsModel.net_cost.toFixed(2) + " USD" : "0.00 USD"
                font.bold: true
                color: "#aaffaa"
            }
        }

        // TODO: add market order cost

        // Maker/Taker proportion
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Maker/Taker:"
                color: "#ffffff"
            }
            Label {
                text: dataContext ? (dataContext.maker_ratio * 100).toFixed(1) + "% / " + 
                    ((1 - dataContext.maker_ratio) * 100).toFixed(1) + "%" : "0.0% / 0.0%"
                color: "#aaffaa"
            }
        }

        // Internal Latency
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Processing Latency:"
                color: "#aaffaa"
            }
            Label {
                text: QuantResultsModel.processing_time.toFixed(2) + "ms"
                color: "#aaffaa"
            }
        }

        Item {
            Layout.fillHeight: true
        } // Spacer
    }
}
