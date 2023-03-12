#ifndef PWF2LOG_H
#define PWF2LOG_H
// Qt headers \/
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QString>
#include <QStringLiteral>
#include <QTextStream>
// Non-Qt headers \/
#include <iostream>
#include <string>
#include <vector>

// Log levels
enum LOG_LEVELS {
	LOG_ERROR,
	LOG_INFO,
	LOG_ISOMOD,
	LOG_PWF2INT,
	/// Future
	LOG_PWF2SOUND,
	LOG_PWF2SPM,
	LOG_PWF2TEX,
	LOG_WARNING
};

static const char *LOG_LEVELS_CHAR[] = {
	"ERROR",
	"INFO",
	"ISOMOD",
	/// Future
	"PWF2INT",
	"PWF2SOUND",
	"PWF2SPM",
	"PWF2TEX",
	"WARNING"
};

// Namespaces
namespace libpwf2log {
	extern QString logFile;
	extern QString logPath;
	extern QString logProduct;
	extern std::string logOutput;
	std::string readFromLog();
	void closeLog(QString dateTimeFormat = "dddd, MMMM dd, yyyy '-' hh:mm:ss");
	void startLog(QString path, QString file, QString product = "pwf2tools", QString dateTimeFormat = "dddd, MMMM dd, yyyy '-' hh:mm:ss");
	void writeToLog(LOG_LEVELS logLevel = LOG_INFO, std::string logContent = "", QString dateTimeFormat = "dddd, MMMM dd, yyyy '-' hh:mm:ss");
	void writeToLogNN(LOG_LEVELS logLevel = LOG_INFO, std::string logContent = "", QString dateTimeFormat = "dddd, MMMM dd, yyyy '-' hh:mm:ss");
};

#endif // PWF2LOG_H
