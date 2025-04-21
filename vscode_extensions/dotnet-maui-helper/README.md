# MAUI XAML Switcher

A lightweight Visual Studio Code extension that makes it easy to switch between `.xaml` files and their `.xaml.cs` code-behind counterparts in .NET MAUI projects.

Press `Alt + O` to quickly toggle between the view and its logic — just like you're used to in full Visual Studio.

---

## ✨ Features

- ✅ **Quick toggle with `Alt + O`** between `.xaml` and `.xaml.cs`
- 🔍 **Context-aware** — only activates on relevant files
- 💡 **Zero configuration** — works out of the box for MAUI and WPF-style XAML projects
- ⚡ Super fast and non-intrusive

---

## 📸 Demo

![Toggle Demo]

> Tip: You can bind the command to another shortcut via VS Code settings if you prefer.

---

## 🔧 Usage

- Open a `.xaml` or `.xaml.cs` file.
- Press `Alt + O`.
- Instantly jump to the paired file in the same directory with the same base filename.

Example:

- From `MainPage.xaml` → `MainPage.xaml.cs`
- From `SettingsView.xaml.cs` → `SettingsView.xaml`

---

## 🧩 Requirements

This extension assumes a typical MAUI/WPF file structure:
- Both `.xaml` and `.xaml.cs` exist in the **same folder**.
- File names are identical (aside from the extensions).

No other configuration is needed.

---

## ⚙️ Extension Settings

This extension currently does not contribute any user-facing settings — it just works.

---

## 🐞 Known Issues

- Only supports files within the same folder. Nested or custom directory structures are not supported (yet).
- Doesn't support `.xaml` files without `.xaml.cs` companions (e.g., resource dictionaries).

---

## 🚀 Release Notes

### 1.0.0
- Initial release.
- `Alt + O` command to switch between `.xaml` and `.xaml.cs`.

---

## 💡 Contributing

Got an idea or bug? PRs and issues are welcome!

---

## 📚 Resources

- [VS Code Extension API](https://code.visualstudio.com/api)
- [MAUI Docs](https://learn.microsoft.com/en-us/dotnet/maui/)
- [XAML in .NET MAUI](https://learn.microsoft.com/en-us/dotnet/maui/user-interface/)

---

**Enjoy fast file navigation!**  
— *Happy coding with MAUI!* 🏝️
