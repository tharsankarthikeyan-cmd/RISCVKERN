#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

bool strcmp(uint8_t* str1, uint8_t* str2){
  uint64_t counter = 0;
  while(str1[counter] != '\0' && str2[counter] != '\0'){
    if(str1[counter] != str2[counter]){
      return false;
    }
    counter++;
  }
  return true;
}
