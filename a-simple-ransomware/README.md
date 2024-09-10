# A simple ransomware

## This repository is for study purposes only.

### How it works

A small example of ransomware written in C++ that uses the AES-128 algorithm to encrypt all files in a specific folder.
To test it, modify the `directoryPath` variable in the `main()` function, build the project, and run the generated executable.

In the data folder there are two plaintext files and their corresponding encrypted files to be used as examples.

**Warning:** The plaintext files are deleted, and only the encrypted files remain available. Both the encryption key and IV are set to 12345678123456781234567812345678.

### Decrypting test

`openssl enc -aes-128-cbc -d -in DivinaCommedia.txt.enc -out DivinaCommedia.dec.txt -K 12345678123456781234567812345678  -iv 12345678123456781234567812345678`