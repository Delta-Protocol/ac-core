<?php

namespace Atlas\DTO;

/**
 * Abstract object describing a raw transaction.
 *
 * @author James Kirby <james.kirby@atlascityfinance.com>
 * @package Atlas\DTO
 * @copyright MIT
 */
class RawTransactionDTO
{
    /**
     * The transaction id.
     *
     * @string
     */
    public $txid;

    /**
     * @int
     */
    public $vout;

    /**
     * The destination address.
     * @string
     */
    public $address;

    /**
     * The number of confirmations a transaction has.
     *
     * @int
     */
    public $confirmations;

    /**
     * The transaction value.
     *
     * @float
     */
    public $amount;

}