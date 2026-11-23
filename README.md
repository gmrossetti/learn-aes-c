# AES-128 (educational implementation)

A from-scratch implementation of the AES-128 block cipher in C, written to understand how the algorithm works step by step.

> ## ⚠️ Educational use only
>
> **Do not use this code to protect real data.** It was written to learn and to show the internals of AES, not to be secure. See [Known limitations](#known-limitations) for the technical reasons. For real applications, use a well-reviewed library such as OpenSSL, libsodium or the platform's crypto API.

## What is AES?
 
AES (Advanced Encryption Standard) is the most widely used **symmetric encryption** algorithm: the same secret key is used to encrypt and to decrypt. It was standardized by NIST in 2001 ([FIPS-197](https://csrc.nist.gov/pubs/fips/197/final)) and is based on the Rijndael cipher by Joan Daemen and Vincent Rijmen.
 
AES is a **block cipher**: it works on fixed blocks of 16 bytes (128 bits). The key can be 128, 192 or 256 bits long; this project implements **AES-128**, which uses a 16-byte key and 10 rounds.
 
### How it works, in short
 
The 16 input bytes are arranged in a 4x4 matrix called the **state**. The key is first expanded into 11 round keys (one per round plus an initial one). Then:
 
1. **Initial round**: `AddRoundKey`
2. **Rounds 1-9**: `SubBytes`, `ShiftRows`, `MixColumns`, `AddRoundKey`
3. **Final round (10)**: `SubBytes`, `ShiftRows`, `AddRoundKey` (no `MixColumns`)

| Step | What it does | Why |
|------|--------------|-----|
| `SubBytes` | Replaces every byte using a fixed lookup table (the S-box) | Makes the relation between key and ciphertext non-linear (*confusion*) |
| `ShiftRows` | Rotates row *n* of the matrix left by *n* positions | Mixes bytes across columns |
| `MixColumns` | Multiplies each column by a fixed matrix in GF(2^8) | Spreads each byte's influence over its whole column (*diffusion*) |
| `AddRoundKey` | XORs the state with the round key | Injects the secret key |
| Key expansion | Derives the 11 round keys from the original key | Gives every round a different key |
 
After a few rounds, changing one bit of the input or the key changes about half of the output bits.
 
<!-- A worked example is used throughout the code: key `Thats my Kung Fu` and message `Two One Nine Two`. The program prints the state after every step, so each round can be followed by hand. -->

## What it does

- Implements the AES-128 round structure from [FIPS-197](https://csrc.nist.gov/pubs/fips/197/final): `AddRoundKey`, `SubBytes`, `ShiftRows`, `MixColumns`
- Generates the S-box at runtime (multiplicative inverse in GF(2^8) + affine transformation) instead of using a hardcoded table
- Implements the key expansion algorithm for the 11 round keys

## Build
 
```sh
make           # builds ./aes
make debug     # build with AddressSanitizer/UBSan (run `make clean` first)
make test         # Build and run unit tests
make clean
```

## Usage

```sh
./aes <key:16 bytes> <message:16 bytes>
./aes -h
./aes --help
```

Both `<key>` and `<message>` must be exactly **16 bytes**. Use quotes if they contain spaces:

```sh
./aes 'Thats my Kung Fu' 'Two One Nine Two'
```

For this input the expected ciphertext is `0x29C3505F571420F6402299B31A02D73A`.

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