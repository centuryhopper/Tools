use std::io::{self, Write};
use color_eyre::Result;
use crossterm::event::KeyModifiers;
use itertools::Itertools;
use chrono::{DateTime, Utc};
use rpassword::read_password;
use serde_with::{serde_as, DisplayFromStr};
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
use serde_json::json;
use style::palette::tailwind;
use unicode_width::UnicodeWidthStr;
use reqwest::Error;
use reqwest::header::{AUTHORIZATION, HeaderValue};

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

fn deserialize_from_int<'de, D>(deserializer: D) -> Result<String, D::Error>
where
    D: Deserializer<'de>,
{
    // Deserialize the ID as an integer
    let int = u32::deserialize(deserializer)?;
    // Convert the integer to a String
    Ok(int.to_string())
}

fn get_user_input(prompt: &str) -> String {
    print!("{}", prompt);
    io::stdout().flush().unwrap(); // Ensure prompt is printed immediately

    let mut input = String::new();
    io::stdin().read_line(&mut input).unwrap();
    input.trim().to_string() // Trim to remove newline
}

fn get_hidden_input(prompt: &str) -> String {
    print!("{}", prompt);
    io::stdout().flush().unwrap(); // Ensure prompt is printed immediately
    read_password().unwrap().trim().to_string()
}


#[derive(Serialize, Debug, Deserialize)]
struct LoginResponse {
    pub flag: bool,
    pub token: Option<String>,
    pub message: String,
}

impl LoginResponse {
    pub fn new(flag: bool, token: impl Into<String>, message: impl Into<String>) -> Self {
        Self {
            flag,
            token: Some(token.into()),
            message: message.into(),
        }
    }
}

#[derive(Serialize, Debug, Deserialize, Clone)]
struct PasswordAccountDTO {
    #[serde(deserialize_with = "deserialize_from_int")]
    id: String,
    #[serde(rename="userId", deserialize_with = "deserialize_from_int")]
    user_id: String,
    title: String,
    username: String,
    password: String,
    #[serde(rename="createdAt")]
    created_at: String,
    #[serde(rename="lastUpdatedAt")]
    last_updated_at: String,
}

impl PasswordAccountDTO {
    const fn ref_array(&self) -> [&String; 7] {
        [&self.id,
        &self.user_id,
        &self.title,
        &self.username,
        &self.password,
        &self.created_at,
        &self.last_updated_at,]
    }

    fn id(&self) -> &str {
        &self.id
    }

    fn user_id(&self) -> &str {
        &self.user_id
    }
    fn title(&self) -> &str {
        &self.title
    }
    fn username(&self) -> &str {
        &self.username
    }
    fn password(&self) -> &str {
        &self.password
    }
    fn created_at(&self) -> &str {
        &self.created_at
    }
    fn last_updated_at(&self) -> &str {
        &self.last_updated_at
    }
}

#[derive(Serialize, Debug, Deserialize)]
struct User {
    #[serde(deserialize_with = "deserialize_from_int")]
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

async fn fetch_passwords(jwt: &str) -> Result<Vec<PasswordAccountDTO>, Error> {
    let url = "https://mypasswordmanager-production-b6be.up.railway.app/api/PasswordManager/passwords";
    let client = reqwest::Client::new();
    let response = client
        .get(url)
        .header(AUTHORIZATION, HeaderValue::from_str(&format!("Bearer {}", jwt)).unwrap(),)
        .send()
        .await?;
    let password_accounts: Vec<PasswordAccountDTO> = response.json().await?;
    Ok(password_accounts)
}

async fn try_login(email: String, password: String) -> Result<LoginResponse, Error> {
    let url = "https://mypasswordmanager-production-b6be.up.railway.app/api/Account/login";
    let request_body = json!({
        "email": email,
        "password": password,
    });
    let client = reqwest::Client::new();
    let response = client.post(url)
    .json(&request_body)
    .send()
    .await?
    .json()
    .await?;
    Ok(response)
}

// Fetch users from the API
async fn fetch_users() -> Result<Vec<User>, Error> {
    let url = "https://mypasswordmanager-production-b6be.up.railway.app/api/PasswordManager/test";
    let response = reqwest::get(url).await?;
    let users: Vec<User> = response.json().await?;
    Ok(users)
}

#[tokio::main]
async fn main() -> Result<()> {
    
    let email = get_user_input("Enter your email: ");
    let password = get_hidden_input("Enter your password: ");
    let login_response = try_login(email, password).await?;
    // println!("{login_response:?}");

    if login_response.flag {
        color_eyre::install()?;
        let terminal = ratatui::init();
        let passwords = fetch_passwords(&login_response.token.unwrap()).await?;
        let passwords_clone = passwords.clone();
        let app_result = App::new(passwords).run(terminal);
        ratatui::restore();
        let mapped_passwords = passwords_clone.iter().map(|x| {
            serde_json::to_string_pretty(x).unwrap()
        }).collect::<String>();
        println!("{mapped_passwords}");
        return app_result;
    }
    Ok(())
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
    items: Vec<PasswordAccountDTO>,
    longest_item_lens: (u16, u16, u16, u16, u16, u16, u16 ), // order is (name, address, email)
    scroll_state: ScrollbarState,
    colors: TableColors,
    color_index: usize,
}

impl App {
    fn new(passwords: Vec<PasswordAccountDTO>) -> Self {
        let data_vec = passwords;
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

        /*
            id
            user_id
            title
            username
            password
            created_at
            last_updated_at
        */

        let header = ["Id", "UserId", "Title", "Username", "Password", "Created At", "Last Updated At"]
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
                Constraint::Min(self.longest_item_lens.4),
                Constraint::Min(self.longest_item_lens.5),
                Constraint::Min(self.longest_item_lens.6),
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


fn constraint_len_calculator(items: &[PasswordAccountDTO]) -> (u16, u16, u16, u16, u16, u16, u16) {
    let id_len = items
        .iter()
        .map(PasswordAccountDTO::id)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);
    let user_id_len = items
        .iter()
        .map(PasswordAccountDTO::user_id)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);
    let title_len = items
        .iter()
        .map(PasswordAccountDTO::title)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);
    let username_len = items
        .iter()
        .map(PasswordAccountDTO::username)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);
    let password_len = items
        .iter()
        .map(PasswordAccountDTO::password)
        .flat_map(str::lines)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);
    let created_at_len = items
        .iter()
        .map(PasswordAccountDTO::created_at)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);
    let last_updated_at_len = items
        .iter()
        .map(PasswordAccountDTO::last_updated_at)
        .map(UnicodeWidthStr::width)
        .max()
        .unwrap_or(0);

    #[allow(clippy::cast_possible_truncation)]
    (id_len as u16, user_id_len as u16, title_len as u16, username_len as u16, password_len as u16, created_at_len as u16, last_updated_at_len as u16)
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