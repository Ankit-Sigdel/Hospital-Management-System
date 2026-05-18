/*
 * googlecalendarwindow.cpp
 * AXON Hospital Management System
 *
 * Persistent Google Calendar viewer.
 *
 * How persistence works
 * ─────────────────────
 * QWebEngineProfile("axon_gcal_profile") stores its data (cookies,
 * cache, localStorage, IndexedDB) in:
 *   Windows: %APPDATA%\AXON_HMS\QtWebEngine\axon_gcal_profile\
 *
 * Once the user signs into Google the first time, the session cookie
 * is saved to that folder. Every subsequent launch loads it and
 * skips the login screen.
 *
 * Build requirement (MSVC)
 * ─────────────────────────
 *   QT += webenginewidgets webenginecore
 *   CONFIG += c++17
 */

#include "googlecalendarwindow.h"

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QWebEngineSettings>
#include <QWebEngineCookieStore>
#include <QToolBar>
#include <QAction>
#include <QStatusBar>

// ════════════════════════════════════════════════════════════════════
//  Constructor / Destructor
// ════════════════════════════════════════════════════════════════════
GoogleCalendarWindow::GoogleCalendarWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Schedule — Google Calendar");
    resize(1280, 800);
    setMinimumSize(800, 600);

    // Apply same dark theme as the rest of the app
    setStyleSheet(R"(
        QMainWindow { background: #0d1b2a; }
        QToolBar {
            background: rgba(10, 22, 40, 0.95);
            border-bottom: 1px solid rgba(255,255,255,0.10);
            spacing: 6px;
            padding: 4px 8px;
        }
        QToolButton {
            background: rgba(255,255,255,0.10);
            border: 1px solid rgba(255,255,255,0.18);
            border-radius: 6px;
            color: #ffffff;
            font-family: 'Segoe UI';
            font-size: 13px;
            padding: 4px 10px;
        }
        QToolButton:hover  { background: rgba(255,255,255,0.20); }
        QToolButton:pressed{ background: rgba(0,180,180,0.30); }
        QStatusBar {
            background: rgba(10,22,40,0.95);
            color: rgba(200,220,240,0.60);
            font-family: 'Segoe UI';
            font-size: 11px;
            border-top: 1px solid rgba(255,255,255,0.07);
        }
        QProgressBar {
            background: rgba(255,255,255,0.08);
            border: none;
            border-radius: 2px;
            max-height: 3px;
        }
        QProgressBar::chunk {
            background: #0aafaf;
            border-radius: 2px;
        }
    )");

    setupProfile();
    setupUi();

    // Navigate to Google Calendar
    m_view->load(QUrl("https://calendar.google.com"));
}

GoogleCalendarWindow::~GoogleCalendarWindow()
{
    // Profile is parented — Qt cleans it up.
    // Do NOT delete the profile here; its data must persist on disk.
}

// ════════════════════════════════════════════════════════════════════
//  Profile setup — this is what makes login persistent
// ════════════════════════════════════════════════════════════════════
void GoogleCalendarWindow::setupProfile()
{
    // ── 1. Determine the storage path ───────────────────────────
    // QStandardPaths::AppDataLocation → %APPDATA%\AXON_HMS on Windows
    QString dataRoot = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString profilePath = dataRoot + "/QtWebEngine/axon_gcal_profile";
    QDir().mkpath(profilePath);

    // ── 2. Create a named, on-disk profile ──────────────────────
    // "axon_gcal_profile" is the profile name; passing the path
    // tells Qt where to store cookies / cache / localStorage.
    m_profile = new QWebEngineProfile("axon_gcal_profile", this);
    m_profile->setPersistentStoragePath(profilePath);
    m_profile->setCachePath(profilePath + "/cache");

    // ── 3. Cookie policy — persist all cookies ───────────────────
    m_profile->setPersistentCookiesPolicy(
        QWebEngineProfile::ForcePersistentCookies);

    // ── 4. Enable features Google Calendar needs ─────────────────
    QWebEngineSettings *s = m_profile->settings();
    s->setAttribute(QWebEngineSettings::JavascriptEnabled,          true);
    s->setAttribute(QWebEngineSettings::LocalStorageEnabled,        true);
    s->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    s->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled,      true);
    s->setAttribute(QWebEngineSettings::PluginsEnabled,             false);
    s->setAttribute(QWebEngineSettings::WebGLEnabled,               true);
    s->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);

    // ── 5. Set a realistic user-agent (avoids Google "unsupported browser" warning)
    m_profile->setHttpUserAgent(
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
        "AppleWebKit/537.36 (KHTML, like Gecko) "
        "Chrome/124.0.0.0 Safari/537.36");

    // ── 6. Create the page on this profile ───────────────────────
    m_page = new QWebEnginePage(m_profile, this);
}

// ════════════════════════════════════════════════════════════════════
//  UI setup
// ════════════════════════════════════════════════════════════════════
void GoogleCalendarWindow::setupUi()
{
    // ── Toolbar ──────────────────────────────────────────────────
    m_toolbar = addToolBar("Navigation");
    m_toolbar->setMovable(false);
    m_toolbar->setFloatable(false);

    // Back
    QAction *backAct = m_toolbar->addAction("◀  Back");
    connect(backAct, &QAction::triggered, this, [this]{ m_view->back(); });

    // Forward
    QAction *fwdAct = m_toolbar->addAction("Forward  ▶");
    connect(fwdAct, &QAction::triggered, this, [this]{ m_view->forward(); });

    // Reload
    QAction *reloadAct = m_toolbar->addAction("↺  Reload");
    connect(reloadAct, &QAction::triggered, this, [this]{ m_view->reload(); });

    m_toolbar->addSeparator();

    // Calendar home shortcut
    QAction *homeAct = m_toolbar->addAction("📅  Google Calendar");
    connect(homeAct, &QAction::triggered, this, [this]{
        m_view->load(QUrl("https://calendar.google.com"));
    });

    m_toolbar->addSeparator();

    // Clear session (lets the user log out / switch accounts)
    QAction *clearAct = m_toolbar->addAction("🔓  Clear Session");
    clearAct->setToolTip("Sign out and clear saved login cookies");
    connect(clearAct, &QAction::triggered, this, [this]{
        m_profile->cookieStore()->deleteAllCookies();
        m_profile->clearAllVisitedLinks();
        m_view->load(QUrl("https://accounts.google.com/signout"));
    });

    // ── Progress bar (thin strip at top of content) ──────────────
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(3);
    m_progressBar->setVisible(false);

    // ── WebEngine view ───────────────────────────────────────────
    m_view = new QWebEngineView(this);
    m_view->setPage(m_page);

    // ── Central layout: progress bar on top, view below ──────────
    QWidget *central = new QWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(central);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(m_progressBar);
    lay->addWidget(m_view);
    setCentralWidget(central);

    // ── Status bar ───────────────────────────────────────────────
    m_statusLabel = new QLabel("Loading…", this);
    statusBar()->addWidget(m_statusLabel);
    statusBar()->setVisible(true);

    // ── Signals ──────────────────────────────────────────────────
    connect(m_view, &QWebEngineView::loadProgress,
            this,   &GoogleCalendarWindow::onLoadProgress);
    connect(m_view, &QWebEngineView::loadFinished,
            this,   &GoogleCalendarWindow::onLoadFinished);
    connect(m_view, &QWebEngineView::titleChanged,
            this,   &GoogleCalendarWindow::onTitleChanged);
}

// ════════════════════════════════════════════════════════════════════
//  Slots
// ════════════════════════════════════════════════════════════════════
void GoogleCalendarWindow::onLoadProgress(int progress)
{
    m_progressBar->setVisible(progress > 0 && progress < 100);
    m_progressBar->setValue(progress);
    m_statusLabel->setText(QString("Loading… %1%").arg(progress));
}

void GoogleCalendarWindow::onLoadFinished(bool ok)
{
    m_progressBar->setVisible(false);
    m_statusLabel->setText(ok ? "Ready" : "Failed to load page");
}

void GoogleCalendarWindow::onTitleChanged(const QString &title)
{
    setWindowTitle(title.isEmpty() ? "Schedule — Google Calendar" : title);
}
