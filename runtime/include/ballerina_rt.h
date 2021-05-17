#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

static const uintptr_t BASE_TYPE_INDEX = 0;

static const uintptr_t ARRAY_MEMBER_TYPE_INDEX = 1;

static const uintptr_t ARRAY_SIZE_LSB_INDEX = 2;

static const uintptr_t ARRAY_SIZE_MSB_INDEX = 3;

struct BString;

struct BalMapAnyData;

struct BalMapInt;

struct DynamicBalArray;

template<typename T = void>
struct Vec;

struct SmtPtr {
  int64_t str_table_offset;
  void *val;
};

extern "C" {

BString *new_string(const uint8_t *c_string, uintptr_t size);

void print_string(BString *opaque_ptr);

void deinit_string(BString *ptr);

bool is_same_type(const char *src_type, const char *dest_type);

void print_integer(int64_t num64);

void printf64(double num64);

void print_boolean(int8_t num8);

DynamicBalArray *array_init_int(int64_t size);

Vec<double> *array_init_float(int64_t size);

Vec<bool> *array_init_bool(int64_t size);

Vec<BString*> *array_init_string(int64_t size);

Vec<void*> *array_init_anydata(int32_t size);

void array_store_int(DynamicBalArray *arr_ptr, int64_t index, int64_t ref_ptr);

int64_t array_load_int(DynamicBalArray *arr_ptr, int64_t index);

void array_store_float(Vec<double> *arr_ptr, int64_t index, double ref_ptr);

void *array_load_anydata(Vec<void*> *arr_ptr, int32_t index);

void array_store_anydata(Vec<void*> *arr_ptr, int32_t index, void *ref_ptr);

double array_load_float(Vec<double> *arr_ptr, int64_t index);

void array_store_bool(Vec<bool> *arr_ptr, int64_t index, bool ref_ptr);

bool array_load_bool(Vec<bool> *arr_ptr, int64_t index);

void array_store_string(Vec<BString*> *arr_ptr, int64_t index, BString *ref_ptr);

BString *array_load_string(Vec<BString*> *arr_ptr, int64_t index);

void array_deinit_int(Vec<int64_t> *ptr);

void array_deinit_float(Vec<double> *ptr);

void array_deinit_bool(Vec<bool> *ptr);

void array_deinit_string(Vec<BString*> *ptr);

BalMapInt *map_new_int();

void map_deint_int(BalMapInt *ptr);

void map_store_int(BalMapInt *ptr, BString *key, int64_t member);

bool map_load_int(BalMapInt *ptr, BString *key, int64_t *output_val);

bool map_load_anydata(BalMapAnyData *ptr, BString *key, SmtPtr *output_val);

void map_spread_field_init(BalMapInt *ptr_source, BalMapInt *ptr_expr);

BalMapAnyData *map_new_anydata();

void map_store_anydata(BalMapAnyData *ptr, BString *key, SmtPtr *member_ptr);

int64_t *box_bal_int(int64_t val);

void unbox_bal_int(int64_t *ptr);

double *box_bal_float(double val);

void unbox_bal_double(double *ptr);

int8_t *box_bal_bool(int8_t val);

void unbox_bal_bool(double *ptr);

} // extern "C"
