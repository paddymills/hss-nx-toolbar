
use native_windows_gui;
use native_windows_derive;


#[derive(Default, native_windows_derive::NwgUi)]
pub struct MultiFileSelect {

    #[nwg_control(size: (10, 10), position: (100, 50))]
    #[nwg_events( OnWindowClose: [MultiFileSelect::exit], OnInit: [MultiFileSelect::show_file_dialog] )]
    window: native_windows_gui::Window,

    #[nwg_resource(title: "Select file(s)...", action: native_windows_gui::FileDialogAction::Open, filters: "NX Part(*.prt)", multiselect: true)]
    dialog: native_windows_gui::FileDialog,
}

impl MultiFileSelect {

    fn show_file_dialog(&self) {
        self.window.set_visible(false);

        if !self.dialog.run(Some(&self.window)) {
            println!("No files chosen");
        }

        self.exit();
    }

    pub fn set_folder(&self, folder: &str) {
        let _ = self.dialog.set_default_folder(folder);
    }

    pub fn get_files(&self) -> Vec<String> {
        let mut res: Vec<String> = Vec::new();

        match self.dialog.get_selected_items() {
            Ok(files) => {
                for f in files {
                    match f.into_string() {
                        Ok(filename) => res.push(filename),
                        _ => ()
                    };
                }
            },
            _ => println!("Failed to get file(s).")
        }

        res
    }


    fn exit(&self) {
        native_windows_gui::stop_thread_dispatch();

        self.window.close();
    }

}