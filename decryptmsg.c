#include <string.h>
#include <stdio.h>
#include <sodium.h>

/*
  decryptmessage.c

*/
int main() {


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
  Initialize sodium library
*/

  if (sodium_init()  == -1)
{
    printf("ERROR: sodium library was not initialized!\n");
    return -1;
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Retreive encrypted message
  */

  FILE *fully_encrypted_message;
  char encrypted_filename[40];

  fully_encrypted_message = fopen("PrivMessage", "rb");


  if (fully_encrypted_message == NULL) {
    printf("ERROR: No such file exists\n");
    return -1;
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Find the length of the fully encrypted_message to put it into a buffer
    http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
  */

  long long SIGNED_MESSAGE_LENGTH;
  fseek(fully_encrypted_message, 0L, SEEK_END);
  // ftell returns the current file position of the given stream.
  SIGNED_MESSAGE_LENGTH = ftell(fully_encrypted_message);
  printf("the file's length is %1dB\n", SIGNED_MESSAGE_LENGTH);
//printf("the size of file is %1dB\n", sizeof() )
  // sets the file position to the beginning of the file of the given stream
  rewind(fully_encrypted_message);
  unsigned char
signed_message[SIGNED_MESSAGE_LENGTH];
  //  unsigned  long long
signed_message_len;
  // put encrypted message into buffer
  //fgets(signed_message, length, fully_encrypted_message);
  //size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
  printf("Signed message length is %d\n", SIGNED_MESSAGE_LENGTH);
  printf("Size of signed message is %d \n", sizeof(SIGNED_MESSAGE_LENGTH));

  fread(signed_message, 1, SIGNED_MESSAGE_LENGTH, fully_encrypted_message);
  printf("the EncryptedMessages file's size is %1dB\n", sizeof(signed_message));


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Get senders's public digital key
  */

  FILE *MyPublicDigitalKey;
MyPublicDigitalKey = fopen("MY_PublicDigitalKey.bin", "rb");
  if (MyPublicDigitalKey == NULL) {
    printf("ERROR: My public key does not exist\n!");
    return -1;
  }
  unsigned char
my_pk[crypto_sign_PUBLICKEYBYTES];
  fread(my_pk, 1, crypto_sign_PUBLICKEYBYTES, MyPublicDigitalKey );
//  printf("the PublicDigitalKey file's size is %1dB\n", sizeof(my_pk));


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Verify Digital Signature and decrypt it
  */

//
//  printf("crypto_sign_BYTES = %d\n", sizeof(crypto_sign_BYTES));
  printf("signed_message_length = %d\n", SIGNED_MESSAGE_LENGTH);
  unsigned long long CIPHERTEXT_NONCE_LENGTH = SIGNED_MESSAGE_LENGTH - crypto_sign_SECRETKEYBYTES;//crypto_sign_BYTES;//crypto_sign_SECRETKEYBYTES;//crypto_sign_BYTES;//crypto_sign_SECRETKEYBYTES;
  unsigned char
ciphertext_nonce[CIPHERTEXT_NONCE_LENGTH];

  printf("crypto_sign_SECRETKEYBYTES = %d\n", crypto_sign_SECRETKEYBYTES);
  printf("The size of the private signing key appended is %d\n", crypto_box_SECRETKEYBYTES);
  printf("CIPHERTEXT_NONCE_LENGTH = %d\n", CIPHERTEXT_NONCE_LENGTH);
  printf("\n");


  //char* m;


    /*
    int crypto_sign_open( unsigned
char *m,    // decrypted message buffer
                  unsigned
long long
*mlen,  // len of decrypted message buffer
                  const
unsigned char
*sm,   // encrypted message
                unsigned
long long
smlen,  // encrypted message length
            const  unsigned char
*pk     ); // digital public key
  */

  //function checks that the signed message sm whose length is smlen bytes has a valid signature for the public key pk.
  //If the signature  doesn't appear to be valid, the function returns -1
  int verifySign = crypto_sign_open( ciphertext_nonce,
                                      &CIPHERTEXT_NONCE_LENGTH,
                                      signed_message,
                                      SIGNED_MESSAGE_LENGTH,
                                      my_pk);

  if (verifySign != 0) {
    printf("ERROR: Digital Signatures Do Not Match!\n");
    return -1;
  }

  printf("The signature of the message is legitimente.\n");


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
  Extract nonce that was encrypted by the digital signature key
  The nonce will be used in decrypting the file
*/



  unsigned long long CIPHERTEXT_LENGTH = CIPHERTEXT_NONCE_LENGTH - crypto_box_NONCEBYTES;
  unsigned char
nonce[crypto_box_NONCEBYTES];

  printf("crypto_box_NONCEBYTES = %d\n", crypto_box_NONCEBYTES);
  printf("CIPHERTEXT_NONCE_LENGTH = %d\n", CIPHERTEXT_NONCE_LENGTH);
  printf("CIPHERTEXT_LENGTH = %d\n", CIPHERTEXT_LENGTH);

  // i should start at the end of the message and end when it reaches the end of the encrypted message to extract the nonce char by char
  int j = 0;
  for (int i = CIPHERTEXT_LENGTH; i < CIPHERTEXT_NONCE_LENGTH; i++) {
    nonce[j] = ciphertext_nonce[i];
    j++;
  }

  printf("size of nonce = %d\n", sizeof(nonce));
  printf("ciphertext_nonce size = %d\n", sizeof(ciphertext_nonce));


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
  Put the seperated ciphertext into a new buffer called ciphertext
*/

  unsigned char ciphertext[CIPHERTEXT_LENGTH];

  printf("Size of ciphertext = %d\n", sizeof(ciphertext));

// SOMETHING WRONG HERE
  for (int k = 0; k < CIPHERTEXT_LENGTH; k++) {
    ciphertext[k] = ciphertext_nonce[k];
  }

  printf("Size of ciphertext = %d\n", sizeof(ciphertext));

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Get senders's public key
  */

  FILE *MyPublicKey;
  MyPublicKey = fopen("MY_PublicKey.bin", "rb");
  if (MyPublicKey == NULL) {
    printf("ERROR: The receiver's public key does not exist!\n");
    return -1;
  }
  unsigned char
my_publickey[crypto_box_PUBLICKEYBYTES];
  fread(my_publickey, 1, crypto_box_PUBLICKEYBYTES, MyPublicKey);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Get recipient's secret key
  */

  unsigned char
receiver_secret_key[crypto_box_SECRETKEYBYTES];
  FILE *Receiver_secret_key;
  Receiver_secret_key = fopen("Recipient_SecretKey.bin", "rb");
  if (Receiver_secret_key == NULL) {
    printf("ERROR: Recipient's Secret Key Not Found\n");
    return -1;
  }
  fread(receiver_secret_key, 1, crypto_box_SECRETKEYBYTES, Receiver_secret_key);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Decrypt Message using sender's public key and receiver's private key
     and the nonce extracted from ciphertext_nonce
  */

  unsigned long long DECRYPTED_MESSAGE_LENGTH = CIPHERTEXT_LENGTH - crypto_box_MACBYTES;
  unsigned char
decrypted_message[DECRYPTED_MESSAGE_LENGTH];

  printf("Size of DECRYPTED_MESSAGE_LENGTH = %d\n", DECRYPTED_MESSAGE_LENGTH);


    /*
    int crypto_box_open_easy(
    unsigned char *m,
     const unsigned char *c,
    unsigned long long clen,
     const unsigned char *n,
   const unsigned char *pk,
   const unsigned char *sk);
                       */

   int verifyDecrypted = crypto_box_open_easy( decrypted_message,
                                               ciphertext,
                                               CIPHERTEXT_LENGTH,
                                               nonce,
                                               my_publickey,
                                               receiver_secret_key);

    if (verifyDecrypted != 0) {
    printf("ERROR: Message Was Not Properly Decrypted!");
    return -1;
    }

    printf("Message has been decrypted.\n");


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Write decrypted message into a file
  */

  FILE *destination;
  destination = fopen("decryptedmessage"/*decrypted_filename*/, "w");
  fwrite(decrypted_message, 1, DECRYPTED_MESSAGE_LENGTH, destination);

  printf("Decrypted data has been written to the specified file\n");
  return 0;


} // end main
