#ifndef ENCRYPTER_H
#define ENCRYPTER_H

int encrypt(const char *target_file, const char *source_file,
                   const char *password,float *progress);
int decrypt(
    const char *target_file, const char *source_file,const char *password,float *progress);

#endif
