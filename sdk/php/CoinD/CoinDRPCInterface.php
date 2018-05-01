<?php

namespace Atlas\CoinD;

/**
 * Interface for the coinD RPC API.
 *
 * @author James Kirby <james.kirby@atlascityfinance.com>
 * @package Atlas\CoinD
 * @copyright MIT
 */
interface CoinDRPCInterface extends \Atlas\Abstracts\AbstractRPCInterface
{
    /**
     * Attempts add or remove (based on the value of $action) $node from the addnode list.
     *
     * @param \Atlas\Abstracts\Node $node
     * @param string $action
     * @return bool
     */
    public function addNode(\Atlas\Abstracts\Node $node, string $action) : bool;

    /**
     * Returns an array of Node Objects.
     *
     * @see \Atlas\Abstracts\Node
     *
     * @return array
     */
    public function getAddedNodeInfo() : array;

    /**
     * Returns the hash of the best (tip) block in the longest block chain.
     *
     * @return string
     */
    public function getBestBlockHash() : string;

    /**
     * Return information about the given block.
     *
     * @param string $hash
     * @return string
     */
    public function getBlock(string $hash) : \Atlas\DTO\BlockInfoDTO;

    /**
     * Returns the number of blocks in the longest block chain.
     *
     * @return int
     */
    public function getBlockCount() : int;

    /**
     * Returns the number of connections to other nodes.
     *
     * @return int
     */
    public function getConnectionCount() : int;

    /**
     * Return an object containing various state information.
     *
     * @return string
     */
    public function getInfo() : \Atlas\DTO\GetInfoDTO;

    /**
     * Returns an array of node objects for each node you are connected to.
     *
     * @see \Atlas\Abstracts\Node
     *
     * @return array
     */
    public function getPeerInfo() : array;

    /**
     * Permanently marks a block as invalid, as if it violated a consensus rule.
     *
     * @param string $hash
     * @return bool
     */
    public function invalidateBlock(string $hash) : bool;
}
