#![allow(dead_code)]

pub mod fileselect;
pub use fileselect::MultiFileSelect;

use native_windows_gui::NativeUi;
use pyo3::prelude::*;

#[pyfunction]
fn get_files(_py: Python, dir: &str) -> PyResult<Vec<String>> {
    let out = gui(dir);

    Ok(out)
}

fn gui(dir: &str) -> Vec<String> {
    native_windows_gui::init().expect("Failed to init Native Windows GUI");

    let app = MultiFileSelect::build_ui(Default::default()).expect("Failed to build app");
    app.set_folder(dir);

    native_windows_gui::dispatch_thread_events();

    app.get_files()
}

#[pymodule(name = "filedialog")]
fn exports(m: &Bound<'_, PyModule>) -> PyResult<()> {
    m.add_function(wrap_pyfunction!(get_files, m)?)
}
