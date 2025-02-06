#ifndef _CSV_H
#define _CSV_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(__cplusplus)
extern "C"
{
#endif

  /**
   * A CSV column data type can be any
   * of these. Feel free to expand
   */
  union csv_dt_u
  {
    int64_t int_data;
    uint64_t uint_data;
    char *str_data;
    double double_data;
  };

  /**
   * Additional summation algebraic type
   * for use in enums to determine the csv_dt_u
   */
  enum csv_dt_e
  {
    INT,
    UINT,
    STR,
    DBL
  };

  /**
   * A csv product type is the combination of:
   * keys: A list of strings of length ncols
   * col_dt: A list of column types of length ncols
   * data: A 2 dimensional array of size nrows x ncols
   *
   * row_capacity: The number of rows possible. CSV structs
   * are dynamically doubled in size when there needs more room
   */
  struct csv_s
  {
    char **keys;
    enum csv_dt_e *col_dt;
    size_t ncols;

    union csv_dt_u **data;
    size_t nrows;
    size_t row_capacity;
  };

  /**
   * @brief Creates a new csv
   *
   * @param csv_ctx A csv_ctx. Must be non null (check is done)
   * @param keys The keys of length ncols.
   * @param data_type The data type of each column
   * @param cols number of columns
   * @return True if succeeded, false otherwise
   */
  bool csv_init (struct csv_s *csv_ctx, const char **keys,
                 const enum csv_dt_e *data_type, const size_t cols);

  /**
   * @brief Frees internal structures of a csv type
   *
   * @return True if succeeded false otherwise
   */
  bool csv_free (struct csv_s *csv_ctx);

  /**
   * @brief Adds data to a csv
   *
   * @param csv_ctx The csv data
   * @param row The row to add
   * @return True if succeeded false otherwise
   */
  bool csv_push (struct csv_s *csv_ctx, const union csv_dt_u *row);

  /**
   * @brief Saves a csv to a file
   *
   * @param csv_ctx The csv data
   * @param filename The file to save
   * @return True if suceeded false else
   */
  bool csv_save (struct csv_s *csv_ctx, const char *filename);

#if defined(__cplusplus)
}
#endif

#endif
