// Non-Qt headers
#include <pwf2tools/libpwf2log.h>

// Namespaces
namespace libpwf2log {
	// Strings
	QString logFile;
	QString logPath;
	QString logProduct;
	std::string logOutput;

	/// I don't think there's a reason anybody would need this,
	/// but it's here just in case.
	std::string readFromLog() {
		logOutput = "";
		QFile log(logPath + logFile);
		if (log.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
			QTextStream fileStream(&log);
			logOutput = fileStream.readAll().toStdString();
		}
		return logOutput;
	}

	void closeLog(QString dateTimeFormat) {
		writeToLog(LOG_INFO, QStringLiteral("Closing %1...").arg(logProduct).toStdString(), dateTimeFormat);
		logOutput = "";
	}

	void startLog(QString path, QString file, QString product, QString dateTimeFormat) {
		logFile = file;
		logOutput = "";
		logPath = path;
		logProduct = product;

		QDir log(logPath);
		if(!log.exists())
			log.mkpath(logPath);

		writeToLog(LOG_INFO, QStringLiteral("%1 started!").arg(logProduct).toStdString(), dateTimeFormat);
	}

	void writeToLog(LOG_LEVELS logLevel, std::string logContent, QString dateTimeFormat) {
		std::string input;
		input = QStringLiteral("[%1, %2] - %3\n").arg(QDateTime::currentDateTime().toString(dateTimeFormat), LOG_LEVELS_CHAR[logLevel], logContent.c_str()).toStdString();
		QFile log(logPath + logFile);
		if(log.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
			QTextStream fileStream(&log);
			fileStream << input.c_str();
		}
		log.close();
	}

	void writeToLogNN(LOG_LEVELS logLevel, std::string logContent, QString dateTimeFormat) {
		std::string input;
		input = QStringLiteral("[%1, %2] - %3").arg(QDateTime::currentDateTime().toString(dateTimeFormat), LOG_LEVELS_CHAR[logLevel], logContent.c_str()).toStdString();
		QFile log(logPath + logFile);
		if(log.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
			QTextStream fileStream(&log);
			fileStream << input.c_str();
		}
		log.close();
	}
}
