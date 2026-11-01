#include "Wallet.h"

#include <QJsonArray>

#include <algorithm>

Wallet::Wallet(QObject *parent)
    : QObject(parent)
{ }

bool Wallet::credit(const QString &key, int amount, const QString &reason, const QDateTime &now)
{
    if (amount <= 0 || key.isEmpty() || hasTransaction(key)) {
        return false;
    }
    m_transactions.append({key, amount, reason, now});
    m_balance += amount;
    emit balanceChanged();
    return true;
}

bool Wallet::hasTransaction(const QString &key) const
{
    return std::any_of(m_transactions.cbegin(), m_transactions.cend(),
                       [&key](const Transaction &transaction) { return transaction.key == key; });
}

QJsonObject Wallet::toJson() const
{
    QJsonArray transactions;
    for (const Transaction &transaction : m_transactions) {
        transactions.append(QJsonObject{
            {QStringLiteral("key"), transaction.key},
            {QStringLiteral("amount"), transaction.amount},
            {QStringLiteral("reason"), transaction.reason},
            {QStringLiteral("createdAt"), transaction.createdAt.toString(Qt::ISODate)},
        });
    }
    return {{QStringLiteral("transactions"), transactions}};
}

void Wallet::fromJson(const QJsonObject &json)
{
    m_transactions.clear();
    m_balance = 0;
    const QJsonArray transactions = json.value(QStringLiteral("transactions")).toArray();
    for (const auto &value : transactions) {
        const QJsonObject object = value.toObject();
        const QString key = object.value(QStringLiteral("key")).toString();
        const int amount = object.value(QStringLiteral("amount")).toInt();
        // Same rules as credit(): skip invalid or duplicate entries.
        if (amount <= 0 || key.isEmpty() || hasTransaction(key)) {
            continue;
        }
        m_transactions.append({
            key,
            amount,
            object.value(QStringLiteral("reason")).toString(),
            QDateTime::fromString(object.value(QStringLiteral("createdAt")).toString(),
                                  Qt::ISODate),
        });
        m_balance += amount;
    }
    emit balanceChanged();
}
