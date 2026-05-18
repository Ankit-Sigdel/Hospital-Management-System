#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QString>
#include <QVariantMap>

/*
 * SessionManager
 * ──────────────
 * Saves the authenticated user's details to a local JSON file so
 * the application can skip the login screen on the next launch.
 *
 * Storage location (Windows / MSVC):
 *   %APPDATA%\AXON_HMS\session.json
 *
 * The file is deleted on logout.
 *
 * Security note: this is a lightweight session token for a local
 * hospital management app. For a networked deployment, replace the
 * JSON file with a proper signed token (JWT) or OS credential store.
 */
class SessionManager
{
public:
    // Save a login session to disk
    static void saveSession(const QString &username,
                            const QString &role);

    // Load session from disk. Returns true if a valid session exists.
    // Populates username and role on success.
    static bool loadSession(QString &outUsername,
                            QString &outRole);

    // Delete the session file (call on logout)
    static void clearSession();

    // Returns true if a session file exists on disk
    static bool hasSession();

private:
    static QString sessionFilePath();
};

#endif // SESSIONMANAGER_H
