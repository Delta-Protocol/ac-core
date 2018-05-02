<?php

namespace Atlas\DTO;

/**
 * Abstract object describing a multi-sig wallet.
 *
 * @author James Kirby <james.kirby@atlascityfinance.com>
 * @package Atlas\DTO
 * @copyright MIT
 */
class MultiSigDTO
{

    /**
     * array of public keys in the multi-sig address.
     *
     * @array
     */
    public $addresses = [];
}