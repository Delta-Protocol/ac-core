<?php

namespace Atlas\DTO;

/**
 * Abstract object describing the response of getBlock().
 *
 * @author James Kirby <james.kirby@atlascityfinance.com>
 * @package Atlas\DTO
 * @copyright MIT
 */
class BlockInfoDTO
{

    /**
     * The block hash.
     *
     * @string
     */
    public $hash;

    /**
     * The block height integer.
     *
     * @int
     */
    public $blockHeight;

    /**
     * The block size.
     *
     * @int
     */
    public $size;

    /**
     * The block version (semver format).
     *
     * @string
     */
    public $version = '0.0.1';

    /**
     * Array of accounts that have had balance diffs.
     *
     * @var array
     */
    public $accounts = [];

    /**
     * The block time in seconds since epoch (Jan 1 1970 GMT)
     *
     * @int
     */
    public $time;

    /**
     * The numeric nonce.
     *
     * @int
     */
    public $nonce;

    /**
     * The hash of the previous block.
     *
     * @string
     */
    public $previousBlockHash;
}