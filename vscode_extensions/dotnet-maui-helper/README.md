# DOTNET MAUI Helper

A lightweight Visual Studio Code extension that makes it easy to switch between `.xaml` files and their `.xaml.cs` code-behind counterparts — as well as between `.cs` and `.UI.cs` files — in .NET MAUI projects.

Press `Alt + O` to quickly toggle between the view and its logic, or `Alt + P` to generate a missing code-behind file — just like you're used to in full Visual Studio.

---

## ✨ Features

- ✅ **Quick toggle with `Alt + O`** between:
  - `.xaml` ↔ `.xaml.cs`
  - `Page.cs` ↔ `Page.UI.cs`
- 🛠️ **Create missing code-behind with `Alt + P`**:
  - Generates `.xaml.cs` for `.xaml`
  - Generates `.UI.cs` for `Page.cs`
- 🔍 **Context-aware** — only activates on relevant files
- 💡 **Zero configuration** — works out of the box for MAUI and WPF-style XAML projects
- ⚡ Super fast and non-intrusive

---

## 📸 Demo

![Toggle Demo]

> Tip: You can bind the commands to other shortcuts via VS Code settings if you prefer.

---

## 🔧 Usage

### 🔁 Toggle (Alt + O)

- Open any of the following:
  - `MainPage.xaml`
  - `MainPage.xaml.cs`
  - `MainPage.cs`
  - `MainPage.UI.cs`
- Press `Alt + O`
- Instantly jump to the logically paired file in the same directory.

Examples:

- `MainPage.xaml` → `MainPage.xaml.cs`
- `SettingsView.xaml.cs` → `SettingsView.xaml`
- `TestPage.cs` → `TestPage.UI.cs`
- `TestPage.UI.cs` → `TestPage.cs`

---

### 🆕 Generate Code-Behind (Alt + P)

- Open either a:
  - `.xaml` file → generates a `MyPage.xaml.cs`
  - `Page.cs` file → generates a `Page.UI.cs`
- Press `Alt + P`
- A matching code-behind file will be created using a predefined template and opened automatically.

---

## 🧩 Requirements

This extension assumes a typical MAUI/WPF file structure:
- Paired files exist in the **same folder**
- File names share the same base name

No other configuration is needed.

---

## ⚙️ Extension Settings

This extension currently does not contribute any user-facing settings — it just works.

---

## 🐞 Known Issues

- Only supports files within the same folder. Nested or custom directory structures are not supported (yet).
- `.xaml` files without `.xaml.cs` companions aren't detected for toggling unless created via `Alt + P`.
- `.UI.cs` switching assumes naming convention like `MyPage.cs` ↔ `MyPage.UI.cs`.

---

## 🚀 Release Notes

### 1.0.0
- Initial release.
- `Alt + O` command to switch between `.xaml` and `.xaml.cs`.

### 1.1.0
- Added support for toggling between `.cs` and `.UI.cs`.

### 1.2.0
- Added `Alt + P` command to auto-generate missing code-behind files.

---

## 💡 Contributing

Got an idea or bug? PRs and issues are welcome!

---

## 📚 Resources

- [VS Code Extension API](https://code.visualstudio.com/api)
- [MAUI Docs](https://learn.microsoft.com/en-us/dotnet/maui/)
- [XAML in .NET MAUI](https://learn.microsoft.com/en-us/dotnet/maui/user-interface/)

---

**Enjoy fast file navigation and seamless code-behind generation!**  
— *Happy coding with MAUI!* 🏝️
