/*
 * This is an OpenSSL pseudo compatible implementation of the NIST
 * SHA1 Message Digest algorithm (RFC3174).
 *
 * It is largely insipred from public domain code by By Steve Reid <steve@edmweb.com>
 */

#ifndef SHA1_H
#define SHA1_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdint.h>

typedef struct
{
    uint32_t state[5];
    uint32_t count[2];
    unsigned char buffer[64];
} SHA1_CTX;

void SHA1_Init(SHA1_CTX * context);
void SHA1_Update(SHA1_CTX * context, const unsigned char *data, uint32_t len);
void SHA1_Final(unsigned char *result, SHA1_CTX * context);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
#endif /* SHA1_H */