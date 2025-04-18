// The module 'vscode' contains the VS Code extensibility API
// Import the module and reference it with the alias vscode in your code below
import * as vscode from 'vscode';

// This method is called when your extension is activated
// Your extension is activated the very first time the command is executed
export function activate(context: vscode.ExtensionContext)
{
    const disposable = vscode.commands.registerCommand('extension.switchXamlCodeBehind', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor)
		{
            vscode.window.showInformationMessage('No file is open.');
            return;
        }

        const currentFile = editor.document.uri.fsPath;

        const isXaml = currentFile.endsWith('.xaml');
        const isCodeBehind = currentFile.endsWith('.xaml.cs');

        let targetFilePath: string | null = null;

        if (isXaml) {
            targetFilePath = currentFile + '.cs'; // .xaml.cs
        } else if (isCodeBehind) {
            targetFilePath = currentFile.replace(/\.xaml\.cs$/, '.xaml');
        } else {
            vscode.window.showInformationMessage('Not a .xaml or .xaml.cs file.');
            return;
        }

        try {
            const doc = await vscode.workspace.openTextDocument(targetFilePath);
            vscode.window.showTextDocument(doc);
        } catch (error) {
            vscode.window.showErrorMessage(`Could not find file: ${targetFilePath}`);
        }
    });

    context.subscriptions.push(disposable);
}

// This method is called when your extension is deactivated
export function deactivate() {}
