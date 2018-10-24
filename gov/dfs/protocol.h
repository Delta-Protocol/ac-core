#ifndef US_GOV_DFS_PROOCOL_H
#define US_GOV_DFS_PROOCOL_H

#include <inttypes.h>
#include <us/gov/relay/protocol.h>

namespace us{ namespace gov{ namespace protocol{

static constexpr uint16_t dfs_base{500};
static constexpr uint16_t file_request{((dfs_base+0)<<2)+query_suffix};
static constexpr uint16_t file_response{((dfs_base+1)<<2)+query_suffix};

}}}

#endif
