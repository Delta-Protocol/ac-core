<?php

namespace Atlas\DTO;

/**
 * Abstract object describing an addresses balance and information.
 *
 * @author James Kirby <james.kirby@atlascityfinance.com>
 * @package Atlas\DTO
 * @copyright MIT
 */
class ReceivedByAddressDTO
{

    /**
     * The receiving address.
     *
     * @string
     */
    public $address;

    /**
     * Total amount received by addresses with this account.
     *
     * @float
     */
    public $amount;

    /**
     * Number of confirmations of the most recent transaction included.
     *
     * @int
     */
    public $confirmations;

}