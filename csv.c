#include "csv.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void csv_error(const char* msg_fmt, ...)
{
  va_list args;
  va_start(args, msg_fmt);
  printf("csv error: ");
  vprintf(msg_fmt, args);
}

static bool initialize_row_range(
    struct csv_s* csv_ctx,
    size_t start_inc,
    size_t end_exc)
{
  for (int i = start_inc; i < end_exc; ++i) {
    csv_ctx->data[i] = malloc(csv_ctx->ncols * sizeof(union csv_dt_u));
    if (!csv_ctx->data[i]) {
      csv_error("Error mallocing data row index: %d\n", i);
      return false;
    }
  }
  return true;
}

static bool guard_csv_ctx_exists(struct csv_s* csv_ctx)
{
  if (!csv_ctx) {
    csv_error("CSV context is null");
    return false;
  }
  return true;
}

static bool guard_csv_ctx_is_init(struct csv_s* csv_ctx)
{
  if (!guard_csv_ctx_exists(csv_ctx))
    return false;

  if (!(csv_ctx->keys != NULL && csv_ctx->col_dt != NULL && csv_ctx->data != NULL && csv_ctx->row_capacity > 0)) {
    csv_error("CSV context was never initialized\n");
    return false;
  }

  return true;
}

bool csv_init(
    struct csv_s* csv_ctx,
    const char** keys,
    const enum csv_dt_e* data_type,
    const size_t cols)
{

  if (!guard_csv_ctx_exists(csv_ctx))
    return false;

  csv_ctx->ncols = cols;

  csv_ctx->keys = malloc(cols * sizeof(char*));
  for (int i = 0; i < cols; ++i) {
    size_t len = strlen(keys[i]) + 1;
    csv_ctx->keys[i] = malloc(len * sizeof(char));
    memcpy(csv_ctx->keys[i], keys[i], len);
  }

  csv_ctx->col_dt = malloc(cols * sizeof(enum csv_dt_e));
  memcpy(csv_ctx->col_dt, data_type, cols * sizeof(enum csv_dt_e));

  csv_ctx->row_capacity = 500;
  csv_ctx->nrows = 0;
  csv_ctx->data = malloc(csv_ctx->row_capacity * sizeof(union csv_dt_u*));

  initialize_row_range(csv_ctx, 0, csv_ctx->row_capacity);

  return true;
}

bool csv_free(struct csv_s* csv_ctx)
{
  if (csv_ctx->keys) {
    for (int i = 0; i < csv_ctx->ncols; ++i)
      free(csv_ctx->keys[i]);
    free(csv_ctx->keys);
  }

  if (csv_ctx->col_dt)
    free(csv_ctx->col_dt);

  if (csv_ctx->data) {
    for (int i = 0; i < csv_ctx->row_capacity; ++i)
      free(csv_ctx->data[i]);
    free(csv_ctx->data);
  }

  return true;
}

static bool double_csv_row_capacity(struct csv_s* csv_ctx)
{
  if (!guard_csv_ctx_is_init(csv_ctx))
    return false;

  size_t old_row_capacity = csv_ctx->row_capacity;
  csv_ctx->row_capacity *= 2;

  union csv_dt_u** realloc_ret = realloc(csv_ctx->data, csv_ctx->row_capacity);
  if (realloc_ret == NULL) {
    csv_error("Error doubling csv context data capacity, realloc returned null\n");
    return false;
  }
  csv_ctx->data = realloc_ret;

  initialize_row_range(csv_ctx, old_row_capacity, csv_ctx->row_capacity);

  return true;
}

bool csv_push(struct csv_s* csv_ctx, const union csv_dt_u* row)
{
  if (!guard_csv_ctx_is_init(csv_ctx))
    return false;
  if (csv_ctx->row_capacity == csv_ctx->nrows) {
    if (!double_csv_row_capacity(csv_ctx))
      return false;
  }

  memcpy(csv_ctx->data[csv_ctx->nrows], row, csv_ctx->ncols * sizeof(union csv_dt_u));
  csv_ctx->nrows++;

  return true;
}

static bool write_data_to_fp(FILE* fp, union csv_dt_u data, enum csv_dt_e data_type)
{
  switch (data_type) {
  case INT: {
    fprintf(fp, "%zu", data.int_data);
    break;
  }
  case DBL: {
    fprintf(fp, "%f", data.double_data);
    break;
  }
  case STR: {
    fprintf(fp, "%s", data.str_data);
    break;
  }
  case UINT: {
    fprintf(fp, "%zu", data.uint_data);
    break;
  }
  default:
    return false;
  }

  return true;
}

bool csv_save(struct csv_s* csv_ctx, const char* filename)
{
  if (!guard_csv_ctx_is_init(csv_ctx))
    return false;

  FILE* fp = fopen(filename, "w");
  if (!fp) {
    csv_error("Failed to open file: %s\n");
    return false;
  }

  // Write keys to file
  for (int i = 0; i < csv_ctx->ncols - 1; ++i) {
    fprintf(fp, "%s,", csv_ctx->keys[i]);
  }
  fprintf(fp, "%s\n", csv_ctx->keys[csv_ctx->ncols - 1]);

  // Write data to file
  for (int i = 0; i < csv_ctx->nrows; ++i) {
    for (int k = 0; k < csv_ctx->ncols - 1; ++k) {
      if (!write_data_to_fp(fp,
              csv_ctx->data[i][k],
              csv_ctx->col_dt[k]))
        return false;
      fprintf(fp, ",");
    }
    if (!write_data_to_fp(fp,
            csv_ctx->data[i][csv_ctx->ncols - 1],
            csv_ctx->col_dt[csv_ctx->ncols - 1]))
      return false;
    fprintf(fp, "\n");
  }

  fclose(fp);

  return true;
}
