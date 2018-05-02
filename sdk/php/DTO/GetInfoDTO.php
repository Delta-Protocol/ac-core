<?php

namespace Atlas\DTO;

/**
 * Abstract object describing the response of getInfo().
 *
 * @author James Kirby <james.kirby@atlascityfinance.com>
 * @package Atlas\DTO
 * @copyright MIT
 */
abstract class GetInfoDTO extends \Atlas\Abstracts\Node
{
    /**
     * Number of blocks in history
     *
     * @int
     */
    public $blocks;

    /**
     * List of un-resolved errors with current node.
     *
     * @array
     */
    public $errors = [];

    /**
     * Current set transaction fee.
     *
     * @int
     */
    public $txFee;

    /**
     * Outgoing proxy. In the format of <address:port>
     *
     * @string
     */
    public $proxy;

    /**
     * Bool to say weather we are in testnet or not.
     *
     * @bool
     */
    public $network;

    /**
     * Boolean to show if node has walletD module running.
     *
     * @bool
     */
    public $WalletMode;

    /**
     * If wallet is running in wallet mode give the version of the walletD (semver format).
     *
     * @string
     */
    public $walletDVersion;
}