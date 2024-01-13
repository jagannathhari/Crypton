#define LIBSODIUM
#include <sodium.h>

#include <stdio.h>
#include <string.h>
#include "encrypter.h"
#include "file_util.h"
#define CHUNK_SIZE 4096
#define KEY_LEN crypto_box_SEEDBYTES
#define SALT_LEN crypto_pwhash_SALTBYTES

static int generate_key(unsigned char *const out, unsigned long long outlen,
                 const char *const passwd, const unsigned char *const salt){
    if(crypto_pwhash(out,outlen,passwd,strlen(passwd),salt,crypto_pwhash_OPSLIMIT_INTERACTIVE,
                crypto_pwhash_MEMLIMIT_INTERACTIVE,crypto_pwhash_ALG_DEFAULT) != 0){
        return -1;
    }
    return 0;
}


float get_progress(long file_size, FILE *file){
       return ((float)ftell(file)/(float)file_size) * 100.0f; 
}

int encrypt(const char *target_file, const char *source_file,
                   const char *password,float *progress) {

    if (sodium_init() < 0) {
        return -1;
    }
    unsigned char salt[SALT_LEN];
    unsigned char key[KEY_LEN];
    long file_size;
    randombytes_buf(salt, sizeof salt);
    if(generate_key(key,sizeof key,password,salt)!=0){
        return -1;
    } 

    unsigned char buf_in[CHUNK_SIZE];
    unsigned char
        buf_out[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];

    crypto_secretstream_xchacha20poly1305_state st;
    FILE *file_target, *file_source;
    unsigned long long out_len;
    size_t rlen;
    int eof;
    unsigned char tag;

    file_source = fopen(source_file, "rb");
    file_target = fopen(target_file, "wb");
    file_size = get_file_size(file_source);

    crypto_secretstream_xchacha20poly1305_init_push(&st, header, key);
    fwrite(salt,1,sizeof salt, file_target);
    fwrite(header, 1, sizeof header, file_target);
    do {
        rlen = fread(buf_in, 1, sizeof buf_in, file_source);
        eof = feof(file_source);
        tag = eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
        crypto_secretstream_xchacha20poly1305_push(&st, buf_out, &out_len,
                                                   buf_in, rlen, NULL, 0, tag);
        fwrite(buf_out, 1, (size_t)out_len, file_target);
        *progress = get_progress(file_size,file_source);
    } while (!eof);
    fclose(file_target);
    fclose(file_source);
    return 0;
}

int decrypt(
    const char *target_file, const char *source_file,const char *password,float *progress){
    unsigned char
        buf_in[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char buf_out[CHUNK_SIZE];
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    unsigned char salt[SALT_LEN];
    unsigned char key[KEY_LEN];
    FILE *fp_t, *fp_s;
    unsigned long long out_len;
    size_t rlen;
    int eof;
    int ret = -1;
    unsigned char tag;
    long file_size;
    fp_s = fopen(source_file, "rb");
    fp_t = fopen(target_file, "wb");
    file_size = get_file_size(fp_s);
    fread(salt,1,sizeof salt, fp_s);
    fread(header, 1, sizeof header, fp_s);

    if(generate_key(key,sizeof key,password,salt)!=0){
        ret  = -1;
        goto ret;
    } 

    if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header, key) !=
        0) {
        goto ret; /* incomplete header */
    }
    do {
        rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        eof = feof(fp_s);
        if (crypto_secretstream_xchacha20poly1305_pull(
                &st, buf_out, &out_len, &tag, buf_in, rlen, NULL, 0) != 0) {
            goto ret; /* corrupted chunk */
        }
        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL) {
            if (!eof) {
                goto ret; /* end of stream reached before the end of the file */
            }
        } else { /* not the final chunk yet */
            if (eof) {
                goto ret; /* end of file reached before the end of the stream */
            }
        }
        fwrite(buf_out, 1, (size_t)out_len, fp_t);
        *progress = get_progress(file_size,fp_s);
    } while (!eof);

    ret = 0;
ret:
    fclose(fp_t);
    fclose(fp_s);
    return ret;
}

