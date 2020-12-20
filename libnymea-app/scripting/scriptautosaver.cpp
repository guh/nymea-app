#include "scriptautosaver.h"

#include <QStandardPaths>
#include <QDir>
#include <QDebug>

ScriptAutoSaver::ScriptAutoSaver(QObject *parent) : QObject(parent)
{

}

ScriptAutoSaver::~ScriptAutoSaver()
{
    storeContent();
}

bool ScriptAutoSaver::available() const
{
    return m_cacheFile.isOpen();
}

bool ScriptAutoSaver::active() const
{
    return m_active;
}

void ScriptAutoSaver::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        emit activeChanged();

        storeContent();
    }
}

QUuid ScriptAutoSaver::scriptId() const
{
    return m_scriptId;
}

void ScriptAutoSaver::setScriptId(const QUuid &scriptId)
{
    if (m_scriptId != scriptId) {
        m_scriptId = scriptId;
        emit scriptIdChanged();

        if (m_cacheFile.isOpen()) {
            m_cacheFile.close();
            emit availableChanged();
        }

        QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/scripts/";
        QDir dir(path);
        if (!dir.exists() && !dir.mkpath(path)) {
            qWarning() << "Cannot create cache directory. Autosaving will not work...";
            return;
        }
        QString fileName = path + m_scriptId.toString().remove(QRegExp("[{}]")) + ".qml.autosave";
        m_cacheFile.setFileName(fileName);
        if (!m_cacheFile.open(QFile::ReadWrite)) {
            qWarning() << "Cannot open cache file. Autosaving will not work...";
            return;
        }
        m_cachedContent = QString::fromUtf8(m_cacheFile.readAll());
        emit cachedContentChanged();
        emit availableChanged();
    }
}

QString ScriptAutoSaver::liveContent() const
{
    return m_liveContent;
}

void ScriptAutoSaver::setLiveContent(const QString &liveContent)
{
    if (m_liveContent != liveContent) {
        m_liveContent = liveContent;
        emit liveContentChanged();

        storeContent();
    }
}

QString ScriptAutoSaver::cachedContent() const
{
    return m_cachedContent;
}

void ScriptAutoSaver::storeContent()
{
    if (m_cacheFile.isOpen() && m_active && m_liveContent != m_cachedContent) {
        qDebug() << "autosaving...";
        m_cacheFile.seek(0);
        m_cacheFile.resize(0);
        m_cacheFile.write(m_liveContent.toUtf8());
        m_cacheFile.flush();

        m_cachedContent = m_liveContent;
        emit cachedContentChanged();
    }

}
