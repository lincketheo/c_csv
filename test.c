#include "csv.h"

// cunit includes
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t rand_u64()
{
  return (uint64_t)rand() << 32 | rand();
}

static uint64_t rand_f64_0_1()
{
  return (double)rand() / RAND_MAX;
}

static uint64_t rand_u64_range(uint64_t low, uint64_t high)
{
  assert(low < high);
  return low + (rand_u64() % (high - low));
}

static int64_t rand_i64_range(int64_t low, int64_t high)
{
  assert(low < high);
  return low + ((int64_t)rand_u64() % (high - low));
}

static double rand_f64(double low, double high)
{
  assert(low < high);
  return low + rand_f64_0_1() * (high - low);
}

static void init_rand_str(char* str, size_t len)
{
  const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  size_t nc = sizeof(chars) - 1;
  for (size_t i = 0; i < len; i++)
    str[i] = chars[rand() % nc];
  str[len] = '\0';
}

static int init_rand_str_data(char** data, size_t len)
{
  assert(data);
  for (int i = 0; i < len; ++i) {
    size_t llen = rand_u64_range(4, 20);
    data[i] = malloc(llen * sizeof *data[i]);
    if (!data[i])
      return -1;
    init_rand_str(data[i], llen - 1);
  }
  return 0;
}

static void init_rand_i64_data(int64_t* data, size_t len)
{
  assert(data);
  for (int i = 0; i < len; ++i)
    data[i] = rand_i64_range(-100, 100);
}

static void init_rand_u64_data(uint64_t* data, size_t len)
{
  assert(data);
  for (int i = 0; i < len; ++i)
    data[i] = rand_u64_range(0, 100);
}

static void init_rand_f64_data(double* data, size_t len)
{
  assert(data);
  for (int i = 0; i < len; ++i)
    data[i] = rand_f64(-100, 100);
}

void test_csv()
{
  int ret = 0;

  struct csv_s csv_ctx;
  const char* keys[] = { "col1", "col2", "col3", "col4", "col5" };
  enum csv_dt_e dtypes[] = { DBL, STR, UINT, INT, STR };

  CU_ASSERT(csv_init(&csv_ctx, keys, dtypes, 5));

  size_t nrows = 400;

  // Create random data
  double* col1 = calloc(nrows, sizeof *col1);
  char** col2 = calloc(nrows, sizeof *col2);
  uint64_t* col3 = calloc(nrows, sizeof *col3);
  int64_t* col4 = calloc(nrows, sizeof *col4);
  char** col5 = calloc(nrows, sizeof *col5);
  if (!(col1 && col2 && col3 && col4 && col5)) {
    printf("CSV test failed because it couldn't allocate memory");
    ret = -1;
    goto CLEANUP;
  }

  int status = 0;
  init_rand_f64_data(col1, nrows);
  status += init_rand_str_data(col2, nrows);
  init_rand_u64_data(col3, nrows);
  init_rand_i64_data(col4, nrows);
  status += init_rand_str_data(col5, nrows);

  if (status) {
    printf("CSV test failed because it couldn't allocate memory");
    ret = -1;
    goto CLEANUP;
  }

  // Add random data
  for (int i = 0; i < nrows; ++i) {
    union csv_dt_u* row = calloc(5, sizeof *row);
    if (!row) {
      printf("CSV test failed because it couldn't allocate memory");
      ret = -1;
      goto CLEANUP;
    }
    row[0] = (union csv_dt_u) { .double_data = col1[i] };
    row[1] = (union csv_dt_u) { .str_data = col2[i] };
    row[2] = (union csv_dt_u) { .uint_data = col3[i] };
    row[3] = (union csv_dt_u) { .int_data = col4[i] };
    row[4] = (union csv_dt_u) { .str_data = col5[i] };
    CU_ASSERT(csv_push(&csv_ctx, row));
    free(row);
  }

  // Verify random data
  for (int i = 0; i < nrows; ++i) {
    CU_ASSERT_EQUAL(col1[i], csv_ctx.data[i][0].double_data);
    CU_ASSERT_STRING_EQUAL(col2[i], csv_ctx.data[i][1].str_data);
    CU_ASSERT_EQUAL(col3[i], csv_ctx.data[i][2].uint_data);
    CU_ASSERT_EQUAL(col4[i], csv_ctx.data[i][3].int_data);
    CU_ASSERT_STRING_EQUAL(col5[i], csv_ctx.data[i][4].str_data);
  }

  // Save file TODO - this test can be more comprehensive
  CU_ASSERT(csv_save(&csv_ctx, "test_csv.csv"));
  remove("test_csv.csv");

CLEANUP:
  if (col1)
    free(col1);
  if (col2) {
    for (int i = 0; i < nrows; ++i)
      if (col2[i])
        free(col2[i]);
    free(col2);
  }
  if (col3)
    free(col3);
  if (col4)
    free(col4);
  if (col5) {
    for (int i = 0; i < nrows; ++i)
      if (col5[i])
        free(col5[i]);
    free(col5);
  }

  CU_ASSERT(ret == 0);
  CU_ASSERT(csv_free(&csv_ctx));
}

int main()
{
  CU_initialize_registry();
  CU_pSuite suite = CU_add_suite("CSV Test", 0, 0);
  CU_add_test(suite, "CSV Test main", test_csv);
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return 0;
}
