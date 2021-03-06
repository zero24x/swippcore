// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRANSACTIONRECORD_H
#define TRANSACTIONRECORD_H

#include "uint256.h"

#include <QList>
#include <QString>

class CWallet;
class CWalletTx;

// UI model for transaction status. The transaction status is the part of a transaction that will change over time.
class TransactionStatus
{
public:
    TransactionStatus(): countsForBalance(false), sortKey(""),
                         matures_in(0), status(Offline), depth(0), open_for(0), cur_num_blocks(-1) {}
    enum Status
    {
        Confirmed,      // < Have 6 or more confirmations (normal tx) or fully mature (mined tx)

        // Normal (sent/received) transactions
        OpenUntilDate,  // < Transaction not yet final, waiting for date
        OpenUntilBlock, // < Transaction not yet final, waiting for block
        Offline,        // < Not sent to any other nodes
        Unconfirmed,    // < Not yet mined into a block
        Confirming,     // < Confirmed, but waiting for the recommended number of confirmations
        Conflicted,     // < Conflicts with other transaction or mempool

        // Generated (mined) transactions
        Immature,       // < Mined but waiting for maturity
        MaturesWarning, // < Transaction will likely not mature because no nodes have confirmed
        NotAccepted     // < Mined but not accepted
    };

    // Transaction counts towards available balance
    bool countsForBalance;

    // Sorting key based on status
    std::string sortKey;

    // Generated (mined) transactions
    int matures_in;

    // Reported status
    Status status;
    qint64 depth;

    // < Timestamp if status==OpenUntilDate, otherwise number of additional blocks that need to be mined before finalization
    int64_t open_for;

    // Current number of blocks (to know whether cached status is still valid)
    int cur_num_blocks;
};

// UI model for a transaction. A core transaction can be represented by multiple UI transactions if it has multiple outputs.
class TransactionRecord
{
public:
    enum Type
    {
        Other,
        Generated,
        SendToAddress,
        SendToOther,
        RecvWithAddress,
        RecvFromOther,
        SendToSelf
    };

    // Number of confirmation recommended for accepting a transaction
    static const int RecommendedNumConfirmations = 10;

    TransactionRecord(int64_t orderPos) : hash(), time(0), type(Other), address(""), debit(0), credit(0),
                                          orderPos(orderPos), idx(0) {}

    TransactionRecord(uint256 hash, int64_t time, int64_t orderPos): hash(hash), time(time), type(Other), address(""), debit(0),
                                                                     credit(0), orderPos(orderPos), idx(0) {}

    TransactionRecord(uint256 hash, int64_t time, Type type, const std::string &address, int64_t debit,
                      int64_t credit, int64_t orderPos) : hash(hash), time(time), type(type), address(address), debit(debit),
                                                          credit(credit), orderPos(orderPos), idx(0) {}

    // Decompose CWallet transaction to model transaction records
    static bool showTransaction(const CWalletTx &wtx);
    static QList<TransactionRecord> decomposeTransaction(const CWallet *wallet, const CWalletTx &wtx);

    uint256 hash;
    qint64 time;
    Type type;
    std::string address;
    qint64 debit;
    qint64 credit;
    qint64 orderPos;

    // Subtransaction index, for sort key
    int idx;

    // Status: can change with block chain update
    TransactionStatus status;

    // Return the unique identifier for this transaction (part)
    QString getTxID() const;

    // Format subtransaction id
    static QString formatSubTxId(const uint256 &hash, int vout);

    // Update status from core wallet tx
    void updateStatus(const CWalletTx &wtx);

    // Return whether a status update is needed
    bool statusUpdateNeeded();
};

#endif
