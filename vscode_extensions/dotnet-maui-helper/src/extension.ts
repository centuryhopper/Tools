// The module 'vscode' contains the VS Code extensibility API
// Import the module and reference it with the alias vscode in your code below
import * as vscode from 'vscode';

// use the "vsce package" command to generate a .vsix package for this project

// This method is called when your extension is activated
// Your extension is activated the very first time the command is executed
export function activate(context: vscode.ExtensionContext)
{
    // assumes code behind exists otherwise shows an error message
    const fileSwitchDisposable = vscode.commands.registerCommand('extension.switchCodeBehind', async () => {
        // just a debug statement
        // vscode.window.showInformationMessage("Switch command activated!");
        const editor = vscode.window.activeTextEditor;
        if (!editor)
		{
            vscode.window.showInformationMessage('No file is open.');
            return;
        }
        const currentFile = editor.document.uri.fsPath;

        const isXaml = currentFile.endsWith('.xaml');
        const isCodeBehind = currentFile.endsWith('.xaml.cs');

        let targetFilePath: string = "";

        switch (true)
        {
            case currentFile.endsWith(".xaml"):
                targetFilePath = currentFile + '.cs'; // .xaml.cs
                break;
            case currentFile.endsWith(".xaml.cs"):
                targetFilePath = currentFile.replace(/\.xaml\.cs$/, '.xaml');
                break;
            case currentFile.endsWith("Page.cs"):
                targetFilePath = currentFile.replace(/\.cs$/, '.UI.cs');
                break;
            case currentFile.endsWith("Page.UI.cs"):
                targetFilePath = currentFile.replace(/\.UI\.cs$/, '.cs');
                break;
            default:
                vscode.window.showInformationMessage('Not a .Net Maui related file.');
                return;
        }
       
        try {
            const doc = await vscode.workspace.openTextDocument(targetFilePath);
            vscode.window.showTextDocument(doc);
        } catch (error) {
            vscode.window.showErrorMessage(`Could not find file: ${targetFilePath}`);
        }
    });

    const createCodeBehindDisposable = vscode.commands.registerCommand('extension.createCodeBehind', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor) {
            vscode.window.showInformationMessage('No file is open.');
            return;
        }
    
        const currentFile = editor.document.uri.fsPath;
        let codeBehindPath : string = "";
        let codeBehindTemplate: string = "";

        switch (true)
        {
            case currentFile.endsWith(".xaml"):
                codeBehindPath = currentFile + ".cs";
                const match : RegExpMatchArray | null = currentFile.match(/([^\/\\]+)\.xaml$/);
                if (!match)
                {
                    return;
                }
                codeBehindTemplate = xamlCodeBehindTemplate(match[1]);
                break;
            case currentFile.endsWith("Page.cs"):
                codeBehindPath = currentFile.replace(/\.cs/, ".UI.cs");
                const csharpMarkupMatch : RegExpMatchArray | null = currentFile.match(/([^\/\\]+)\.cs$/);
                if (!csharpMarkupMatch)
                {
                    return;
                }
                codeBehindTemplate = csharpMarkupCodeBehindTemplate(csharpMarkupMatch[1]);
                break;
            default:
                vscode.window.showInformationMessage('File must end with either "Page.cs" or ".xaml"');
                return;
        }

        // create code-behind file
        try
        {
            const fileUri = vscode.Uri.file(codeBehindPath);
    
            // Check if file exists
            await vscode.workspace.fs.stat(fileUri);
            vscode.window.showInformationMessage('Code-behind file already exists.');
            return;
        } catch {
            // File doesn't exist — continue to create it
        }

        try {
            const encoder = new TextEncoder();
            const fileUri = vscode.Uri.file(codeBehindPath);
            await vscode.workspace.fs.writeFile(fileUri, encoder.encode(codeBehindTemplate));
    
            const doc = await vscode.workspace.openTextDocument(fileUri);
            await vscode.window.showTextDocument(doc);
            vscode.window.showInformationMessage('Code-behind file created.');
        } catch (error) {
            vscode.window.showErrorMessage(`Failed to create code-behind file: ${error}`);
        }
    });
    
    context.subscriptions.push(fileSwitchDisposable);
    context.subscriptions.push(createCodeBehindDisposable);
}

const xamlCodeBehindTemplate = (className: string) : string => `
using System;
using Microsoft.Maui.Controls;

namespace YourAppNamespace
{
    public partial class ${className} : ContentPage
    {
        public ${className}()
        {
            InitializeComponent();
        }
    }
}
    `;

const csharpMarkupCodeBehindTemplate = (className: string) : string => `
using CommunityToolkit.Maui.Markup;

public partial class ${className} : ContentPage
{
	private View BuildContent()
	{
		var layout = new VerticalStackLayout
		{
			VerticalOptions = LayoutOptions.Center,
			HorizontalOptions = LayoutOptions.Center,
			Children = {
				new Label {
					Text = "${className}"
				}.Center(),
			},
		};

		return layout;
	}
}
`;

// This method is called when your extension is deactivated
export function deactivate() {}
