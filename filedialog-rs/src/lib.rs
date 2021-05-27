
pub mod fileselect;
pub use fileselect::MultiFileSelect;

use native_windows_gui::NativeUi;
use pyo3::prelude::*;

#[pymodule]
fn filedialog(_py: Python, pymod: &PyModule) -> PyResult<()> {
    
    #[pyfn(pymod, "get_files")]
    fn get_files(_py: Python, dir: &str) -> PyResult<Vec<String>> {
        let out = gui(dir);

        Ok(out)
    }

    Ok(())
}

fn gui(dir: &str) -> Vec<String> {
    native_windows_gui::init().expect("Failed to init Native Windows GUI");

    let app = MultiFileSelect::build_ui(Default::default()).expect("Failed to build app");
    app.set_folder(dir);

    native_windows_gui::dispatch_thread_events();

    app.get_files()
}