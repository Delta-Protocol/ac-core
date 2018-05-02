<?php

namespace Atlas\WalletD;

/**
 * Interface for the walletD RPC API.
 *
 * @author James Kirby <james.kirby@atlascityfinance.com>
 * @package Atlas\WalletD
 * @copyright MIT
 */
interface WalletDRPCInterface extends \Atlas\Abstracts\AbstractRPCInterface
{

    /**
     * Add a n-required-to-sign multi-signature address to the wallet.
     *
     * Each key is an address or hex-encoded public key.
     *
     * Returns a string containing the address.
     *
     * @param int $nRequired
     * @param array $keys
     * @return string
     */
    public function addMultiSigAddress(int $nRequired, array $keys) : string;

    /**
     * Safely copies wallet.dat to destination, which can be a directory or a path with filename.
     *
     * @param string $destination
     * @return bool
     */
    public function backUpWallet(string $destination) : bool;

    /**
     * Creates a multi-signature address and returns an object containing a list of public keys associated to the multi-sig.
     *
     * @see \Atlas\DTO\MultiSigDTO
     *
     * @param int $nRequired
     * @param array $keys
     * @return \Atlas\DTO\MultiSigDTO
     */
    public function createMultiSigWallet(int $nRequired, array $keys) : \Atlas\DTO\MultiSigDTO;

    /**
     * Creates a raw transaction spending given inputs.
     *
     * @see \Atlas\DTO\RawTransactionDTO
     *
     * @param string $txid
     * @param int $utxo
     * @param string $address
     * @return \Atlas\DTO\RawTransactionDTO
     */
    public function createRawTransaction(string $txid, int $utxo, string $address) : \Atlas\DTO\RawTransactionDTO;

    /**
     * Produces a human-readable JSON object for a raw transaction.
     *
     * @see \Atlas\DTO\TransactionDTO
     *
     * @param string $transactionHex
     * @return \Atlas\DTO\TransactionDTO
     */
    public function decodeRawTransaction(string $transactionHex) : \Atlas\DTO\TransactionDTO;

    /**
     * Reveals the private key corresponding to the public key parameter.
     *
     * @param string $publicKey
     * @return string
     */
    public function dumpPrivateKey(string $publicKey) : string;

    /**
     * Exports all wallet private keys to file.
     *
     * Return the file path and name.
     *
     * @param string $fileName
     * @return string
     */
    public function dumpWallet(string $fileName) : string;

    /**
     * Encrypts the wallet with the passphrase parameter.
     *
     * @param string $passphrase
     * @return bool
     */
    public function encryptWallet(string $passphrase) : bool;


    /**
     * If $address is not specified, returns the server's total available balance.
     *
     * If $address is specified, returns the balance of the $address.
     *
     * @param string $address
     * @param int $minConf
     * @return float
     */
    public function getBalance(string $address = null, int $minConf = 1) : float;

    /**
     * Return a new address for receiving payments.
     *
     * @return string
     */
    public function getNewAddress() : string;

    /**
     * Returns a raw representation of a given transaction.
     *
     * @see \Atlas\DTO\RawTransactionDTO
     *
     * @param string $txid
     * @return \Atlas\DTO\RawTransactionDTO
     */
    public function getRawTransaction(string $txid): \Atlas\DTO\RawTransactionDTO;

    /**
     * Returns the amount received by $address in transactions with at least $minconf confirmations.
     * It correctly handles the case where someone has sent to the address in multiple transactions.
     * Keep in mind that addresses are only ever used for receiving transactions.
     * Works only for addresses in the local wallet, external addresses will always show 0.
     *
     * @param string $address
     * @param int $minconf
     * @return float
     */
    public function getReceivedByAddress(string $address, int $minconf = 1) : float;

    /**
     * Returns an object about the given transaction.
     *
     * @see \Atlas\DTO\TransactionDTO
     *
     * @param string $txid
     * @return \Atlas\DTO\TransactionDTO
     */
    public function getTransaction(string $txid) : \Atlas\DTO\TransactionDTO;

    /**
     * Adds a private key (as returned by dumpPrivateKey() ) to your wallet
     *
     * @param string $privKey
     * @param string $label
     * @return bool
     */
    public function importPrivateKey(string $privKey, string $label) : bool;

    /**
     * Returns $count array of most recent transactions skipping the first $from transactions for $address.
     *
     * @see \Atlas\DTO\TransactionDTO
     *
     * @param string $address
     * @param int $count
     * @param int $from
     * @return array
     */
    public function listTransactions(string $address, int $count = 10, int $from = 0) : array;

    /**
     * $amount is a rounded to 8 decimal places.
     * Will send the given amount to the given address, ensuring the account has a valid balance using $minconf confirmations.
     * Returns the transaction ID if successful.
     *
     * @see \Atlas\DTO\TransactionDTO
     *
     * @param string $sourceAccount
     * @param string $destinationAccount
     * @param int $amount
     * @param int $minConf
     * @param null $msg
     * @return \Atlas\DTO\TransactionDTO
     */
    public function sendFrom(string $sourceAccount, string $destinationAccount, int $amount, int $minConf = 1, $msg = null) : \Atlas\DTO\TransactionDTO;

    /**
     * Same as sendFrom(), but takes an array of multiple destination addresses.
     * Returns an array of transaction objects.
     *
     * @see \Atlas\DTO\TransactionDTO
     *
     * @param string $string
     * @param array $addresses
     * @param int $amount
     * @param int $minConf
     * @param null $msg
     * @return array
     */
    public function sendMany(string $string, array $addresses, int $amount, int $minConf = 1, $msg = null) : array;

    /**
     * Submits raw transaction (that has been serialized as a hex coded string) to the node.
     *
     * @see \Atlas\DTO\TransactionDTO
     *
     * @param string $hex
     * @return \Atlas\DTO\TransactionDTO
     */
    public function sendRawTransaction(string $hex) : \Atlas\DTO\TransactionDTO;

    /**
     * $amount is rounded to 8 decimal places.
     * Returns a transaction object.
     *
     * @see \Atlas\DTO\TransactionDTO
     *
     * @param string $address
     * @param int $amount
     * @param string|null $msg
     * @return \Atlas\DTO\TransactionDTO
     */
    public function sendToAddress(string $address, int $amount, string $msg = null) : \Atlas\DTO\TransactionDTO;

    /**
     * Sets default tx fee for transactions.
     * is a real and is rounded to the nearest 0.00000001
     *
     * @param float $amount
     * @return bool
     */
    public function setTxFee(float $amount = 0.00000001) : bool;

    /**
     * Sign a message with the private key of an address.
     * Returns the message signature.
     *
     * @param string $address
     * @param string $msg
     * @return string
     */
    public function signMessage(string $address, string $msg) : string;

    /**
     * Adds signatures to a raw transaction and returns the resulting raw transaction.
     *
     * @param string $hex
     * @param \Atlas\DTO\RawTransactionDTO $rawTransaction
     * @return \Atlas\DTO\RawTransactionDTO
     */
    public function signRawTransactions(string $hex, \Atlas\DTO\RawTransactionDTO $rawTransaction) : \Atlas\DTO\RawTransactionDTO;

    /**
     * Attempts to validate an address format is correct.
     *
     * @param string $address
     * @return bool
     */
    public function validateAddress(string $address) : bool;

    /**
     * Verify a signed message.
     *
     * @param string $address
     * @param string $signature
     * @param string $message
     * @return bool
     */
    public function verifyAddress(string $address, string $signature, string $message) : bool;

    /**
     * Removes the wallet encryption key from memory, locking the wallet.
     * After calling this method, you will need to call wallet passphrase again before being able to call any methods which require the wallet to be unlocked.
     *
     * @return bool
     */
    public function walletLock() : bool;

    /**
     * Stores the wallet decryption key in memory for $timeout seconds.
     *
     * @param string $passPhrase
     * @param int $timeout
     * @return bool
     */
    public function walletPassPhrase(string $passPhrase, int $timeout = 3000) : bool;

    /**
     * Changes the wallet pass phrase from $oldPassPhrase to $newPassPhrase.
     *
     * @param string $oldPassPhrase
     * @param string $newPassPhrase
     * @return bool
     */
    public function walletPassPhraseChange(string $oldPassPhrase, string $newPassPhrase) : bool;
}