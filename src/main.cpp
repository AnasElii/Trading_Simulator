#include <QObject>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

#include <iostream>

#include "QuantOrderbook.h"
#include "QuantWebSocket.h"
#include "QuantInputHandler.h"
#include "QuantConstants.h"
#include "QuantCalculatorAPI.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Create orderbook instance
    Quant::QuantOrderbook orderbook;
    engine.rootContext()->setContextProperty("QuantOrderbookModel", &orderbook);

	// Create input handler instance
	Quant::QuantInputHandler input_handler;
	engine.rootContext()->setContextProperty("QuantInputModel", &input_handler);

	// Create calculator API instance
	Quant::QuantCalculatorAPI calculator_api;
	engine.rootContext()->setContextProperty("QuantCalculatorModel", &calculator_api);
	engine.rootContext()->setContextProperty("QuantResultsModel", calculator_api.GetResult());

	// Connect the calculator to its input handler
    calculator_api.SetInputHandler(&input_handler);
    calculator_api.SetOrderbook(&orderbook);

    // Initialize the calculator interface
	calculator_api.selectedExchange();

    // Create webSocket instance
    Quant::QuantWebSocket websocket;

	// Connect websocket signals to orderbook slots
    QObject::connect(&websocket, &Quant::QuantWebSocket::orderbookUpdated, &orderbook, &Quant::QuantOrderbook::updateOrderbook);
    QObject::connect(&websocket, &Quant::QuantWebSocket::error, &orderbook, 
        [](const QString &error)
        {
            qWarning() << "WebSocket error:" << error;
        });

	// Load QML file
    const QUrl url(u"qrc:/Main/interface/main.qml"_qs);

    // write the function to be called from QML
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl)
        {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // Start Websocket connection
    websocket.connect(Quant::QuantConstants::SOCKET_ENDPOINT);

    engine.load(url);
    return app.exec();
}