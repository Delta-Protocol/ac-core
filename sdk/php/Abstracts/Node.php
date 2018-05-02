<?php

namespace Atlas\Abstracts;

/**
 * Abstract class describing the Node Object.
 *
 * @author James Kirby <james.kirby@atlascityfinance.com>
 * @package Atlas\Abstracts
 * @copyright MIT
 */
abstract class Node
{

    /**
     * Public key of the node.
     *
     * @string
     */
    public $publicKey;

    /**
     * IPV4 of the node.
     *
     * @string
     */
    public $ipv4;

    /**
     * Number of active connections to the current node.
     *
     * @int
     */
    public $connections;

    /**
     * Version of nodes coind (semver format)
     *
     * @string
     */
    public $coinDVersion;
}