#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>

class UserManager
{
public:
    explicit UserManager(const QString &userFilePath = QString());

    bool registerUser(const QString &username, const QString &password, QString *errorMessage = nullptr);
    bool validateUser(const QString &username, const QString &password) const;
    bool userExists(const QString &username) const;
private:
    QString defaultUserFilePath() const;
    //bool userExists(const QString &username) const;
    bool ensureDataDir(QString *errorMessage) const;

private:
    QString m_userFilePath;
};

#endif // USERMANAGER_H
