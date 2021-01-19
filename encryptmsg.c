#include <stdio.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>

/*

  Encrypt a file with provided public and private keys from keypairs.c

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
    Retreive an existing file or make a new file
  */

  printf("Enter file to encrypt\n");
  printf("40 characters maxiumum\n");
  char user_input_filename[40];
  fgets(user_input_filename, 40, stdin);
  int input_length = strlen(user_input_filename);
  if (input_length == 0) {
    printf("Filename Cannot Be An Empty String!\n");
    return 0;
  }
  printf("Input length = \n");
  printf("%d", input_length);
  printf("\n");
  user_input_filename[input_length - 1] = '\0';



  printf("NEW Input length = \n");
  printf("%d", strlen(user_input_filename));
  printf("\n");






  //int user_input_length = user_input[strcspn(user_input, "\r\n")] = 0;
/*
  printf("User_inpt_name = \n");
  printf("%s", user_input_length);
  printf("\n");
  char user_input_filename[40];
  for (int i = 0; i < user_input_length; i++) {
    user_input_filename[i] = user_input[i];
  }
*/
  //strtok(user_input_filename, "\n");
  //user_input_filename[strcspn(user_input_filename, "\r\n")] = 0;
  /*if (fgets(user_input_filename, sizeof user_input_filename, stdin) != NULL) {

  size_t len = strlen(user_input_filename);
  if (len > 0 && user_input_filename[len-1] == '\n') {
    user_input_filename[--len] = '\0';
  }
}
*/

  //FILE *file;





  //fscanf(stdin, "%40c", user_input_filename);



  FILE *message;
  message = fopen(user_input_filename, "r");
  if (message == NULL) {
    printf("ERROR: File Not Found!\n");
    return -1;
  }



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Find the length of the message given by the user to place it in a buffer

    fseek and ftell
    fread to get unsigned bytes
    int fseek(FILE *stream, long int offset, int whence)
    seek to the end of the file and then ask for the position
    http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c

  */

  fseek(message, 0L, SEEK_END);
  int message_length = ftell(message);
  unsigned char
message_buffer[message_length];
  // put cursor to the file back to beginning of file
  rewind(message);
  fread(message_buffer, 1, message_length, message);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Get My Secret Key
  */

  FILE *MySecretKey;
  MySecretKey = fopen("MY_SecretKey.bin", "rb");
  if (MySecretKey == NULL) {
    printf("ERROR: MY_SecretKey.bin Cannnot Be Found!\n");
    return -1;
  }
  unsigned char
my_secretkey[crypto_box_SECRETKEYBYTES];
  fread(my_secretkey, 1, crypto_box_SECRETKEYBYTES, MySecretKey);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Retreive Receiver's Public Encryption Key
  */

  FILE *RecPublicKey;
RecPublicKey = fopen("Recipient_PublicKey.bin", "rb");
  if (RecPublicKey == NULL) {
    printf("ERROR: Receiver's public key cannot be found!\n");
    return -1;
  }
  unsigned char
rec_pk[crypto_sign_PUBLICKEYBYTES];
  fread(rec_pk, 1, crypto_sign_PUBLICKEYBYTES, RecPublicKey);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Generate The Nonce and Encrypt Message Using the Nonce
  */

  unsigned char
nonce[crypto_box_NONCEBYTES];
  unsigned char
ciphertext[crypto_box_MACBYTES + message_length];
  randombytes_buf(nonce,
sizeof nonce);

  // MIGHT HAVE TO PUT CIPHERTEXT IN A NEW BUFFER AND THEN PUT NONCE IN IT
  int verifyEncryption = crypto_box_easy(ciphertext, message_buffer, message_length, nonce, rec_pk, my_secretkey);
  if(verifyEncryption != 0) {
    printf("ERROR: File Was Not Encrypted Properly!\n");
    return -1;
  }

  printf("The size of the ciphertext is %d\n", sizeof(ciphertext));


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Put ciphertext into the a buffer (starting at the first position) that will have
     additional space for the nonce which will be unencrypted
  */

  unsigned long long CIPHERTEXT_NONCE_LENGTH = crypto_box_MACBYTES + message_length + crypto_box_NONCEBYTES;
  unsigned char
ciphertext_nonce[CIPHERTEXT_NONCE_LENGTH];

  for (int k = 0; k < crypto_box_MACBYTES + message_length; k++) {
    ciphertext_nonce[k] = ciphertext[k];
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Append nonce into the cipherext_nonce buffer after the ciphertext
  */

  int j = 0;
  for (int i = crypto_box_MACBYTES + message_length; i < CIPHERTEXT_NONCE_LENGTH; i++) {
    ciphertext_nonce[i] = nonce[j];
    j++;
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Retreive My Secret Signing Key (Sender's Secret Key)
  */

  FILE *MySecretDigitalKey;
  MySecretDigitalKey = fopen("MY_SecretDigitalKey.bin", "rb");
  if (MySecretDigitalKey == NULL) {
    printf("ERROR: Sender's Digital Key Cannot Be Found!\n");
    return -1;
  }
  unsigned char
my_sk[crypto_sign_SECRETKEYBYTES];
  fread(my_sk, 1, crypto_sign_SECRETKEYBYTES, MySecretDigitalKey);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    Digitally Sign ciphertext
  */

  unsigned long long SIGNED_MESSAGE_LENGTH = message_length + crypto_box_MACBYTES + crypto_box_NONCEBYTES + crypto_sign_BYTES;//crypto_sign_BYTES;//crypto_sign_SECRETKEYBYTES;//crypto_sign_BYTES;
  unsigned char
signed_message[SIGNED_MESSAGE_LENGTH];

  //crypto_sign(signed_message,
&signed_message_len, message,
MESSAGE_LEN, MySecretDigitalKey);
  int verifysign = crypto_sign(signed_message, &SIGNED_MESSAGE_LENGTH, ciphertext_nonce, CIPHERTEXT_NONCE_LENGTH, my_sk);

  if (verifysign != -0) {
    printf("ERROR: Digital Signature Was Not Appended!\n");
    return -1;
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    write fully encrypted signed message to a new file
  */



  /*
  printf("Name encrypted file\n");
  printf("40 characters maxiumum\n");
  char encrypted_filename[40];
  fgets(encrypted_filename, 40, stdin);
*/

/*
  printf("HEY\n");
  if (encrypted_filename == NULL) {
    //encrypted_filename = "PrivMessage.txt";
    return -1;
  }
  if (strlen(encrypted_filename) == 0) {
    printf("Filename Cannot Be An Empty String!\n");
    return 0;
  }
  printf("HEY2\n");

  encrypted_filename[strlen(encrypted_filename) - 1] = '\0';
  printf("HEY3\n");
*/
  FILE *fp;
  fp = fopen(/*encrypted_filename*/ "PrivMessage", "w");

//  printf("HEY4\n");
  // ORGINAL
  //printf("%llu", sizeof(signed_message));
//  printf("HEY5\n");
  fwrite(signed_message, 1, SIGNED_MESSAGE_LENGTH, fp);
  // ORGINAL
//  fwrite(signed_message, 1, sizeof(signed_message), fp);
//  printf("HEY6\n");

  fclose(fp);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
   Close all other files used and end program
  */

  fclose(RecPublicKey);
  fclose(MySecretKey);
  fclose(MySecretDigitalKey);
  fclose(message);
  return 0;

}
