/*
 * Copyright (c) 2021, WSO2 Inc. (http://www.wso2.org) All Rights Reserved.
 *
 * WSO2 Inc. licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

// Rust Library

mod type_checker;

use std::ffi::CStr;
use std::io::{self, Write};
use std::mem;
use std::os::raw::c_char;
use std::slice;

mod bal_map;
pub use bal_map::map::BalMapInt;

pub struct BString {
    value: &'static str,
}

// Return a pointer to struct containing heap allocated string
#[no_mangle]
pub extern "C" fn new_string(c_string: *const u8, size: usize) -> *mut BString {
    assert!(!c_string.is_null());
    let slice = unsafe { std::slice::from_raw_parts(c_string, size) };
    let string = std::str::from_utf8(slice);
    let opaque = BString {
        value: string.unwrap(),
    };
    let opaque_ptr = Box::into_raw(Box::new(opaque));
    return opaque_ptr;
}

#[no_mangle]
pub extern "C" fn print_string(opaque_ptr: *mut BString) {
    assert!(!opaque_ptr.is_null());
    print!("{}", unsafe { (*opaque_ptr).value });
    io::stdout().flush().unwrap();
}

#[no_mangle]
pub extern "C" fn deinit_string(ptr: *mut BString) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

// To check whether typecast is possible from source to destination
#[no_mangle]
pub extern "C" fn is_same_type(src_type: *const c_char, dest_type: *const c_char) -> bool {
    assert!(!src_type.is_null());
    assert!(!dest_type.is_null());
    //Conversion to CStr
    let source_cstr: &CStr = unsafe { CStr::from_ptr(src_type) };
    let dest_cstr: &CStr = unsafe { CStr::from_ptr(dest_type) };
    //Conversion to rust strings
    let source: String = source_cstr.to_str().unwrap().to_owned();
    let destination: String = dest_cstr.to_str().unwrap().to_owned();

    debug_assert!(&source[..2] == "__" && &destination[..2] == "__");
    debug_assert!(source.len() > 2 && destination.len() > 2);

    let filtered_source = &source[2..];
    let filtered_destination = &destination[2..];
    return type_checker::same_type(filtered_source, filtered_destination);
}

// Prints 64 bit signed integer
#[no_mangle]
pub extern "C" fn print64(num64: i64) {
    println!("{}", num64);
}

// Prints 32 bit signed integer
#[no_mangle]
pub extern "C" fn print32(num32: i32) {
    println!("{}", num32);
}

// Prints 16 bit signed integer
#[no_mangle]
pub extern "C" fn print16(num16: i16) {
    println!("{}", num16);
}

// Prints 8 bit signed integer
#[no_mangle]
pub extern "C" fn print8(num8: i8) {
    println!("{}", num8);
}

// Prints 64 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu64(num64: u64) {
    println!("{}", num64);
}

// Prints 32 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu32(num32: u32) {
    println!("{}", num32);
}

// Prints 16 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu16(num16: u16) {
    println!("{}", num16);
}

// Prints 8 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu8(num8: u8) {
    println!("{}", num8);
}

// Prints 64 bit float
#[no_mangle]
pub extern "C" fn printf64(num64: f64) {
    println!("{}", num64);
}

// Prints 32 bit float
#[no_mangle]
pub extern "C" fn printf32(num32: f32) {
    println!("{}", num32);
}

// Prints boolean
#[no_mangle]
pub extern "C" fn print_boolean(num8: i8) {
    if num8 == 0 {
        println!("false");
    } else {
        println!("true");
    }
}

#[no_mangle]
pub extern "C" fn array_init_int(size: i32) -> *mut Vec<*mut i32> {
    let size_t = if size > 0 { size } else { 8 };
    let size_t = size_t as usize;
    let foo: Box<Vec<*mut i32>> = Box::new(Vec::with_capacity(size_t));
    let vec_pointer = Box::into_raw(foo);
    return vec_pointer as *mut Vec<*mut i32>;
}

#[no_mangle]
pub extern "C" fn array_init_float(size: i32) -> *mut Vec<*mut f32> {
    let size_t = if size > 0 { size } else { 8 };
    let size_t = size_t as usize;
    let foo: Box<Vec<*mut f32>> = Box::new(Vec::with_capacity(size_t));
    let vec_pointer = Box::into_raw(foo);
    return vec_pointer as *mut Vec<*mut f32>;
}

#[no_mangle]
pub extern "C" fn array_init_bool(size: i32) -> *mut Vec<*mut bool> {
    let size_t = if size > 0 { size } else { 8 };
    let size_t = size_t as usize;
    let foo: Box<Vec<*mut bool>> = Box::new(Vec::with_capacity(size_t));
    let vec_pointer = Box::into_raw(foo);
    return vec_pointer as *mut Vec<*mut bool>;
}

#[no_mangle]
pub extern "C" fn array_init_string(size: i32) -> *mut Vec<*mut String> {
    let size_t = if size > 0 { size } else { 8 };
    let size_t = size_t as usize;
    let foo: Box<Vec<*mut String>> = Box::new(Vec::with_capacity(size_t));
    let vec_pointer = Box::into_raw(foo);
    return vec_pointer as *mut Vec<*mut String>;
}

#[no_mangle]
pub extern "C" fn array_store_int(arr_ptr: *mut Vec<*mut i32>, index: i32, ref_ptr: *mut i32) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    let len = index_n + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut i32);
    }
    arr[index_n] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_int(arr_ptr: *mut Vec<*mut i32>, index: i32) -> *mut i32 {
    let arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    // check the out of bounds.
    assert!(arr.len() > index_n);
    let return_val = arr[index_n];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_store_float(arr_ptr: *mut Vec<*mut f32>, index: i32, ref_ptr: *mut f32) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    let len = index_n + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut f32);
    }
    arr[index_n] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_float(arr_ptr: *mut Vec<*mut f32>, index: i32) -> *mut f32 {
    let arr = unsafe { Box::from_raw(arr_ptr)};
    let index_n = index as usize;
    // check the out of bounds.
    assert!(arr.len() > index_n);
    let return_val = arr[index_n];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_store_bool(arr_ptr: *mut Vec<*mut bool>, index: i32, ref_ptr: *mut bool) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    let len = index_n + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut bool);
    }
    arr[index_n] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_bool(arr_ptr: *mut Vec<*mut bool>, index: i32) -> *mut bool {
    let arr = unsafe { Box::from_raw(arr_ptr)};
    let index_n = index as usize;
    // check the out of bounds.
    assert!(arr.len() > index_n);
    let return_val = arr[index_n];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_store_string(arr_ptr: *mut Vec<*mut String>, index: i32, ref_ptr: *mut String) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    let len = index_n + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut String);
    }
    arr[index_n] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_string(arr_ptr: *mut Vec<*mut String>, index: i32) -> *mut String {
    let arr = unsafe { Box::from_raw(arr_ptr)};
    let index_n = index as usize;
    // check the out of bounds.
    assert!(arr.len() > index_n);
    let return_val = arr[index_n];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_deinit_int(ptr: *mut Vec<*mut i32>) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn array_deinit_float(ptr: *mut Vec<*mut f32>) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn array_deinit_bool(ptr: *mut Vec<*mut bool>) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn array_deinit_string(ptr: *mut Vec<*mut String>) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

// Ballerina Map implementation
#[no_mangle]
pub extern "C" fn map_new_int() -> *mut BalMapInt {
    Box::into_raw(Box::new(BalMapInt::new()))
}

#[no_mangle]
pub extern "C" fn map_deint_int(ptr: *mut BalMapInt) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn map_store_int(ptr: *mut BalMapInt, key: *const c_char, member_ptr: *const i32) {
    // Load BalMap from pointer
    assert!(!ptr.is_null());
    let bal_map = unsafe { &mut *ptr };
    // Load Key C string
    assert!(!key.is_null());
    let key = unsafe { CStr::from_ptr(key) };
    let key_str = key.to_str().unwrap();

    // Load member value
    assert!(!member_ptr.is_null());
    let member = unsafe { slice::from_raw_parts(member_ptr, 1) };
    // Insert new field
    bal_map.insert(key_str, member[0]);

    // Print length to test functionality
    println!("length={}", bal_map.length());
}

#[no_mangle]
pub extern "C" fn box_bal_int(val: i32) -> *mut i32 {
    Box::into_raw(Box::new(val))
}

#[no_mangle]
pub extern "C" fn unbox_bal_int(ptr: *mut i32) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn box_bal_float(val: f64) -> *mut f64 {
    Box::into_raw(Box::new(val))
}

#[no_mangle]
pub extern "C" fn unbox_bal_float(ptr: *mut f64) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn box_bal_bool(val: i8) -> *mut i8 {
    Box::into_raw(Box::new(val))
}

#[no_mangle]
pub extern "C" fn unbox_bal_bool(ptr: *mut f64) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn map_spread_field_init(ptr_source: *mut BalMapInt, ptr_expr: *mut BalMapInt) {
    // Load source BalMap from pointer
    assert!(!ptr_source.is_null());
    let map_src = unsafe { &mut *ptr_source };
    // Load expr BalMap from pointer
    assert!(!ptr_expr.is_null());
    let map_expr = unsafe { &mut *ptr_expr };
    // Insert from spread field expression
    map_src.insert_spread_field(map_expr);

    // Print length to test functionality
    println!("length={}", map_src.length());
}
