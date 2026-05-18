#ifndef GOOGLECALENDARWINDOW_H
#define GOOGLECALENDARWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QVBoxLayout>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>

/*
 * GoogleCalendarWindow
 * ────────────────────
 * Opens Google Calendar in a persistent QWebEngineView.
 * Uses a named profile stored on disk so cookies, sessions,
 * and localStorage survive between application runs.
 *
 * The user only needs to log in to Google once; subsequent
 * opens skip the login screen entirely.
 */
class GoogleCalendarWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GoogleCalendarWindow(QWidget *parent = nullptr);
    ~GoogleCalendarWindow();

private slots:
    void onLoadProgress(int progress);
    void onLoadFinished(bool ok);
    void onTitleChanged(const QString &title);

private:
    void setupUi();
    void setupProfile();

    // Persistent profile (survives app restarts)
    QWebEngineProfile *m_profile  = nullptr;
    QWebEnginePage    *m_page     = nullptr;
    QWebEngineView    *m_view     = nullptr;

    // Toolbar widgets
    QToolBar    *m_toolbar        = nullptr;
    QProgressBar *m_progressBar   = nullptr;
    QLabel      *m_statusLabel    = nullptr;
};

#endif // GOOGLECALENDARWINDOW_H
