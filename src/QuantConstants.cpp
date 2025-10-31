/*
 * Configuration Files - Consider as the best practice to store sensitive information
 * on the system.
 * The following code checks for the presence of environment variables
 * then check for the presence of a config.json file in the application config location.
 * Configuration Files:
 * Store in user's config directory (e.g., ~/.config/QuantApp/config.json on Linux)
 * Encrypting sensitive information in the config file.
 * Use Proper file permissions to restrict access to the config file.
 */

#include "QuantConstants.h"

#include <QProcessEnvironment>
#include <QFile>
#include <QJsonObject>
#include <QStandardPaths>

namespace Quant
{
	
	QString QuantConstants::GetApiKey()
	{
		// Env variable check
		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		if (env.contains("OKX_API_KEY"))
			return env.value("OKX_API_KEY");

		// Config file check
		QString config_path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.json";
		QFile config_file(config_path);
		if (config_file.open(QIODevice::ReadOnly))
		{
			QJsonDocument json_doc = QJsonDocument::fromJson(config_file.readAll());
			config_file.close();

			if (json_doc.isNull() && json_doc.isObject())
			{
				QJsonObject json_obj = json_doc.object();
				if (json_obj.contains("api_key"))
					return json_obj["api_key"].toString();
			}
		}

		return QString();
	}

	QString QuantConstants::GetApiSecret()
	{
		// Env variable check
		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		if (env.contains("OKX_API_SECRET"))
			return env.value("OKX_API_SECRET");

		// Config file check
		QString config_path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.json";
		QFile config_file(config_path);
		if (config_file.open(QIODevice::ReadOnly))
		{
			QJsonDocument json_doc = QJsonDocument::fromJson(config_file.readAll());
			config_file.close();

			if (json_doc.isNull() && json_doc.isObject())
			{
				QJsonObject json_obj = json_doc.object();
				if (json_obj.contains("api_secret"))
					return json_obj["api_secret"].toString();
			}
		}

		return QString();
	}

	QString QuantConstants::GetApiPassphrase()
	{
		// Env variable check
		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		if (env.contains("OKX_API_PASSPHRASE"))
			return env.value("OKX_API_PASSPHRASE");

		// Config file check
		QString config_path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.json";
		QFile config_file(config_path);
		if (config_file.open(QIODevice::ReadOnly))
		{
			QJsonDocument json_doc = QJsonDocument::fromJson(config_file.readAll());
			config_file.close();

			if (json_doc.isNull() && json_doc.isObject())
			{
				QJsonObject json_obj = json_doc.object();
				if (json_obj.contains("api_passphrase"))
					return json_obj["api_passphrase"].toString();
			}
		}

		return QString();
	}


}