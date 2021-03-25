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
    return type_checker::same_type(source, destination);
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

// Prints string
#[no_mangle]
pub extern "C" fn print_str(val: *const c_char) {
    let cstr: &CStr = unsafe { CStr::from_ptr(val) };
    let string: String = cstr.to_str().unwrap().to_owned();
    print!("{}", string);
    io::stdout().flush().unwrap();
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
pub extern "C" fn array_store_int(arr_ptr: *mut Vec<*mut i32>, n: i32, ref_ptr: *mut i32) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let n_size = n as usize;
    let len = n_size + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut i32);
    }
    arr[n_size] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_int(arr_ptr: *mut Vec<*mut i32>, n: i32) -> *mut i32 {
    let arr = unsafe { Box::from_raw(arr_ptr) };
    let n_size = n as usize;
    let return_val = arr[n_size];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_store_float(arr_ptr: *mut Vec<*mut f32>, n: i32, ref_ptr: *mut f32) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let n_size = n as usize;
    let len = n_size + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut f32);
    }
    arr[n_size] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_float(arr_ptr: *mut Vec<*mut f32>, n: i32) -> *mut f32 {
    let arr = unsafe { Box::from_raw(arr_ptr)};
    let n_size = n as usize;
    let return_val = arr[n_size];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_store_bool(arr_ptr: *mut Vec<*mut bool>, n: i32, ref_ptr: *mut bool) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let n_size = n as usize;
    let len = n_size + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut bool);
    }
    arr[n_size] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_bool(arr_ptr: *mut Vec<*mut bool>, n: i32) -> *mut bool {
    let arr = unsafe { Box::from_raw(arr_ptr)};
    let n_size = n as usize;
    let return_val = arr[n_size];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_store_string(arr_ptr: *mut Vec<*mut String>, n: i32, ref_ptr: *mut String) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let n_size = n as usize;
    let len = n_size + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut String);
    }
    arr[n_size] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_string(arr_ptr: *mut Vec<*mut String>, n: i32) -> *mut String {
    let arr = unsafe { Box::from_raw(arr_ptr)};
    let n_size = n as usize;
    let return_val = arr[n_size];
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
    let bal_map = unsafe {
        assert!(!ptr.is_null());
        &mut *ptr
    };
    // Load Key C string
    let key = unsafe {
        assert!(!key.is_null());
        CStr::from_ptr(key)
    };
    let key_str = key.to_str().unwrap();

    // Load member value
    let member = unsafe {
        assert!(!member_ptr.is_null());
        slice::from_raw_parts(member_ptr, 1)
    };
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
