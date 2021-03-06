/* aes.c
 * Code was written: June 7, 2017
 *
 * aes.c implements AES-128, AES-192, and AES-256 for RLCE
 * Copyright (C) 2017 Yongge Wang
 * 
 * Yongge Wang
 * Department of Software and Information Systems
 * UNC Charlotte
 * Charlotte, NC 28223
 * yonwang@uncc.edu
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define SIZE_CHAR sizeof(unsigned char)
#define SIZE_4CHAR 4*sizeof(unsigned char)
#define SIZE_16CHAR 16*sizeof(unsigned char)
#define NUM_OF_T 500000
 
typedef struct AESkey {
  unsigned short Nk;  //Number of words in the key
  unsigned short Nr;  //Number of rounds in the encryption algorithm
  unsigned short Nb;  //Number of words in the block
  unsigned short wLen;
  unsigned short keylen;
  unsigned char * key;
} * aeskey_t;
 
void aeskey_free(aeskey_t key) {
  free(key->key);
  free(key);
  return;
}
 
void AES_encrypt(unsigned char*, unsigned char*, aeskey_t);
void AES_decrypt(unsigned char*, unsigned char*, aeskey_t);
void testEncryption(unsigned char*, unsigned char*,aeskey_t, unsigned char*,int);
void testDecryption(unsigned char*, unsigned char*,aeskey_t, unsigned char*,int);
//void round(unsigned char*);
aeskey_t aeskey_init(unsigned short);
 
int main (int argc, char *argv[]) {
  unsigned char msg[]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
               0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
  unsigned char decryptedmsg[16];
  unsigned char cipher[16];
    
  aeskey_t key=aeskey_init(128);  
  unsigned char keystring[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
  memcpy(key->key, keystring, 16);
  unsigned char expectedcipher[]={0x69,0xc4,0xe0,0xd8,0x6a,0x7b,0x04,0x30,
                0xd8,0xcd,0xb7,0x80,0x70,0xb4,0xc5,0x5a};
  testEncryption(msg, cipher,key,expectedcipher,NUM_OF_T);
  testDecryption(msg, cipher,key,decryptedmsg,NUM_OF_T);
  aeskey_free(key);
 
  key=aeskey_init(192);
  char keystring192[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,
               0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17};
  memcpy(key->key,keystring192,24);
  unsigned char expectedcipher192[]={0xdd,0xa9,0x7c,0xa4,0x86,0x4c,0xdf,0xe0,
                     0x6e,0xaf,0x70,0xa0,0xec,0x0d,0x71,0x91};
  testEncryption(msg, cipher,key,expectedcipher192,NUM_OF_T);
  testDecryption(msg, cipher,key,decryptedmsg,NUM_OF_T);
  aeskey_free(key);
   
  key=aeskey_init(256);
  unsigned char keystring256[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
                0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
                0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
  memcpy(key->key, keystring256, 32);
  unsigned char expectedcipher256[]={0x8e,0xa2,0xb7,0xca,0x51,0x67,0x45,0xbf,
                     0xea,0xfc,0x49,0x90,0x4b,0x49,0x60,0x89};
  testEncryption(msg, cipher,key,expectedcipher256,NUM_OF_T);
  testDecryption(msg, cipher,key,decryptedmsg,NUM_OF_T);
  aeskey_free(key); 
  exit(0);
}
 
void testEncryption(unsigned char msg[], unsigned char cipher[],aeskey_t key, unsigned char expectedcipher[],int numofT) {
  int i, failed=0;
  clock_t start, finish;
  double seconds;
  AES_encrypt(msg, cipher, key);
  for (i=16;i--;) {
    if (expectedcipher[i] != cipher[i]){
      failed =1;
    }
  }
  if (failed >0) {
    printf("AES-%d encryption failed\n", (key->keylen)*8);
  } else {
    start = clock();
    for (i=numofT;i--;) {
      AES_encrypt(msg, cipher,key);
    }
    finish = clock();
    seconds = ((double)(finish - start))/CLOCKS_PER_SEC;
    printf("%f seconds for %d times of AES-%d encryption\n",seconds,numofT,(key->keylen)*8);
  }
}
 
void testDecryption(unsigned char msg[], unsigned char cipher[],aeskey_t key, unsigned char decryptedmsg[],int numofT) {
  int i, failed=0;
  clock_t start, finish;
  double seconds;
  AES_decrypt(cipher,decryptedmsg, key);
  for (i=16;i--;) {
    if (decryptedmsg[i] != msg[i])  {
      failed =1;
    }
  }
 
  if (failed >0) {
    printf("AES-%d decryption failed\n",(key->keylen)*8);
  } 
  else {
    start = clock();
    for (i=numofT;i--;) {
      AES_decrypt(cipher,decryptedmsg, key);
    }
    finish = clock();
    seconds = ((double)(finish - start))/CLOCKS_PER_SEC;
    printf("%f seconds for %d times of AES-%d decryption\n",seconds,numofT,(key->keylen)*8);    
  }
}
 
/*
* The substitution box of the cipher.  Allows an element of GF(2^8) to map
* to its inverse.
*/
static const unsigned char sbox[256] ={
  0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
  0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
  0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
  0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
  0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
  0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
  0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
  0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
  0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
  0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
  0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
  0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
  0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
  0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
  0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
  0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};
 
/*
* The inverse substitution box of the cipher.  Allows an element of GF(2^8) to map
* to its inverse during the decryption process.
*/ 
static const unsigned char Invsbox[256] ={
  0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
  0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
  0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
  0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
  0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
  0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
  0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
  0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
  0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
  0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
  0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
  0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
  0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
  0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
  0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
  0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};
 
static const unsigned char galoisInvBox[256] = {
  0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,
  0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
  0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
  0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,
  0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,
  0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,
  0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,
  0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,
  0x1b,0x19,0x1f,0x1d,0x13,0x11,0x17,0x15,0x0b,0x09,0x0f,0x0d,0x03,0x01,0x07,0x05,
  0x3b,0x39,0x3f,0x3d,0x33,0x31,0x37,0x35,0x2b,0x29,0x2f,0x2d,0x23,0x21,0x27,0x25,
  0x5b,0x59,0x5f,0x5d,0x53,0x51,0x57,0x55,0x4b,0x49,0x4f,0x4d,0x43,0x41,0x47,0x45,
  0x7b,0x79,0x7f,0x7d,0x73,0x71,0x77,0x75,0x6b,0x69,0x6f,0x6d,0x63,0x61,0x67,0x65,
  0x9b,0x99,0x9f,0x9d,0x93,0x91,0x97,0x95,0x8b,0x89,0x8f,0x8d,0x83,0x81,0x87,0x85,
  0xbb,0xb9,0xbf,0xbd,0xb3,0xb1,0xb7,0xb5,0xab,0xa9,0xaf,0xad,0xa3,0xa1,0xa7,0xa5,
  0xdb,0xd9,0xdf,0xdd,0xd3,0xd1,0xd7,0xd5,0xcb,0xc9,0xcf,0xcd,0xc3,0xc1,0xc7,0xc5,
  0xfb,0xf9,0xff,0xfd,0xf3,0xf1,0xf7,0xf5,0xeb,0xe9,0xef,0xed,0xe3,0xe1,0xe7,0xe5
};
 
static const unsigned char Rcon[11] = {0x8d,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36};
 
aeskey_t aeskey_init(unsigned short kappa) {
  if ((kappa!=128)&&(kappa!=192)&&(kappa!=256)){
    return NULL;
  }
  aeskey_t out;
  out = (aeskey_t) malloc(sizeof(struct AESkey));
  out->keylen= kappa/8;
  out->Nk = kappa/32;
  switch(out->Nk) {
  case 4:
    out->Nr=10;
    out->wLen = 176; 
    break;
  case 6:
    out->Nr=12;
    out->wLen = 208;
    break;
  case 8:
    out->Nr=14;
    out->wLen = 240;
    break;
  default:
    return NULL;
  }
  out->key = (unsigned char *) calloc(kappa/8, SIZE_CHAR);
  return out;
}
 
/*
* Creates a round key
*/
int KeyExpansion(aeskey_t key, unsigned char w[]){
  unsigned short i, iNk;
  unsigned char temp[4];
  unsigned char tmp;
  unsigned short Nk = key->Nk;
  unsigned short Nr= key->Nr;
  memcpy(w, key->key, 4*Nk*SIZE_CHAR);
  for (i = Nk; i<4*(Nr+1); ++i){
    iNk = i%Nk;
    memcpy(temp, &w[4*(i-1)], 4);
    if ((iNk) == 0) {
      tmp = temp[0];
      temp[0]=sbox[temp[1]];
      temp[1]=sbox[temp[2]];
      temp[2]=sbox[temp[3]];
      temp[3]=sbox[tmp];
      temp[0] ^= Rcon[i/Nk];
    } else if ((Nk==8) && ((iNk)==4)) {
      temp[0]= sbox[temp[0]];
      temp[1]= sbox[temp[1]];
      temp[2]= sbox[temp[2]];
      temp[3]= sbox[temp[3]];
    }
    w[4*i] = w[4*(i-Nk)] ^ temp[0];
    w[4*i+1] = w[4*(i-Nk)+1] ^ temp[1];
    w[4*i+2] = w[4*(i-Nk)+2] ^ temp[2];
    w[4*i+3] = w[4*(i-Nk)+3] ^ temp[3];
  }
  return 0;
}
 
/*
* Maps an array of byte representations of elements of GF(2^8) to 
* their inverses.
*/
static void SubBytes(unsigned char cipher[]) {
  cipher[0]=sbox[cipher[0]];
  cipher[1]=sbox[cipher[1]];
  cipher[2]=sbox[cipher[2]];
  cipher[3]=sbox[cipher[3]];
  cipher[4]=sbox[cipher[4]];
  cipher[5]=sbox[cipher[5]];
  cipher[6]=sbox[cipher[6]];
  cipher[7]=sbox[cipher[7]];
  cipher[8]=sbox[cipher[8]];
  cipher[9]=sbox[cipher[9]];
  cipher[10]=sbox[cipher[10]];
  cipher[11]=sbox[cipher[11]];
  cipher[12]=sbox[cipher[12]];
  cipher[13]=sbox[cipher[13]];
  cipher[14]=sbox[cipher[14]];
  cipher[15]=sbox[cipher[15]];
}

/*
* Shifts each row of the block cyclically to the left.  Indices that
* are equivalent modulo 4 are on the same row of the block.
*/
static void ShiftRows(unsigned char cipher[]) {
  unsigned char temp;
  temp = cipher[1];
  cipher[1]=cipher[5];
  cipher[5]=cipher[9];
  cipher[9]=cipher[13];
  cipher[13]=temp;
  temp=cipher[2];
  cipher[2]=cipher[10];
  cipher[10]=temp;
  temp=cipher[6];
  cipher[6]=cipher[14];
  cipher[14]=temp;
  temp=cipher[15];
  cipher[15]=cipher[11];
  cipher[11]=cipher[7];
  cipher[7]=cipher[3];
  cipher[3]=temp;  
}
 
/*
* Multiplies each column, as an element of GF(2^8), of the state by the polynomial 
* {03}x^3+{01}x^2+{01}x+{02} modulo x^4+1
*/
static void MixColumns(unsigned char cipher[]) {
  int i,j;
  unsigned char a[4], b[4];
  for (i=4; i--;) {
    memcpy(a,&cipher[4*i], 4);  //Copies the ith column of cipher[] to a[]
    b[0]=((a[0]<<1)^(0x1B & (unsigned char)((signed char) a[0] >> 7))); 
    b[1]=((a[1]<<1)^(0x1B & (unsigned char)((signed char) a[1] >> 7))); 
    b[2]=((a[2]<<1)^(0x1B & (unsigned char)((signed char) a[2] >> 7))); 
    b[3]=((a[3]<<1)^(0x1B & (unsigned char)((signed char) a[3] >> 7))); 
    cipher[4*i] = b[0] ^ b[1] ^ a[1] ^ a[2] ^ a[3];
    cipher[4*i+1] = a[0] ^ b[1] ^ b[2] ^ a[2] ^ a[3];
    cipher[4*i+2] = a[0] ^ a[1] ^ b[2] ^ b[3] ^ a[3];
    cipher[4*i+3] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
  } 
}

/*
* Merges the responsibilities of SubBytes, ShiftRows, and MixColumns
*/
static void roundCalc(unsigned char cipher[]) {
  unsigned char a[4], b[4], temp;
  
  cipher[0]=sbox[cipher[0]];
  cipher[1]=sbox[cipher[1]];
  cipher[2]=sbox[cipher[2]];
  cipher[3]=sbox[cipher[3]];
  cipher[4]=sbox[cipher[4]];
  cipher[5]=sbox[cipher[5]];
  cipher[6]=sbox[cipher[6]];
  cipher[7]=sbox[cipher[7]];
  cipher[8]=sbox[cipher[8]];
  cipher[9]=sbox[cipher[9]];
  cipher[10]=sbox[cipher[10]];
  cipher[11]=sbox[cipher[11]];
  cipher[12]=sbox[cipher[12]];
  cipher[13]=sbox[cipher[13]];
  cipher[14]=sbox[cipher[14]];
  cipher[15]=sbox[cipher[15]];
  
  temp = cipher[1];
  cipher[1]=cipher[5];
  cipher[5]=cipher[9];
  cipher[9]=cipher[13];
  cipher[13]=temp;
  temp=cipher[2];
  cipher[2]=cipher[10];
  cipher[10]=temp;
  temp=cipher[6];
  cipher[6]=cipher[14];
  cipher[14]=temp;
  temp=cipher[15];
  cipher[15]=cipher[11];
  cipher[11]=cipher[7];
  cipher[7]=cipher[3];
  cipher[3]=temp;
  
  memcpy(a,&cipher[0], 4);  //Copies the ith column of cipher[] to a[]
  b[0]=((a[0]<<1)^(0x1B & (unsigned char)((signed char) a[0] >> 7))); 
  b[1]=((a[1]<<1)^(0x1B & (unsigned char)((signed char) a[1] >> 7))); 
  b[2]=((a[2]<<1)^(0x1B & (unsigned char)((signed char) a[2] >> 7))); 
  b[3]=((a[3]<<1)^(0x1B & (unsigned char)((signed char) a[3] >> 7))); 
  cipher[0] = b[0] ^ b[1] ^ a[1] ^ a[2] ^ a[3];
  cipher[1] = a[0] ^ b[1] ^ b[2] ^ a[2] ^ a[3];
  cipher[2] = a[0] ^ a[1] ^ b[2] ^ b[3] ^ a[3];
  cipher[3] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
  
  memcpy(a,&cipher[4], 4);  //Copies the ith column of cipher[] to a[]
  b[0]=((a[0]<<1)^(0x1B & (unsigned char)((signed char) a[0] >> 7))); 
  b[1]=((a[1]<<1)^(0x1B & (unsigned char)((signed char) a[1] >> 7))); 
  b[2]=((a[2]<<1)^(0x1B & (unsigned char)((signed char) a[2] >> 7))); 
  b[3]=((a[3]<<1)^(0x1B & (unsigned char)((signed char) a[3] >> 7))); 
  cipher[4] = b[0] ^ b[1] ^ a[1] ^ a[2] ^ a[3];
  cipher[5] = a[0] ^ b[1] ^ b[2] ^ a[2] ^ a[3];
  cipher[6] = a[0] ^ a[1] ^ b[2] ^ b[3] ^ a[3];
  cipher[7] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
  
  memcpy(a,&cipher[8], 4);  //Copies the ith column of cipher[] to a[]
  b[0]=((a[0]<<1)^(0x1B & (unsigned char)((signed char) a[0] >> 7))); 
  b[1]=((a[1]<<1)^(0x1B & (unsigned char)((signed char) a[1] >> 7))); 
  b[2]=((a[2]<<1)^(0x1B & (unsigned char)((signed char) a[2] >> 7))); 
  b[3]=((a[3]<<1)^(0x1B & (unsigned char)((signed char) a[3] >> 7))); 
  cipher[8] = b[0] ^ b[1] ^ a[1] ^ a[2] ^ a[3];
  cipher[9] = a[0] ^ b[1] ^ b[2] ^ a[2] ^ a[3];
  cipher[10] = a[0] ^ a[1] ^ b[2] ^ b[3] ^ a[3];
  cipher[11] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
  
  memcpy(a,&cipher[12], 4);  //Copies the ith column of cipher[] to a[]
  b[0]=((a[0]<<1)^(0x1B & (unsigned char)((signed char) a[0] >> 7))); 
  b[1]=((a[1]<<1)^(0x1B & (unsigned char)((signed char) a[1] >> 7))); 
  b[2]=((a[2]<<1)^(0x1B & (unsigned char)((signed char) a[2] >> 7))); 
  b[3]=((a[3]<<1)^(0x1B & (unsigned char)((signed char) a[3] >> 7))); 
  cipher[12] = b[0] ^ b[1] ^ a[1] ^ a[2] ^ a[3];
  cipher[13] = a[0] ^ b[1] ^ b[2] ^ a[2] ^ a[3];
  cipher[14] = a[0] ^ a[1] ^ b[2] ^ b[3] ^ a[3];
  cipher[15] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
   
}
 
void AES_encrypt(unsigned char plain[], unsigned char cipher[], aeskey_t key) {
  unsigned short k;
  unsigned int l;
  unsigned char a[4], b[4], temp;
  unsigned char Nr=key->Nr;
  unsigned char w[key->wLen];
  
  KeyExpansion(key, w);
  memcpy(cipher, plain, SIZE_16CHAR);
  
  cipher[0] ^=w[0];
  cipher[1] ^=w[1];
  cipher[2] ^=w[2];
  cipher[3] ^=w[3];
  cipher[4] ^=w[4];
  cipher[5] ^=w[5];
  cipher[6] ^=w[6];
  cipher[7] ^=w[7];
  cipher[8] ^=w[8];
  cipher[9] ^=w[9];
  cipher[10] ^=w[10];
  cipher[11] ^=w[11];
  cipher[12] ^=w[12];
  cipher[13] ^=w[13];
  cipher[14] ^=w[14];
  cipher[15] ^=w[15];
  for (k=1; k<Nr; ++k) {
    l=16*k;
  
    //Beginning Roud Calculation
  
    cipher[0]=sbox[cipher[0]];
    cipher[4]=sbox[cipher[4]];
    cipher[8]=sbox[cipher[8]];
    cipher[12]=sbox[cipher[12]];
    temp = sbox[cipher[1]];
    cipher[1]=sbox[cipher[5]];
    cipher[5]=sbox[cipher[9]];
    cipher[9]=sbox[cipher[13]];
    cipher[13]=temp;
    temp=sbox[cipher[2]];
    cipher[2]=sbox[cipher[10]];
    cipher[10]=temp;
    temp=sbox[cipher[6]];
    cipher[6]=sbox[cipher[14]];
    cipher[14]=temp;
    temp=sbox[cipher[15]];
    cipher[15]=sbox[cipher[11]];
    cipher[11]=sbox[cipher[7]];
    cipher[7]=sbox[cipher[3]];
    cipher[3]=temp;
    
    /*cipher[0]=sbox[cipher[0]];
    cipher[1]=sbox[cipher[1]];
    cipher[2]=sbox[cipher[2]];
    cipher[3]=sbox[cipher[3]];
    cipher[4]=sbox[cipher[4]];
    cipher[5]=sbox[cipher[5]];
    cipher[6]=sbox[cipher[6]];
    cipher[7]=sbox[cipher[7]];
    cipher[8]=sbox[cipher[8]];
    cipher[9]=sbox[cipher[9]];
    cipher[10]=sbox[cipher[10]];
    cipher[11]=sbox[cipher[11]];
    cipher[12]=sbox[cipher[12]];
    cipher[13]=sbox[cipher[13]];
    cipher[14]=sbox[cipher[14]];
    cipher[15]=sbox[cipher[15]];
    
    temp = cipher[1];
    cipher[1]=cipher[5];
    cipher[5]=cipher[9];
    cipher[9]=cipher[13];
    cipher[13]=temp;
    temp=cipher[2];
    cipher[2]=cipher[10];
    cipher[10]=temp;
    temp=cipher[6];
    cipher[6]=cipher[14];
    cipher[14]=temp;
    temp=cipher[15];
    cipher[15]=cipher[11];
    cipher[11]=cipher[7];
    cipher[7]=cipher[3];
    cipher[3]=temp;*/
    
    memcpy(a,&cipher[0], 4);  //Copies the ith column of cipher[] to a[]
    b[0]=galoisInvBox[a[0]];
    b[1]=galoisInvBox[a[1]];
    b[2]=galoisInvBox[a[2]];
    b[3]=galoisInvBox[a[3]];
    cipher[0] = b[0] ^ b[1] ^ a[1] ^ a[2] ^ a[3];
    cipher[1] = a[0] ^ b[1] ^ b[2] ^ a[2] ^ a[3];
    cipher[2] = a[0] ^ a[1] ^ b[2] ^ b[3] ^ a[3];
    cipher[3] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
    
    memcpy(a,&cipher[4], 4);  //Copies the ith column of cipher[] to a[]
    b[0]=galoisInvBox[a[0]];
    b[1]=galoisInvBox[a[1]];
    b[2]=galoisInvBox[a[2]];
    b[3]=galoisInvBox[a[3]];
    cipher[4] = b[0] ^ b[1] ^ a[1] ^ a[2] ^ a[3];
    cipher[5] = a[0] ^ b[1] ^ b[2] ^ a[2] ^ a[3];
    cipher[6] = a[0] ^ a[1] ^ b[2] ^ b[3] ^ a[3];
    cipher[7] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
    
    memcpy(a,&cipher[8], 4);  //Copies the ith column of cipher[] to a[]
    b[0]=galoisInvBox[a[0]];
    b[1]=galoisInvBox[a[1]];
    b[2]=galoisInvBox[a[2]];
    b[3]=galoisInvBox[a[3]];
    cipher[8] = b[0] ^ b[1] ^ a[1] ^ a[2] ^ a[3];
    cipher[9] = a[0] ^ b[1] ^ b[2] ^ a[2] ^ a[3];
    cipher[10] = a[0] ^ a[1] ^ b[2] ^ b[3] ^ a[3];
    cipher[11] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
    
    memcpy(a,&cipher[12], 4);  //Copies the ith column of cipher[] to a[]
    b[0]=galoisInvBox[a[0]];
    b[1]=galoisInvBox[a[1]];
    b[2]=galoisInvBox[a[2]];
    b[3]=galoisInvBox[a[3]];
    cipher[12] = b[0] ^ b[1] ^ a[1] ^ a[2] ^ a[3];
    cipher[13] = a[0] ^ b[1] ^ b[2] ^ a[2] ^ a[3];
    cipher[14] = a[0] ^ a[1] ^ b[2] ^ b[3] ^ a[3];
    cipher[15] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
    
    //End Round Calculation
    cipher[0] ^=w[l];
    cipher[1] ^=w[l+1];
    cipher[2] ^=w[l+2];
    cipher[3] ^=w[l+3];
    cipher[4] ^=w[l+4];
    cipher[5] ^=w[l+5];
    cipher[6] ^=w[l+6];
    cipher[7] ^=w[l+7];
    cipher[8] ^=w[l+8];
    cipher[9] ^=w[l+9];
    cipher[10] ^=w[l+10];
    cipher[11] ^=w[l+11];
    cipher[12] ^=w[l+12];
    cipher[13] ^=w[l+13];
    cipher[14] ^=w[l+14];
    cipher[15] ^=w[l+15];
  }
  
  //Beginning Final Round Calculation
  /*cipher[0]=sbox[cipher[0]];
  cipher[1]=sbox[cipher[1]];
  cipher[2]=sbox[cipher[2]];
  cipher[3]=sbox[cipher[3]];
  cipher[4]=sbox[cipher[4]];
  cipher[5]=sbox[cipher[5]];
  cipher[6]=sbox[cipher[6]];
  cipher[7]=sbox[cipher[7]];
  cipher[8]=sbox[cipher[8]];
  cipher[9]=sbox[cipher[9]];
  cipher[10]=sbox[cipher[10]];
  cipher[11]=sbox[cipher[11]];
  cipher[12]=sbox[cipher[12]];
  cipher[13]=sbox[cipher[13]];
  cipher[14]=sbox[cipher[14]];
  cipher[15]=sbox[cipher[15]];
  
  temp = cipher[1];
  cipher[1]=cipher[5];
  cipher[5]=cipher[9];
  cipher[9]=cipher[13];
  cipher[13]=temp;
  temp=cipher[2];
  cipher[2]=cipher[10];
  cipher[10]=temp;
  temp=cipher[6];
  cipher[6]=cipher[14];
  cipher[14]=temp;
  temp=cipher[15];
  cipher[15]=cipher[11];
  cipher[11]=cipher[7];
  cipher[7]=cipher[3];
  cipher[3]=temp;*/
  
  cipher[0]=sbox[cipher[0]];
  cipher[4]=sbox[cipher[4]];
  cipher[8]=sbox[cipher[8]];
  cipher[12]=sbox[cipher[12]];
  temp = sbox[cipher[1]];
  cipher[1]=sbox[cipher[5]];
  cipher[5]=sbox[cipher[9]];
  cipher[9]=sbox[cipher[13]];
  cipher[13]=temp;
  temp=sbox[cipher[2]];
  cipher[2]=sbox[cipher[10]];
  cipher[10]=temp;
  temp=sbox[cipher[6]];
  cipher[6]=sbox[cipher[14]];
  cipher[14]=temp;
  temp=sbox[cipher[15]];
  cipher[15]=sbox[cipher[11]];
  cipher[11]=sbox[cipher[7]];
  cipher[7]=sbox[cipher[3]];
  cipher[3]=temp;
    
  //End Final Round Calculation
    
  l=16*Nr;
  cipher[0] ^=w[l];
  cipher[1] ^=w[l+1];
  cipher[2] ^=w[l+2];
  cipher[3] ^=w[l+3];
  cipher[4] ^=w[l+4];
  cipher[5] ^=w[l+5];
  cipher[6] ^=w[l+6];
  cipher[7] ^=w[l+7];
  cipher[8] ^=w[l+8];
  cipher[9] ^=w[l+9];
  cipher[10] ^=w[l+10];
  cipher[11] ^=w[l+11];
  cipher[12] ^=w[l+12];
  cipher[13] ^=w[l+13];
  cipher[14] ^=w[l+14];
  cipher[15] ^=w[l+15];
}
 
static void InvShiftRows(unsigned char plain[]) {
  unsigned char tmp;
  plain[0]=Invsbox[plain[0]];
  plain[4]=Invsbox[plain[4]];
  plain[8]=Invsbox[plain[8]];
  plain[12]=Invsbox[plain[12]];
  /* row 1 */
  tmp=plain[13];
  plain[13]=Invsbox[plain[9]];
  plain[9]=Invsbox[plain[5]];
  plain[5]=Invsbox[plain[1]];
  plain[1]=Invsbox[tmp];
   /* row 2 */
  tmp = plain[2];
  plain[2]=Invsbox[plain[10]];
  plain[10]=Invsbox[tmp];
  tmp = plain[6];
  plain[6]=Invsbox[plain[14]];
  plain[14]=Invsbox[tmp];
  /* row 3 */
  tmp=plain[3];
  plain[3]=Invsbox[plain[7]];
  plain[7]=Invsbox[plain[11]];
  plain[11]=Invsbox[plain[15]];
  plain[15]=Invsbox[tmp];
}
 
static unsigned char f256times2(unsigned char a) {
  return ((a<<1) ^ (0x1B & (unsigned char)((signed char) a >> 7)));
}
 
static unsigned char f256mul(unsigned char b, unsigned char a) {
  unsigned char temp;
  switch(b) {
  case 0x09:
    temp=((a<<1) ^ (0x1B & (unsigned char)((signed char) a >> 7)));
    temp=((temp<<1) ^ (0x1B & (unsigned char)((signed char) temp >> 7)));
    temp=((temp<<1) ^ (0x1B & (unsigned char)((signed char) temp >> 7))) ^ a;
    return temp;
    //return f256times2(f256times2(f256times2(a))) ^ a;
  case 0x0b:
    temp=((a<<1) ^ (0x1B & (unsigned char)((signed char) a >> 7)));
    temp=a ^ ((temp<<1) ^ (0x1B & (unsigned char)((signed char) temp >> 7)));
    temp=((temp<<1) ^ (0x1B & (unsigned char)((signed char) temp >> 7))) ^ a;
    return temp;
    //return f256times2(a^f256times2(f256times2(a))) ^a;
  case 0x0d:
    temp=a^((a<<1) ^ (0x1B & (unsigned char)((signed char) a >> 7)));
    temp=((temp<<1) ^ (0x1B & (unsigned char)((signed char) temp >> 7)));
    temp=((temp<<1) ^ (0x1B & (unsigned char)((signed char) temp >> 7))) ^ a;
    return temp;
    //return f256times2(f256times2(a^f256times2(a))) ^a;
  case 0x0e:
    temp=a ^ ((a<<1) ^ (0x1B & (unsigned char)((signed char) a >> 7)));
    temp=((temp<<1) ^ (0x1B & (unsigned char)((signed char) temp >> 7))) ^ a;
    temp=((temp<<1) ^ (0x1B & (unsigned char)((signed char) temp >> 7)));
    return temp;
    //return f256times2(a^f256times2(a^f256times2(a)));
  }
  return '\0';
}
 
static void InvRoundCalc(unsigned char plain[]) {
  unsigned char a[4], tmp;    
  memcpy(a, plain, SIZE_4CHAR);
  plain[0]   = f256mul(0x0e,a[0])^f256mul(0x0b,a[1])^f256mul(0x0d,a[2])^f256mul(0x09,a[3]);
  plain[1] = f256mul(0x09,a[0])^f256mul(0x0e,a[1])^f256mul(0x0b,a[2])^f256mul(0x0d,a[3]); 
  plain[2] = f256mul(0x0d,a[0])^f256mul(0x09,a[1])^f256mul(0x0e,a[2])^f256mul(0x0b,a[3]);
  plain[3] = f256mul(0x0b,a[0])^f256mul(0x0d,a[1])^f256mul(0x09,a[2])^f256mul(0x0e,a[3]);
  
  memcpy(a, plain+4, SIZE_4CHAR);
  plain[4]   = f256mul(0x0e,a[0])^f256mul(0x0b,a[1])^f256mul(0x0d,a[2])^f256mul(0x09,a[3]);
  plain[4+1] = f256mul(0x09,a[0])^f256mul(0x0e,a[1])^f256mul(0x0b,a[2])^f256mul(0x0d,a[3]); 
  plain[4+2] = f256mul(0x0d,a[0])^f256mul(0x09,a[1])^f256mul(0x0e,a[2])^f256mul(0x0b,a[3]);
  plain[4+3] = f256mul(0x0b,a[0])^f256mul(0x0d,a[1])^f256mul(0x09,a[2])^f256mul(0x0e,a[3]);
  
  memcpy(a, plain+8, SIZE_4CHAR);
  plain[8]   = f256mul(0x0e,a[0])^f256mul(0x0b,a[1])^f256mul(0x0d,a[2])^f256mul(0x09,a[3]);
  plain[9] = f256mul(0x09,a[0])^f256mul(0x0e,a[1])^f256mul(0x0b,a[2])^f256mul(0x0d,a[3]); 
  plain[10] = f256mul(0x0d,a[0])^f256mul(0x09,a[1])^f256mul(0x0e,a[2])^f256mul(0x0b,a[3]);
  plain[11] = f256mul(0x0b,a[0])^f256mul(0x0d,a[1])^f256mul(0x09,a[2])^f256mul(0x0e,a[3]);
  
  memcpy(a, plain+12, SIZE_4CHAR);
  plain[12]   = f256mul(0x0e,a[0])^f256mul(0x0b,a[1])^f256mul(0x0d,a[2])^f256mul(0x09,a[3]);
  plain[13] = f256mul(0x09,a[0])^f256mul(0x0e,a[1])^f256mul(0x0b,a[2])^f256mul(0x0d,a[3]); 
  plain[14] = f256mul(0x0d,a[0])^f256mul(0x09,a[1])^f256mul(0x0e,a[2])^f256mul(0x0b,a[3]);
  plain[15] = f256mul(0x0b,a[0])^f256mul(0x0d,a[1])^f256mul(0x09,a[2])^f256mul(0x0e,a[3]);
  
  plain[0]=Invsbox[plain[0]];
  plain[4]=Invsbox[plain[4]];
  plain[8]=Invsbox[plain[8]];
  plain[12]=Invsbox[plain[12]];
  /* row 1 */
  tmp=plain[13];
  plain[13]=Invsbox[plain[9]];
  plain[9]=Invsbox[plain[5]];
  plain[5]=Invsbox[plain[1]];
  plain[1]=Invsbox[tmp];
   /* row 2 */
  tmp = plain[2];
  plain[2]=Invsbox[plain[10]];
  plain[10]=Invsbox[tmp];
  tmp = plain[6];
  plain[6]=Invsbox[plain[14]];
  plain[14]=Invsbox[tmp];
  /* row 3 */
  tmp=plain[3];
  plain[3]=Invsbox[plain[7]];
  plain[7]=Invsbox[plain[11]];
  plain[11]=Invsbox[plain[15]];
  plain[15]=Invsbox[tmp];
}
 
void AES_decrypt(unsigned char cipher[], unsigned char plain[], aeskey_t key) {
  unsigned short i,j;
  unsigned int l;
  unsigned char Nr = key->Nr;
  unsigned char *w;
  unsigned char a[4], tmp;
  w=calloc(key->wLen, SIZE_CHAR);
  KeyExpansion(key, w);
  memcpy(plain, cipher, SIZE_16CHAR);
  l=16*Nr;
  plain[0] ^=w[l];
  plain[1] ^=w[l+1];
  plain[2] ^=w[l+2];
  plain[3] ^=w[l+3];
  plain[4] ^=w[l+4];
  plain[5] ^=w[l+5];
  plain[6] ^=w[l+6];
  plain[7] ^=w[l+7];
  plain[8] ^=w[l+8];
  plain[9] ^=w[l+9];
  plain[10] ^=w[l+10];
  plain[11] ^=w[l+11];
  plain[12] ^=w[l+12];
  plain[13] ^=w[l+13];
  plain[14] ^=w[l+14];
  plain[15] ^=w[l+15];
  
  plain[0]=Invsbox[plain[0]];
  plain[4]=Invsbox[plain[4]];
  plain[8]=Invsbox[plain[8]];
  plain[12]=Invsbox[plain[12]];
  /* row 1 */
  tmp=plain[13];
  plain[13]=Invsbox[plain[9]];
  plain[9]=Invsbox[plain[5]];
  plain[5]=Invsbox[plain[1]];
  plain[1]=Invsbox[tmp];
   /* row 2 */
  tmp = plain[2];
  plain[2]=Invsbox[plain[10]];
  plain[10]=Invsbox[tmp];
  tmp = plain[6];
  plain[6]=Invsbox[plain[14]];
  plain[14]=Invsbox[tmp];
  /* row 3 */
  tmp=plain[3];
  plain[3]=Invsbox[plain[7]];
  plain[7]=Invsbox[plain[11]];
  plain[11]=Invsbox[plain[15]];
  plain[15]=Invsbox[tmp];
  
  for(i=Nr-1;i>0;--i)  {
    l=16*i;
    plain[0] ^=w[l];
    plain[1] ^=w[l+1];
    plain[2] ^=w[l+2];
    plain[3] ^=w[l+3];
    plain[4] ^=w[l+4];
    plain[5] ^=w[l+5];
    plain[6] ^=w[l+6];
    plain[7] ^=w[l+7];
    plain[8] ^=w[l+8];
    plain[9] ^=w[l+9];
    plain[10] ^=w[l+10];
    plain[11] ^=w[l+11];
    plain[12] ^=w[l+12];
    plain[13] ^=w[l+13];
    plain[14] ^=w[l+14];
    plain[15] ^=w[l+15];
    
    memcpy(a, plain, SIZE_4CHAR);
    plain[0] = f256mul(0x0e,a[0])^f256mul(0x0b,a[1])^f256mul(0x0d,a[2])^f256mul(0x09,a[3]);
    plain[1] = f256mul(0x09,a[0])^f256mul(0x0e,a[1])^f256mul(0x0b,a[2])^f256mul(0x0d,a[3]); 
    plain[2] = f256mul(0x0d,a[0])^f256mul(0x09,a[1])^f256mul(0x0e,a[2])^f256mul(0x0b,a[3]);
    plain[3] = f256mul(0x0b,a[0])^f256mul(0x0d,a[1])^f256mul(0x09,a[2])^f256mul(0x0e,a[3]);
    
    memcpy(a, plain+4, SIZE_4CHAR);
    plain[4] = f256mul(0x0e,a[0])^f256mul(0x0b,a[1])^f256mul(0x0d,a[2])^f256mul(0x09,a[3]);
    plain[5] = f256mul(0x09,a[0])^f256mul(0x0e,a[1])^f256mul(0x0b,a[2])^f256mul(0x0d,a[3]); 
    plain[6] = f256mul(0x0d,a[0])^f256mul(0x09,a[1])^f256mul(0x0e,a[2])^f256mul(0x0b,a[3]);
    plain[7] = f256mul(0x0b,a[0])^f256mul(0x0d,a[1])^f256mul(0x09,a[2])^f256mul(0x0e,a[3]);
    
    memcpy(a, plain+8, SIZE_4CHAR);
    plain[8]  = f256mul(0x0e,a[0])^f256mul(0x0b,a[1])^f256mul(0x0d,a[2])^f256mul(0x09,a[3]);
    plain[9]  = f256mul(0x09,a[0])^f256mul(0x0e,a[1])^f256mul(0x0b,a[2])^f256mul(0x0d,a[3]); 
    plain[10] = f256mul(0x0d,a[0])^f256mul(0x09,a[1])^f256mul(0x0e,a[2])^f256mul(0x0b,a[3]);
    plain[11] = f256mul(0x0b,a[0])^f256mul(0x0d,a[1])^f256mul(0x09,a[2])^f256mul(0x0e,a[3]);
    
    memcpy(a, plain+12, SIZE_4CHAR);
    plain[12] = f256mul(0x0e,a[0])^f256mul(0x0b,a[1])^f256mul(0x0d,a[2])^f256mul(0x09,a[3]);
    plain[13] = f256mul(0x09,a[0])^f256mul(0x0e,a[1])^f256mul(0x0b,a[2])^f256mul(0x0d,a[3]); 
    plain[14] = f256mul(0x0d,a[0])^f256mul(0x09,a[1])^f256mul(0x0e,a[2])^f256mul(0x0b,a[3]);
    plain[15] = f256mul(0x0b,a[0])^f256mul(0x0d,a[1])^f256mul(0x09,a[2])^f256mul(0x0e,a[3]);
    
    plain[0]=Invsbox[plain[0]];
    plain[4]=Invsbox[plain[4]];
    plain[8]=Invsbox[plain[8]];
    plain[12]=Invsbox[plain[12]];
    /* row 1 */
    tmp=plain[13];
    plain[13]=Invsbox[plain[9]];
    plain[9]=Invsbox[plain[5]];
    plain[5]=Invsbox[plain[1]];
    plain[1]=Invsbox[tmp];
     /* row 2 */
    tmp = plain[2];
    plain[2]=Invsbox[plain[10]];
    plain[10]=Invsbox[tmp];
    tmp = plain[6];
    plain[6]=Invsbox[plain[14]];
    plain[14]=Invsbox[tmp];
    /* row 3 */
    tmp=plain[3];
    plain[3]=Invsbox[plain[7]];
    plain[7]=Invsbox[plain[11]];
    plain[11]=Invsbox[plain[15]];
    plain[15]=Invsbox[tmp];
  }
  plain[0] ^=w[0];
  plain[1] ^=w[1];
  plain[2] ^=w[2];
  plain[3] ^=w[3];
  plain[4] ^=w[4];
  plain[5] ^=w[5];
  plain[6] ^=w[6];
  plain[7] ^=w[7];
  plain[8] ^=w[8];
  plain[9] ^=w[9];
  plain[10] ^=w[10];
  plain[11] ^=w[11];
  plain[12] ^=w[12];
  plain[13] ^=w[13];
  plain[14] ^=w[14];
  plain[15] ^=w[15];
  return;
}
