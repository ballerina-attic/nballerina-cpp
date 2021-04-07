pub mod error {
    use crate::BString;
    pub struct BalError {
        error: *mut BString,
    }

    impl BalError {
        pub fn new(err: *mut BString) -> BalError {
            BalError {
                error: err,
            }
        }
    }
}
