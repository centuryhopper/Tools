
use color_eyre::Result;
use crossterm::event::KeyModifiers;
use itertools::Itertools;
use ratatui::{
    crossterm::event::{self, Event, KeyCode, KeyEventKind},
    layout::{Constraint, Layout, Margin, Rect},
    style::{self, Color, Modifier, Style, Stylize},
    text::Text,
    widgets::{
        Block, BorderType, Cell, HighlightSpacing, Paragraph, Row, Scrollbar, ScrollbarOrientation,
        ScrollbarState, Table, TableState,
    },
    DefaultTerminal, Frame,
};

use serde::{Serialize, Deserialize, Deserializer};
use style::palette::tailwind;
use unicode_width::UnicodeWidthStr;
use reqwest::Error;

const PALETTES: [tailwind::Palette; 4] = [
    tailwind::BLUE,
    tailwind::EMERALD,
    tailwind::INDIGO,
    tailwind::RED,
];

const INFO_TEXT: [&str; 2] = [
    "(Esc) quit | (↑) move up | (↓) move down | (←) move left | (→) move right",
    "(Shift + →) next color | (Shift + ←) previous color",
];

const ITEM_HEIGHT: usize = 4;

fn deserialize_id_from_int<'de, D>(deserializer: D) -> Result<String, D::Error>
where
    D: Deserializer<'de>,
{
    // Deserialize the ID as an integer
    let id_int = u32::deserialize(deserializer)?;
    // Convert the integer to a String
    Ok(id_int.to_string())
}

#[derive(Serialize, Debug, Deserialize)]
struct User {
    #[serde(deserialize_with = "deserialize_id_from_int")]
    id: String,
    name: String,
    username: String,
    email: String,
}

impl User {
    const fn ref_array(&self) -> [&String; 4] {
        [&self.id, &self.name, &self.username, &self.email]
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn username(&self) -> &str {
        &self.username
    }

    fn id(&self) -> &str {
        &self.id
    }

    fn email(&self) -> &str {
        &self.email
    }
}

// Fetch users from the API
async fn fetch_users() -> Result<Vec<User>, Error> {
    let url = "https://jsonplaceholder.typicode.com/users";
    let response = reqwest::get(url).await?;
    let users: Vec<User> = response.json().await?;
    Ok(users)
}

#[tokio::main]
async fn main() -> Result<()> {
    color_eyre::install()?;
    let terminal = ratatui::init();
    let users = fetch_users().await?;
    // let mappedUsers = users.iter().map(|x| {
    //     serde_json::to_string_pretty(x).unwrap()
    // }).collect::<String>();
    // println!("{mappedUsers}");
    // Ok(())
    let app_result = App::new(users).run(terminal);
    ratatui::restore();
    app_result
}
struct TableColors {
    buffer_bg: Color,
    header_bg: Color,
    header_fg: Color,
    row_fg: Color,
    selected_row_style_fg: Color,
    selected_column_style_fg: Color,
    selected_cell_style_fg: Color,
    normal_row_color: Color,
    alt_row_color: Color,
    footer_border_color: Color,
}

impl TableColors {
    const fn new(color: &tailwind::Palette) -> Self {
        Self {
            buffer_bg: tailwind::SLATE.c950,
            header_bg: color.c900,
            header_fg: tailwind::SLATE.c200,
            row_fg: tailwind::SLATE.c200,
            selected_row_style_fg: color.c400,
            selected_column_style_fg: color.c400,
            selected_cell_style_fg: color.c600,
            normal_row_color: tailwind::SLATE.c950,
            alt_row_color: tailwind::SLATE.c900,
            footer_border_color: color.c400,
        }
    }
}

struct App {
    state: TableState,
    items: Vec<User>,
    longest_item_lens: (u16, u16, u16, u16), // order is (name, address, email)
    scroll_state: ScrollbarState,
    colors: TableColors,
    color_index: usize,
}

impl App {
    fn new(users: Vec<User>) -> Self {
        let data_vec = users;
        Self {
            state: TableState::default().with_selected(0),
            longest_item_lens: constraint_len_calculator(&data_vec),
            scroll_state: ScrollbarState::new((data_vec.len() - 1) * ITEM_HEIGHT),
            colors: TableColors::new(&PALETTES[0]),
            color_index: 0,
            items: data_vec,
        }
    }
    pub fn next_row(&mut self) {
        let i = match self.state.selected() {
            Some(i) => {
                if i >= self.items.len() - 1 {
                    0
                } else {
                    i + 1
                }
            }
            None => 0,
        };
        self.state.select(Some(i));
        self.scroll_state = self.scroll_state.position(i * ITEM_HEIGHT);
    }

    pub fn previous_row(&mut self) {
        let i = match self.state.selected() {
            Some(i) => {
                if i == 0 {
                    self.items.len() - 1
                } else {
                    i - 1
                }
            }
            None => 0,
        };
        self.state.select(Some(i));
        self.scroll_state = self.scroll_state.position(i * ITEM_HEIGHT);
    }

    pub fn next_column(&mut self) {
        self.state.select_next_column();
    }

    pub fn previous_column(&mut self) {
        self.state.select_previous_column();
    }

    pub fn next_color(&mut self) {
        self.color_index = (self.color_index + 1) % PALETTES.len();
    }

    pub fn previous_color(&mut self) {
        let count = PALETTES.len();
        self.color_index = (self.color_index + count - 1) % count;
    }

    pub fn set_colors(&mut self) {
        self.colors = TableColors::new(&PALETTES[self.color_index]);
    }

    fn run(mut self, mut terminal: DefaultTerminal) -> Result<()> {
        loop {
            terminal.draw(|frame| self.draw(frame))?;

            if let Event::Key(key) = event::read()? {
                if key.kind == KeyEventKind::Press {
                    let shift_pressed = key.modifiers.contains(KeyModifiers::SHIFT);
                    match key.code {
                        KeyCode::Char('q') | KeyCode::Esc => return Ok(()),
                        KeyCode::Char('j') | KeyCode::Down => self.next_row(),
                        KeyCode::Char('k') | KeyCode::Up => self.previous_row(),
                        KeyCode::Char('l') | KeyCode::Right if shift_pressed => self.next_color(),
                        KeyCode::Char('h') | KeyCode::Left if shift_pressed => {
                            self.previous_color();
                        }
                        KeyCode::Char('l') | KeyCode::Right => self.next_column(),
                        KeyCode::Char('h') | KeyCode::Left => self.previous_column(),
                        _ => {}
                    }
                }
            }
        }
    }

    fn draw(&mut self, frame: &mut Frame) {
        let vertical = &Layout::vertical([Constraint::Min(5), Constraint::Length(4)]);
        let rects = vertical.split(frame.area());

        self.set_colors();

        self.render_table(frame, rects[0]);
        self.render_scrollbar(frame, rects[0]);
        self.render_footer(frame, rects[1]);
    }

    fn render_table(&mut self, frame: &mut Frame, area: Rect) {
        let header_style = Style::default()
            .fg(self.colors.header_fg)
            .bg(self.colors.header_bg);
        let selected_row_style = Style::default()
            .add_modifier(Modifier::REVERSED)
            .fg(self.colors.selected_row_style_fg);
        let selected_col_style = Style::default().fg(self.colors.selected_column_style_fg);
        let selected_cell_style = Style::default()
            .add_modifier(Modifier::REVERSED)
            .fg(self.colors.selected_cell_style_fg);

        let header = ["Id", "Name", "Username", "Email"]
            .into_iter()
            .map(Cell::from)
            .collect::<Row>()
            .style(header_style)
            .height(1);
        let rows = self.items.iter().enumerate().map(|(i, data)| {
            let color = match i % 2 {
                0 => self.colors.normal_row_color,
                _ => self.colors.alt_row_color,
            };
            let item = data.ref_array();
            item.into_iter()
                .map(|content| Cell::from(Text::from(format!("\n{content}\n"))))
                .collect::<Row>()
                .style(Style::new().fg(self.colors.row_fg).bg(color))
                .height(4)
        });
        let bar = " █ ";
        let t = Table::new(
            rows,
            // columns
            [
                // + 1 is for padding.
                Constraint::Length(self.longest_item_lens.0 + 1),
                Constraint::Min(self.longest_item_lens.1 + 1),
                Constraint::Min(self.longest_item_lens.2),
                Constraint::Min(self.longest_item_lens.3),
            ],
        )
        .header(header)
        .row_highlight_style(selected_row_style)
        .column_highlight_style(selected_col_style)
        .cell_highlight_style(selected_cell_style)
        .highlight_symbol(Text::from(vec![
            "".into(),
            bar.into(),
            bar.into(),
            "".into(),
        ]))
        .bg(self.colors.buffer_bg)
        .highlight_spacing(HighlightSpacing::Always);
        frame.render_stateful_widget(t, area, &mut self.state);
    }

    fn render_scrollbar(&mut self, frame: &mut Frame, area: Rect) {
        frame.render_stateful_widget(
            Scrollbar::default()
                .orientation(ScrollbarOrientation::VerticalRight)
                .begin_symbol(None)
                .end_symbol(None),
            area.inner(Margin {
                vertical: 1,
                horizontal: 1,
            }),
            &mut self.scroll_state,
        );
    }

    fn render_footer(&self, frame: &mut Frame, area: Rect) {
        let info_footer = Paragraph::new(Text::from_iter(INFO_TEXT))
            .style(
                Style::new()
                    .fg(self.colors.row_fg)
                    .bg(self.colors.buffer_bg),
            )
            .centered()
            .block(
                Block::bordered()
                    .border_type(BorderType::Double)
                    .border_style(Style::new().fg(self.colors.footer_border_color)),
            );
        frame.render_widget(info_footer, area);
    }
}


fn constraint_len_calculator(items: &[User]) -> (u16, u16, u16, u16) {
    let name_len = items
        .iter()
        .map(User::name)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);
    let id_len = items
        .iter()
        .map(User::id)
        .flat_map(str::lines)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);
    let email_len = items
        .iter()
        .map(User::email)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);
    let username_len = items
        .iter()
        .map(User::username)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);

    #[allow(clippy::cast_possible_truncation)]
    (name_len as u16, id_len as u16, email_len as u16, username_len as u16)
}


#[cfg(test)]
mod tests {
    // use crate::Data;

    #[test]
    fn constraint_len_calculator() {
        // let test_data = vec![
        //     Data {
        //         name: "Emirhan Tala".to_string(),
        //         address: "Cambridgelaan 6XX\n3584 XX Utrecht".to_string(),
        //         email: "tala.emirhan@gmail.com".to_string(),
        //     },
        //     Data {
        //         name: "thistextis26characterslong".to_string(),
        //         address: "this line is 31 characters long\nbottom line is 33 characters long"
        //             .to_string(),
        //         email: "thisemailis40caharacterslong@ratatui.com".to_string(),
        //     },
        // ];
        // let (longest_name_len, longest_address_len, longest_email_len) =
        //     crate::constraint_len_calculator(&test_data);

        // assert_eq!(26, longest_name_len);
        // assert_eq!(33, longest_address_len);
        // assert_eq!(40, longest_email_len);
    }
}