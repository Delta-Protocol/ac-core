<?php

namespace Atlas\DTO;

/**
 * Abstract object describing a transaction.
 *
 * @author James Kirby <james.kirby@atlascityfinance.com>
 * @package Atlas\DTO
 * @copyright MIT
 */
class TransactionDTO
{
    /**
     * The transaction id.
     *
     * @string
     */
    public $txid;

    /**
     * The destination address.
     * @string
     */
    public $address;

    /**
     * The transaction value.
     *
     * @float
     */
    public $amount;

}