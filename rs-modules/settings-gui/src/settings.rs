
use eframe::{App, AppCreator, CreationContext, egui};

#[derive(Debug, Default)]
struct Post {
    name: String,
    active: bool
}

#[repr(C)]
#[derive(Debug, Default)]
pub struct SettingsApp {
    save_on_post: bool,

    posts: Vec<Post>,

    is_open: bool,
}

impl SettingsApp {
    const NAME: &str = "hss-post-settings";

    fn win_opts() -> eframe::NativeOptions {
        eframe::NativeOptions {
            always_on_top: true,
            decorated: false,
            default_theme: eframe::Theme::Light,
            maximized: true,
            transparent: true,
    
            ..Default::default()
        }
    }

    fn init(_cc: &CreationContext<'_>) -> Self {
        Self {
            is_open: true,

            // TODO: read from config
            posts: vec![
                Post { name: "Peddimat".into(), active: true },
                Post { name: "Mazak M2".into(), active: true },
                Post { name: "Mazak SV20".into(), active: true },
                Post { name: "Fryer VB120/VB160".into(), active: true },
            ],

            ..Default::default()
        }
    }

    pub fn new() -> AppCreator {
        Box::new(|cc| Box::new(Self::init(cc)))
    }

    pub fn run() {
        eframe::run_native(
            Self::NAME,
            Self::win_opts(),
            Self::new()
        );
    }
}

impl App for SettingsApp {
    fn update(&mut self, ctx: &egui::Context, frame: &mut eframe::Frame) {
        egui::Window::new("Post Options")
            .anchor(egui::Align2::CENTER_CENTER, [0., 0.])
            .open(&mut self.is_open)
            .collapsible(false)
            .show(ctx, |ui| {
                ui.vertical_centered(|ui| {
                    ui.group(|ui| {
                        ui.heading("Active Posts");
                        ui.separator();

                        for post in &mut self.posts {
                            ui.checkbox(&mut post.active, &post.name);
                        }
                    });

                    ui.checkbox(&mut self.save_on_post, "Save part after posting");

                    if ui.button("Save").clicked() {
                        frame.close();
                    }

                    if ui.button("Close").clicked() {
                        frame.close();
                    }
                });
            });
        
        if !self.is_open {
            frame.close();
        }
    }
}
