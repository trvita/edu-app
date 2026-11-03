#include <QSignalSpy>
#include <QtTest>

#include "Wallet.h"

class TestWallet : public QObject
{
    Q_OBJECT

private slots:
    void startsEmpty()
    {
        const Wallet wallet;
        QCOMPARE(wallet.balance(), 0);
        QVERIFY(wallet.transactions().isEmpty());
    }

    void creditAddsToBalanceAndLedger()
    {
        Wallet wallet;
        QSignalSpy spy(&wallet, &Wallet::balanceChanged);
        QVERIFY(wallet.credit("quiz:a", 3, "quiz"));
        QVERIFY(wallet.credit("quiz-perfect:a", 5, "quiz-perfect"));
        QCOMPARE(wallet.balance(), 8);
        QCOMPARE(wallet.transactions().size(), 2);
        QCOMPARE(spy.count(), 2);
    }

    void sameKeyIsGrantedOnlyOnce()
    {
        Wallet wallet;
        QVERIFY(wallet.credit("quiz:a", 3, "quiz"));
        QVERIFY(!wallet.credit("quiz:a", 3, "quiz"));
        QCOMPARE(wallet.balance(), 3);
    }

    void nonPositiveAmountsAndEmptyKeysAreRejected()
    {
        Wallet wallet;
        QVERIFY(!wallet.credit("zero", 0, "quiz"));
        QVERIFY(!wallet.credit("negative", -2, "quiz"));
        QVERIFY(!wallet.credit("", 2, "quiz"));
        QCOMPARE(wallet.balance(), 0);
        QVERIFY(!wallet.hasTransaction("zero")); // a rejected key stays free
    }

    void jsonRoundTripRestoresBalance()
    {
        Wallet original;
        original.credit("daily-login:2026-11-02", 1, "daily-login");
        original.credit("quiz:a", 2, "quiz");

        Wallet restored;
        restored.fromJson(original.toJson());
        QCOMPARE(restored.balance(), 3);
        QVERIFY(restored.hasTransaction("quiz:a"));
        QVERIFY(!restored.credit("quiz:a", 2, "quiz"));
    }

    void resetClearsBalanceAndFreesKeys()
    {
        Wallet wallet;
        wallet.credit("quiz:a", 2, "quiz");
        const QSignalSpy spy(&wallet, &Wallet::balanceChanged);
        wallet.reset();
        QCOMPARE(spy.count(), 1);
        QCOMPARE(wallet.balance(), 0);
        QVERIFY(wallet.transactions().isEmpty());
        QVERIFY(wallet.credit("quiz:a", 2, "quiz"));
    }

    void invalidSavedEntriesAreSkipped()
    {
        const QJsonObject json{
            {"transactions",
             QJsonArray{
                 QJsonObject{{"key", "a"}, {"amount", 2}},
                 QJsonObject{{"key", "a"}, {"amount", 2}}, // duplicate
                 QJsonObject{{"key", "b"}, {"amount", -5}}, // negative
                 QJsonObject{{"amount", 4}}, // no key
             }},
        };
        Wallet wallet;
        wallet.fromJson(json);
        QCOMPARE(wallet.balance(), 2);
    }
};

QTEST_APPLESS_MAIN(TestWallet)
#include "tst_wallet.moc"
