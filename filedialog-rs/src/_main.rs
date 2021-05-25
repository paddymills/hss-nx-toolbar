
mod fileselect;

use fileselect::MultiFileSelect;

use native_windows_gui::NativeUi;
use pyo3::prelude::*;

fn main() {
    native_windows_gui::init().expect("Failed to init Native Windows GUI");

    let app = MultiFileSelect::build_ui(Default::default()).expect("Failed to build app");
    app.set_folder(r"\\hssieng\hssedsserv\Jobs\dxf\test_files");

    native_windows_gui::dispatch_thread_events();

    println!("File(s) chosen:");
    for f in app.get_files() {
        println!("\t{:?}", f);
    }
}
