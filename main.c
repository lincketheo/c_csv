
#include "csv.h"
#include <stdio.h>
#include <string.h>

int main()
{
  struct csv_s csv_ctx;
  const char* keys[] = { "col1", "col2", "col3", "col4", "col5" };
  enum csv_dt_e dtypes[] = { DBL, STR, UINT, INT, STR };
  if (!csv_init(&csv_ctx, keys, dtypes, 5))
    return 1;

  char* str1 = "Hello";
  char* str2 = "World";
  char* str3 = "C";
  char* str4 = "Is Awesome";

  union csv_dt_u* row = calloc(5, sizeof *row);
  row[0] = (union csv_dt_u){
    .double_data = 90.23
  };
  row[1] = (union csv_dt_u){
    .str_data = str1
  };
  row[2] = (union csv_dt_u){
    .uint_data = 1238907123
  };
  row[3] = (union csv_dt_u){
    .int_data = -1230
  };
  row[4] = (union csv_dt_u){
    .str_data = str2
  };
  if(!csv_push(&csv_ctx, row))
    return 1;

  row[0] = (union csv_dt_u){
    .double_data = 12398.0
  };
  row[1] = (union csv_dt_u){
    .str_data = str3
  };
  row[2] = (union csv_dt_u){
    .uint_data = 0
  };
  row[3] = (union csv_dt_u){
    .int_data = 123098
  };
  row[4] = (union csv_dt_u){
    .str_data = str4
  };
  if(!csv_push(&csv_ctx, row))
    return 1;

  free(row);

  const char* filename = "output.csv";
  csv_save(&csv_ctx, filename);
  printf("Saved output to %s\n", filename);

  if(!csv_free(&csv_ctx))
    return 1;

  return 0;
}
