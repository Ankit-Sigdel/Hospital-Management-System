/*
 * sessionmanager.cpp
 * AXON Hospital Management System
 *
 * Reads and writes a simple JSON session file so the user does not
 * have to log in on every application launch.
 */

#include "sessionmanager.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

// ────────────────────────────────────────────────────────────────────
//  Private helper — returns the full path to session.json
// ────────────────────────────────────────────────────────────────────
QString SessionManager::sessionFilePath()
{
    // %APPDATA%\AXON_HMS\  on Windows with MSVC
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/session.json";
}

// ────────────────────────────────────────────────────────────────────
//  Save
// ────────────────────────────────────────────────────────────────────
void SessionManager::saveSession(const QString &username,
                                 const QString &role)
{
    QJsonObject obj;
    obj["username"]  = username;
    obj["role"]      = role;
    // Store the login timestamp so you can expire sessions if needed
    obj["loginTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QFile file(sessionFilePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
        file.close();
    }
}

// ────────────────────────────────────────────────────────────────────
//  Load
// ────────────────────────────────────────────────────────────────────
bool SessionManager::loadSession(QString &outUsername,
                                 QString &outRole)
{
    QFile file(sessionFilePath());
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();

    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    QJsonObject obj = doc.object();

    // Validate required fields
    if (!obj.contains("username") || !obj.contains("role"))
        return false;

    outUsername = obj["username"].toString();
    outRole     = obj["role"].toString();

    if (outUsername.isEmpty() || outRole.isEmpty())
        return false;

    // ── Optional: expire sessions older than 30 days ─────────────
    if (obj.contains("loginTime")) {
        QDateTime loginTime = QDateTime::fromString(
            obj["loginTime"].toString(), Qt::ISODate);
        if (loginTime.isValid() &&
            loginTime.daysTo(QDateTime::currentDateTime()) > 30) {
            clearSession();
            return false;
        }
    }

    return true;
}

// ────────────────────────────────────────────────────────────────────
//  Clear
// ────────────────────────────────────────────────────────────────────
void SessionManager::clearSession()
{
    QFile::remove(sessionFilePath());
}

// ────────────────────────────────────────────────────────────────────
//  Has session
// ────────────────────────────────────────────────────────────────────
bool SessionManager::hasSession()
{
    return QFile::exists(sessionFilePath());
}
