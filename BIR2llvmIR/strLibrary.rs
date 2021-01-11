// Rust Library

// String comparision
fn is_same_type(src_type : &str, dest_type : &str) -> bool
{
  let same_type:bool;
  if src_type == dest_type {
    same_type = true;
  }
  else
  {
    same_type = false;
  }
  return same_type;
}

// Returns string object based on passed string literal
#[no_mangle]
pub extern "C" fn str_to_strobj(cstring : &str) -> std::string::String
{
  println!("{}", cstring);
  let content_string = String::from(cstring);
  return content_string;
}

// Prints 64 bit signed integer
fn print64(num64 : i64)
{
   println!("{}", num64);
}

// Prints 32 bit signed integer
fn print32(num32 : i32)
{
   println!("{}", num32);
}

// Prints 16 bit signed integer
fn print16(num16 : i16)
{
   println!("{}", num16);
}

// Prints 8 bit signed integer
fn print8(num8 : i8)
{
   println!("{}", num8);
}

// Prints 64 bit unsigned integer
fn printu64(num64 : u64)
{
   println!("{}", num64);
}

// Prints 32 bit unsigned integer
fn printu32(num32 : u32)
{
   println!("{}", num32);
}

// Prints 16 bit unsigned integer
fn printu16(num16 : u16)
{
   println!("{}", num16);
}

// Prints 8 bit unsigned integer
fn printu8(num8 : u8)
{
   println!("{}", num8);
}

// Prints 64 bit float
fn printf64(num64 : f64)
{
   println!("{}", num64);
}

// Prints 32 bit float
fn printf32(num32 : f32)
{
   println!("{}", num32);
}

// Prints string
fn print_str(string : &str)
{
   println!("{}", string);
}

fn main()
{
  str_to_strobj("hello string literal");
}
