#ifndef SCRIPTAUTOSAVER_H
#define SCRIPTAUTOSAVER_H

#include <QObject>
#include <QUuid>
#include <QFile>

class ScriptAutoSaver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUuid scriptId READ scriptId WRITE setScriptId NOTIFY scriptIdChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QString liveContent READ liveContent WRITE setLiveContent NOTIFY liveContentChanged)
    Q_PROPERTY(QString cachedContent READ cachedContent NOTIFY cachedContentChanged)

public:
    explicit ScriptAutoSaver(QObject *parent = nullptr);
    ~ScriptAutoSaver();

    bool available() const;

    bool active() const;
    void setActive(bool active);

    QUuid scriptId() const;
    void setScriptId(const QUuid &scriptId);

    QString liveContent() const;
    void setLiveContent(const QString &liveContent);

    QString cachedContent() const;

signals:
    void scriptIdChanged();
    void availableChanged();
    void activeChanged();
    void liveContentChanged();
    void cachedContentChanged();

private slots:
    void storeContent();

private:
    QUuid m_scriptId;
    QString m_cachedContent;
    QString m_liveContent;

    QFile m_cacheFile;

    bool m_active = false;
};

#endif // SCRIPTAUTOSAVER_H
