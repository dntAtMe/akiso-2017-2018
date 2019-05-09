#include "iofunctions.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

int str_size(char *str);
int print_number_as_array(int out, int number, int base);
int format_array_to_int(char *array, int base);
int format_hex_array(char *array);


int myprintf(const char *input, ...)
{
  int i = 0;
  int n = 0;

  char *out;
  int number;
  char *num_out;

  //int *stack_head = __builtin_frame_address(0);
  //stack_head += 3 ;

  char *pointer = (char *)&input + sizeof(input);

  while(input[i])
  {
   if (input[i] == '%')
   {
    i++;
    switch(input[i])
    {
      case 'd':
       number = *((int *)pointer);
       pointer+= sizeof(int);
       //number = *stack_head++;
       //printf("nunu: %s\n", (char *) number);
       print_number_as_array(1, number, 10);
       break;
      case 'b':
       number = *((int *)pointer);
       pointer+= sizeof(int);
       //number = *stack_head++;
       print_number_as_array(1, number, 2);
       break;

      case 'x':
       number = *((int *)pointer);
       pointer+= sizeof(int);
       //number = *stack_head++;
       print_number_as_array(1, number, 16);
       break;

      case 's':
       out = *((int *)pointer);
       pointer+= sizeof(char *);
       //out = ((char *) *stack_head++);
       //printf("szieof char*: %d\n out: %s\n current: %s\n str_size(): %d\n", sizeof (char), out, current, str_size("out"));
      write(1, out, str_size(out));

       break;
    }
   } else
   {
     write(1, input+i, 1);
   }

   i++;
  }

  return 0;
}

int str_size(char *str)
{
  int i = 0;

  while(str[i])
  {
    if(str[i] == '\0' || str[i] == '\n')
     break;
    i++;
  }

  return i;
}

int print_number_as_array(int out, int number, int base)
{
 char buf[50];
 char *s = buf + (sizeof(buf) / sizeof(buf[0])) - 1;

 *s = '\0';
 do {
   s--;
   *s = '0' + number % base;
   if(*s > '9')
    *s += 7;
 } while (number /= base);


 write(out, s, str_size(s));

 return 1;

}


int myscanf(const char *input, ...)
{
 const int buffer_size = 1024;
 char text[buffer_size];
 char *destination;

 int *number;
 int formatted_number;
 int i = 0;

 if (read(0, text, buffer_size) < 0)
 {
   write(2, "Wrong format.\n", str_size("Wrong format.\n"));
   return -1;
 }

 char *pointer = (char *)&input + sizeof(input);

 while(input[i])
 {
  if (input[i] == '%')
  {
    i++;
    switch(input[i])
    {
      case 's':
       destination = *((int *)pointer);
       pointer += sizeof(char *);
       memcpy(destination, text, str_size(text) + 1);
       break;
      case 'd':
       number = *((int *)pointer);
       pointer += sizeof (int *);
       formatted_number = format_array_to_int(text, 10);

       memcpy(number, &formatted_number, sizeof(int));
       break;
      case 'b':
       number = *((int *)pointer);
       pointer += sizeof (int *);
       formatted_number = format_array_to_int(text, 2);

       memcpy(number, &formatted_number, sizeof(int));

       break;
       case 'x':
        number = *((int *)pointer);
        pointer += sizeof (int *);
        format_hex_array(text);
        formatted_number = format_array_to_int(text, 16);

        memcpy(number, &formatted_number, sizeof(int));

        break;
    }
  }
  i++;
 }

 return 0;
}

int format_hex_array(char *array)
{
  int length = str_size(array);

  for (int i = 0; i < length; i++)
  {
    if (array[i] >= 97 && array[i] <= 102)
     array[i] = (char) (array[i] - ('a' - ':'));
    else if (array[i] >= 65 && array[i] <= 70)
     array[i] = (char) (array[i] - ('A' - ':'));
  }

  return 0;
}

int format_array_to_int(char *array, int base)
{
 int length = str_size(array);
 int number = 0;
 int power = 1;

 for (int i = 0; i < length; i++)
 {
   power = 1;
   for (int j = 0; j < i; j++)
    power *= base;
   number += power * (array[length - 1 - i] - '0');
 }

 return number;
}


int main(int argc, char **argv)
{
  int in;

  char tab[1024];
  myscanf("%s", tab);
  myprintf("%s\n", tab);
  myscanf("%d", &in);
  myprintf("%d, %x, %b\n", in, in, in);
  myscanf("%b", &in);
  myprintf("%d, %x, %b\n", in, in, in);
  myscanf("%x", &in);
  myprintf("%d, %x, %b\n", in, in, in);

}
