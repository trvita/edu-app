#pragma once

#include <QDateTime>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

// Virtual coin balance with a ledger of every change (ARCH-03, BR-ECON-02/03).
// Each transaction has a unique key, so the same reward can never be granted twice.
// The balance is the sum of the ledger and never goes negative.
class Wallet : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by LearningService")

    Q_PROPERTY(int balance READ balance NOTIFY balanceChanged)

public:
    struct Transaction
    {
        QString key; // unique, e.g. "quiz:m1-l1"
        int amount = 0;
        QString reason;
        QDateTime createdAt;
    };

    explicit Wallet(QObject *parent = nullptr);

    int balance() const { return m_balance; }
    QList<Transaction> transactions() const { return m_transactions; }

    // Adds coins. Returns false (and changes nothing) for a non-positive amount or a key
    // that was already used.
    bool credit(const QString &key, int amount, const QString &reason,
                const QDateTime &now = QDateTime::currentDateTimeUtc());
    bool hasTransaction(const QString &key) const;
    // Removes every transaction; the balance becomes 0 (progress reset).
    void reset();

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

signals:
    void balanceChanged();

private:
    QList<Transaction> m_transactions;
    int m_balance = 0;
};
