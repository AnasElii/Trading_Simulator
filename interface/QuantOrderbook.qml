import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: orderBookPanel
    color: "#1e1e1e"

    // Reference to shared data context
    property var dataContext

    // Store models locally for reactivity
    property var bidsModel: []
    property var asksModel: []

    // Update models when data changes
    Connections {

        target: QuantOrderbookModel
        function onOrderbookUpdated() {
            //console.log("QML: Orderbook updated signal received");
            bidsModel = QuantOrderbookModel.getBids();
            asksModel = QuantOrderbookModel.getAsks();
            //console.log("QML: Updated bids:", bidsModel.length, "asks:", asksModel.length);

            // TODO: Update output parameter base on new orderbook
            // If auto-update is enabled, recalculate outputs
            // this would update output parameters with each new tick
            //if (dataContext)
                //dataContext.calculateOutput();
        }
    }

    // Initial population
    Component.onCompleted: {
        //console.log("QML: Component completed, fetching initial data");
        bidsModel = QuantOrderbookModel.getBids();
        asksModel = QuantOrderbookModel.getAsks();
        //console.log("QML: Initial bids:", bidsModel.length, "asks:", asksModel.length);
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Label {
            text: "L2 Orderbook"
            font.bold: true
            font.pixelSize: 18
            color: "#ffffff"
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20

            // Bids (Buy Orders) Column - Green
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 5

                Label {
                    text: "Bids (Buy Orders)"
                    color: "#00ff00"
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }

                ListView {
                    id: bidsView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: bidsModel

                    // Debug placeholder
                    Text {
                        anchors.centerIn: parent
                        text: "Bids: " + (bidsModel ? bidsModel.length : 0)
                        visible: bidsModel.length === 0
                    }

                    delegate: Rectangle {
                        width: bidsView.width
                        height: 30
                        color: "#1a3a1a"  // Dark green background

                        Row {
                            width: parent.width
                            Text {
                                width: parent.width * 0.5
                                text: modelData.price
                                color: "#00ff00"  // Green text
                            }
                            Text {

                                width: parent.width * 0.5
                                text: modelData.amount
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }

            // Asks (Sell Orders) Column - Red
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 5

                Label {
                    text: "Asks (Sell Orders)"
                    color: "#ff0000"
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }

                ListView {
                    id: asksView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: asksModel

                    // Debug placeholder
                    Text {
                        anchors.centerIn: parent
                        text: "Asks: " + (asksModel ? asksModel.length : 0)
                        visible: asksModel.length === 0
                    }

                    delegate: Rectangle {
                        width: asksView.width
                        height: 30
                        color: "#3a1a1a"  // Dark red background

                        Row {
                            width: parent.width
                            Text {
                                width: parent.width * 0.5
                                text: modelData.price
                                color: "#ff0000"  // Red text
                            }
                            Text {
                                width: parent.width * 0.5
                                text: modelData.amount
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }
        }
    }
}
