# AES-128 (educational implementation)

A from-scratch implementation of the AES-128 block cipher in C, written to understand how the algorithm works step by step.

> ## ⚠️ Educational use only
>
> **Do not use this code to protect real data.** It was written to learn and to show the internals of AES, not to be secure. See [Known limitations](#known-limitations) for the technical reasons. For real applications, use a well-reviewed library such as OpenSSL, libsodium or the platform's crypto API.

## What it does

- Implements the AES-128 round structure from [FIPS-197](https://csrc.nist.gov/pubs/fips/197/final): `AddRoundKey`, `SubBytes`, `ShiftRows`, `MixColumns`
- Generates the S-box at runtime (multiplicative inverse in GF(2^8) + affine transformation) instead of using a hardcoded table
- Implements the key expansion algorithm for the 11 round keys

## Build

```sh
# TODO: adjust file names to the repo layout
gcc -Wall -Wextra -O2 -o aes main.c
```

## Usage

```sh
./aes <key:16 bytes> <message:16 bytes>
./aes --h
./aes --help
```

Both `<key>` and `<message>` must be exactly **16 bytes**. Use quotes if they contain spaces:

```sh
./aes 'Thats my Kung Fu' 'Two One Nine Two'
```

For this input the expected ciphertext is `29C3505F571420F6402299B31A02D73A`.

## Known limitations

This project intentionally ignores most of what makes a cryptographic implementation safe:

- **Not constant-time.** The S-box is accessed with indexes derived from secret data (cache-timing side channel), and `gf_mul` branches on secret values. Both leak information through timing.
- **No mode of operation.** It processes a single 16-byte block, equivalent to ECB: identical plaintext blocks give identical ciphertext blocks, which leaks patterns. There is no IV/nonce, no padding and no chaining.
- **No authentication.** There is no MAC or AEAD (e.g. GCM), so ciphertext can be modified undetected.
- **Weak key handling.** Keys are raw ASCII strings passed on the command line, so they have low entropy and are visible in the process list and shell history. There is no key derivation function (e.g. PBKDF2, Argon2).
- **Secrets are never wiped.** Keys, round keys and intermediate states stay in memory (stack) after use, and debug output prints intermediate values.
- **Not thread-safe.** The lazily initialized S-box has no synchronization.
- **Not validated.** It has been checked against a worked example only, not against the full NIST test vectors (CAVP), and has had no security review or fault/side-channel analysis.

## References

- [FIPS-197: Advanced Encryption Standard (AES)](https://csrc.nist.gov/pubs/fips/197/final)
- [NIST SP 800-38A: Block cipher modes of operation](https://csrc.nist.gov/pubs/sp/800/38/a/final)

## License

Released under the [MIT License](LICENSE).
 
This software is provided "as is", without warranty of any kind. The authors are not liable for any damage, data loss or security incident resulting from its use, and in particular from using it outside its educational purpose (e.g. to protect real data), as explained in [Known limitations](#known-limitations).