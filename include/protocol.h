#ifndef REQLY_PROTOCOL_H
#define REQLY_PROTOCOL_H

#include <stddef.h>

/**
 * Supported protocol identifiers.
 * Designed to be easily extensible for future protocols (WebSocket, gRPC, etc.).
 */
typedef enum {
    REQLY_PROTOCOL_HTTP = 0,
    REQLY_PROTOCOL_COUNT
} ReqlyProtocolId;

/**
 * Protocol definition descriptor.
 */
typedef struct {
    ReqlyProtocolId id;
    const char *name;
    const char *default_scheme;
} ReqlyProtocol;

/**
 * Returns the total number of registered protocols.
 */
size_t reqly_protocol_get_count(void);

/**
 * Gets protocol descriptor by its identifier.
 */
const ReqlyProtocol *reqly_protocol_get_by_id(ReqlyProtocolId id);

/**
 * Gets protocol descriptor by index (0 .. count - 1).
 */
const ReqlyProtocol *reqly_protocol_get_by_index(size_t index);

/**
 * Finds protocol descriptor by name (case-insensitive, e.g. "HTTP").
 */
const ReqlyProtocol *reqly_protocol_find_by_name(const char *name);

/**
 * Finds matching protocol descriptor based on a URL scheme.
 */
const ReqlyProtocol *reqly_protocol_find_by_url(const char *url);

/**
 * Returns a NULL-terminated list of protocol display names for UI dropdowns.
 */
const char * const *reqly_protocol_get_names(void);

/**
 * Gets the methods supported by a given protocol.
 *
 * @param id Protocol identifier.
 * @param out_count Optional pointer to receive method count.
 * @return NULL-terminated array of method name strings.
 */
const char * const *reqly_protocol_get_methods(ReqlyProtocolId id, size_t *out_count);

#endif /* REQLY_PROTOCOL_H */
