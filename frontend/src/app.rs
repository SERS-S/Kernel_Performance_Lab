use crate::backend_api::{BackendApi, RunItem};
use eframe::egui;

pub struct KplApp {
    api: BackendApi,
    runs: Vec<RunItem>,
    selected_run: Option<i32>,
    scenario_input: String,
}

impl KplApp {
    pub fn new(api: BackendApi) -> Self {
        let mut app = Self {
            api,
            runs: Vec::new(),
            selected_run: None,
            scenario_input: "fio_smoke".to_string(),
        };
        // Загружаем список при старте
        app.refresh_runs();
        app
    }

    fn refresh_runs(&mut self) {
        self.runs = self.api.list_runs();
    }
}

impl eframe::App for KplApp {
    fn ui(&mut self, ctx: &mut eframe::egui::Ui, _frame: &mut eframe::Frame) {
        egui::TopBottomPanel::top("top_panel").show(ctx, |ui| {
            egui::menu::bar(ui, |ui| {
                ui.heading("Kernel Performance Lab");

                ui.with_layout(egui::Layout::right_to_left(egui::Align::Center), |ui| {
                    if ui.button("Refresh").clicked() {
                        self.refresh_runs();
                    }
                });
            });
        });

        egui::SidePanel::left("left_panel")
            .resizable(true)
            .show(ctx, |ui| {
                ui.heading("Runs");
                ui.separator();

                egui::ScrollArea::vertical().show(ui, |ui| {
                    for run in &self.runs {
                        let label = format!("#{} {} [{}]", run.id, run.scenario, run.status);
                        if ui
                            .selectable_label(self.selected_run == Some(run.id), label)
                            .clicked()
                        {
                            self.selected_run = Some(run.id);
                        }
                    }
                });
            });

        egui::CentralPanel::default().show(ctx, |ui| {
            ui.heading("Dashboard");
            ui.separator();

            ui.group(|ui| {
                ui.label("Start new benchmark:");
                ui.horizontal(|ui| {
                    ui.text_edit_singleline(&mut self.scenario_input);
                    if ui.button("Start Run").clicked() {
                        self.api.start_run(&self.scenario_input);
                        self.refresh_runs();
                    }
                });
            });

            ui.separator();

            if let Some(run_id) = self.selected_run {
                if let Some(run) = self.runs.iter().find(|r| r.id == run_id) {
                    ui.heading(format!("Run Details: #{}", run.id));
                    ui.label(format!("Scenario: {}", run.scenario));
                    ui.label(format!("Status: {}", run.status));
                }
            } else {
                ui.label("Select a run from the left panel to view details.");
            }
        });
    }
}
