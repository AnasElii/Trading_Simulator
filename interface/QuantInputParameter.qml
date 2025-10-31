import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: inputPanel
    color: "#2a2a2a"

    // Reference to shared data context
    property var dataContext

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Label {
            text: "Input Parameters"
            font.bold: true
            font.pixelSize: 18
            color: "#ffffff"
        }

        // Exchange selection
        Label {
            text: "Exchange"
            color: "#ffffff"
        }
        ComboBox {
            Layout.fillWidth: true
            model: QuantInputModel.available_exchanges
            currentIndex: 0
            enabled: false // Only OKX supported
        }

        // Asset selection
        Label {
            text: "Spot Asset"
            color: "#ffffff"
        }
        ComboBox {
            Layout.fillWidth: true
            model: QuantInputModel.available_spot_assets
            currentIndex: 0
            onCurrentTextChanged: dataContext.selected_asset = currentText
            enabled: false // Only BTS-USDC-SWAP supported
        }

        // Order type
        Label {
            text: "Order Type"
            color: "#ffffff"
        }
        ComboBox {
            Layout.fillWidth: true
            model: QuantInputModel.available_order_types
            currentIndex: 0
            onCurrentTextChanged: dataContext.order_type = currentText
            enabled: false // Only market supported
        }

        // Quantity
        Label {
            text: "Quantity (USD)"
            color: "#ffffff"
        }
        TextField {
            Layout.fillWidth: true
            text: dataContext.usd_amount.toString()
            validator: DoubleValidator {
                bottom: 0
                decimals: 2
            }
            onTextChanged: if (text)
                QuantInputModel.usd_amount = parseFloat(text)
        }

        // Volatility
        ColumnLayout {
            spacing: 5
        
            Switch {
                id: volatilityToggle
            
                text: qsTr("Manual Volatility")
                checked: false
            
                onClicked:{
                    QuantInputModel.volatility_enabled = checked
                    volatilitySlider.enabled = checked
                    if(checked)
                        volatilitySlider.value = QuantInputModel.volatility
                }
            }
            Label {
                text: "Volatility"
                color: "#ffffff"
            }
            Slider {
                id: volatilitySlider
            
                Layout.fillWidth: true
                from: 0.01
                to: 1000.0
                enabled: volatilityToggle.checked
                value: QuantResultsModel.volatility
            
                onValueChanged: if (enabled) QuantInputModel.volatility = value
            }
            Text {
                text: QuantResultsModel.volatility.toFixed(2) + "%"
                color: "#ffffff"
            }
        }

        // Fee tier
        Label {
            text: "Fee Tier"
            color: "#ffffff"
        }
        ComboBox {
            Layout.fillWidth: true
            model: QuantInputModel.available_fee_tiers
            currentIndex: 0
            onCurrentTextChanged: QuantInputModel.fee_tier = currentText
        }

        Item {
            Layout.fillHeight: true
        } // Spacer

    }
}
