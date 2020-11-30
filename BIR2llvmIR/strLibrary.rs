// String comparision implementation in Rust

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
