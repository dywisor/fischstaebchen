## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

STAGEDIVE_CONFIG_ROOT="${INIT_HOOKDIR:?}/stagedive"
STAGEDIVE_PROFILES_DIR="${STAGEDIVE_CONFIG_ROOT}/profiles"
STAGEDIVE_HOOKS_SRCDIR="${STAGEDIVE_CONFIG_ROOT}/components"
STAGEDIVE_BOOTSTRAP_SRCDIR="${STAGEDIVE_CONFIG_ROOT}/bootstrap"

STAGEDIVE_CLEANUP_HOOKDIR="y"

STAGEDIVE_RUNTIME_CONFDIR="${ISHARE_CFG:?}/stagedive"
STAGEDIVE_BOOTSTRAP_CFG="${STAGEDIVE_RUNTIME_CONFDIR}/bootstrap_cfg.sh"
