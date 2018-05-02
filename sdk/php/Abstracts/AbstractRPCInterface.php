<?php

namespace Atlas\Abstracts;

/**
 * Abstract interface for all daemon RPC layers..
 *
 * @author James Kirby <james.kirby@atlascityfinance.com>
 * @package Atlas\Abstracts
 * @copyright MIT
 */
interface AbstractRPCInterface
{
    /**
     * Return a list of commands that can be used with this daemon.
     *
     * @return array
     */
    public function help() : array;
}