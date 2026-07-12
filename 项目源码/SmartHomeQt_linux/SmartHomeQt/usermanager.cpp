#include "usermanager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QTextStream>

UserManager::UserManager(const QString &userFilePath)
{
    if (userFilePath.isEmpty())
        m_userFilePath = defaultUserFilePath();
    else
        m_userFilePath = userFilePath;
}

bool UserManager::registerUser(const QString &username, const QString &password, QString *errorMessage)
{
    QString name = username.trimmed();
    QString pass = password.trimmed();

    if (name.isEmpty() || pass.isEmpty()) {
        if (errorMessage)
            *errorMessage = "账号和密码不能为空";
        return false;
    }

    if (name.contains(",") || pass.contains(",")) {
        if (errorMessage)
            *errorMessage = "账号和密码不能包含逗号";
        return false;
    }

    if (userExists(name)) {
        if (errorMessage)
            *errorMessage = "账号已存在";
        return false;
    }

    if (!ensureDataDir(errorMessage))
        return false;

    QFile file(m_userFilePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        if (errorMessage)
            *errorMessage = "账号文件打开失败";
        return false;
    }

    QTextStream out(&file);
    out << name << "," << pass << "\n";

    return true;
}

bool UserManager::validateUser(const QString &username, const QString &password) const
{
    QString name = username.trimmed();
    QString pass = password.trimmed();

    QFile file(m_userFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(",");

        if (parts.size() == 2 && parts.at(0) == name && parts.at(1) == pass)
            return true;
    }

    return false;
}

QString UserManager::defaultUserFilePath() const
{
    return QCoreApplication::applicationDirPath() + "/data/users.txt";
}

bool UserManager::userExists(const QString &username) const
{
    QFile file(m_userFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(",");

        if (parts.size() == 2 && parts.at(0) == username)
            return true;
    }

    return false;
}

bool UserManager::ensureDataDir(QString *errorMessage) const
{
    QFileInfo info(m_userFilePath);
    QDir dir(info.absolutePath());

    if (dir.exists())
        return true;

    if (!dir.mkpath(".")) {
        if (errorMessage)
            *errorMessage = "data目录创建失败";
        return false;
    }

    return true;
}
