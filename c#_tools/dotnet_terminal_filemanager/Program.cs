using Terminal.Gui;
using System.IO;

class Program
{
    static void Main(string[] args)
    {
        Application.Init();

        // Set the background color to black
        Application.Top.ColorScheme = new ColorScheme
        {
            Normal = Terminal.Gui.Attribute.Make(Color.White, Color.Black),
            Focus = Terminal.Gui.Attribute.Make(Color.White, Color.Black),
            HotNormal = Terminal.Gui.Attribute.Make(Color.White, Color.Black),
            HotFocus = Terminal.Gui.Attribute.Make(Color.White, Color.Black),
            Disabled = Terminal.Gui.Attribute.Make(Color.White, Color.Black),
        };

        var top = Application.Top;

        var win = new Window("File Manager (Ctrl+Q to quit)")
        {
            X = 0,
            Y = 0,
            Width = Dim.Fill(),
            Height = Dim.Fill()
        };

        var fileListView = new ListView()
        {
            X = 0,
            Y = 0,
            Width = Dim.Fill(),
            Height = Dim.Fill() - 1,
            AllowsMarking = false
        };

        var fileSystem = new FileSystem();

        void RefreshContent()
        {
            // fileSystem.GetFiles().ToList().ForEach(f => System.Console.WriteLine(f));
            fileListView.SetSource(fileSystem.GetFiles());
        }

        var statusBar = new StatusBar(new StatusItem[] {
            new StatusItem(Key.F5, "~F5~ Refresh", () => {
                System.Console.WriteLine("refreshing");
                RefreshContent();
            }),
            // new StatusItem(Key.Q, "~Q~ Quit", () => { 
            //     System.Console.WriteLine("stopping app");
            //     top.Running = false;
            //     Application.RequestStop(top: top);
            // }),
        });

        var frame = new FrameView("Files")
        {
            X = 0,
            Y = 1,
            Width = Dim.Fill(),
            Height = Dim.Fill()
        };

        fileListView.OpenSelectedItem += (e) =>
        {
            var selectedFile = fileSystem.GetFiles()[fileListView.SelectedItem];

            if (File.Exists(selectedFile))
            {
                var fileInfo = new FileInfo(selectedFile);
                MessageBox.Query("File Information", $"Name: {fileInfo.Name}\nSize: {fileInfo.Length} bytes", "OK");
            }
            else if (Directory.Exists(selectedFile))
            {
                fileSystem.ChangeDirectory(selectedFile);
                RefreshContent();
            }
        };

        bool isCursorRightPressed = false;
        bool isLeftArrowPressed = false; 

        // DO NOT USE ENTER KEY AS IT IS DEFAULTED TO SELECTION IN THIS APP
        // DOING SO CAUSES WEIRD BEHAVIOR
        fileListView.KeyUp += (e) =>
        {
            if (e.KeyEvent.Key == Key.CursorRight)
            {
                isCursorRightPressed = false;
            }
            else if (e.KeyEvent.Key == Key.CursorLeft)
            {
                isLeftArrowPressed = false;
            }
        };
        
        // Handle the left arrow key to move back to the parent directory
        fileListView.KeyDown += (e) =>
        {
            if (e.KeyEvent.Key == Key.CursorRight && !isCursorRightPressed)
            {
                isCursorRightPressed = true;
                if (fileSystem.GetFiles().Any())
                {
                    // System.Console.WriteLine(fileListView.SelectedItem);
                    var selectedFile = fileSystem.GetFiles()[fileListView.SelectedItem];

                    if (Directory.Exists(selectedFile))
                    {
                        System.Console.WriteLine("most recently visited directory: " + selectedFile);
                        fileSystem.ChangeDirectory(selectedFile);
                        RefreshContent();
                    }
                }
            }
            else if (e.KeyEvent.Key == Key.CursorLeft && !isLeftArrowPressed)
            {
                isLeftArrowPressed = true;
                var parentDirectory = Directory.GetParent(fileSystem.GetCurrentDirectory())?.FullName;
                // System.Console.WriteLine($"parentDirectory: {parentDirectory}");
                if (parentDirectory != null)
                {
                    System.Console.WriteLine("most recently visited directory: " + parentDirectory);
                    fileSystem.ChangeDirectory(parentDirectory);
                    RefreshContent();
                }
            }
        };

        frame.Add(fileListView);
        win.Add(frame);
        top.Add(win, statusBar);
        RefreshContent();

        Application.Run();
    }
}

public class FileSystem
{
    private string currentDirectory;

    public FileSystem()
    {
        currentDirectory = Directory.GetCurrentDirectory();
    }

    public string GetCurrentDirectory()
    {
        return currentDirectory;
    }

    public string[] GetFiles()
    {
        return Directory.GetFileSystemEntries(currentDirectory);
    }

    public void ChangeDirectory(string newDirectory)
    {
        currentDirectory = newDirectory;
    }
}
