/*
 Legal Notice: Some portions of the source code contained in this file were
 derived from the source code of TrueCrypt 7.1a, which is
 Copyright (c) 2003-2012 TrueCrypt Developers Association and which is
 governed by the TrueCrypt License 3.0, also from the source code of
 Encryption for the Masses 2.02a, which is Copyright (c) 1998-2000 Paul Le Roux
 and which is governed by the 'License Agreement for Encryption for the Masses'
 Modifications and additions to the original source code (contained in this file)
 and all other portions of this file are Copyright (c) 2013-2025 AM Crypto
 and are governed by the Apache License 2.0 the full text of which is
 contained in the file License.txt included in VeraCrypt binary and source
 code distribution packages. */


#include "Crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RNG defines & pool pointers */
#define RNG_POOL_SIZE	320	// Must be divisible by the size of the output of each of the implemented hash functions. (in bytes)

#if RNG_POOL_SIZE % SHA512_DIGESTSIZE || RNG_POOL_SIZE % WHIRLPOOL_DIGESTSIZE || RNG_POOL_SIZE % BLAKE2S_DIGESTSIZE
#error RNG_POOL_SIZE must be divisible by the size of the output of each of the implemented hash functions.
#endif

#define RANDOMPOOL_ALLOCSIZE	RNG_POOL_SIZE

// After every RANDMIX_BYTE_INTERVAL-th byte written to the pool, the pool mixing function is applied to the entire pool
#define RANDMIX_BYTE_INTERVAL	16

// FastPoll interval (in milliseconds)
#define FASTPOLL_INTERVAL		500

void RandAddInt ( unsigned __int32 x );
int Randinit ( void );
int RandinitWithCheck ( int* pAlreadyInitialized);
void RandStop (BOOL freePool);
BOOL IsRandomNumberGeneratorStarted ();
void RandSetHashFunction ( int hash_algo_id );
int RandGetHashFunction (void);
void SetRandomPoolEnrichedByUserStatus (BOOL enriched);
BOOL IsRandomPoolEnrichedByUser ();
BOOL Randmix ( void );
void RandaddBuf ( void *buf , int len );
BOOL FastPoll ( void );
BOOL SlowPoll ( void );

/* Get len random bytes from the pool (max. RNG_POOL_SIZE bytes per a single call) */
BOOL RandgetBytes ( void* hwndDlg, unsigned char *buf , int len, BOOL forceSlowPoll );

/* Get len random bytes from the pool.
 *  If allowAnyLength is FALSE, then len must be less or equal to RNG_POOL_SIZE
 *  If allowAnyLength is TRUE, then len can have any positive value
 */
BOOL RandgetBytesFull ( void* hwndDlg, unsigned char *buf , int len, BOOL forceSlowPoll , BOOL allowAnyLength);

#ifdef _WIN32

extern BOOL volatile bFastPollEnabled;
extern BOOL volatile bRandmixEnabled;
extern DWORD CryptoAPILastError;
extern DWORD ProcessedMouseEventsCounter;

BOOL RandpeekBytes ( void* hwndDlg, unsigned char *buf , int len, DWORD* mouseCounter );
void RandAddInt64 ( unsigned __int64 x );

LRESULT CALLBACK MouseProc ( int nCode , WPARAM wParam , LPARAM lParam );
LRESULT CALLBACK KeyboardProc ( int nCode , WPARAM wParam , LPARAM lParam );
static unsigned __stdcall PeriodicFastPollThreadProc (void *dummy);

#endif

#ifdef __cplusplus
}
#endif
