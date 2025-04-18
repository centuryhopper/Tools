import 'package:PasswordManager/Services/AuthService.dart';
import 'package:PasswordManager/main.dart';
import 'package:PasswordManager/statemanagement/bloc/DarkMode/darkmode_bloc.dart';
import 'package:PasswordManager/statemanagement/bloc/DarkMode/darkmode_event.dart';
import 'package:PasswordManager/statemanagement/bloc/DarkMode/darkmode_state.dart';
import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:fluttertoast/fluttertoast.dart';

class SettingsPage extends StatelessWidget {
  const SettingsPage({super.key});

  @override
  Widget build(BuildContext context) {
    /*

    Use context.watch<T>():
      When you need the UI to react to state changes.
      For widgets that should update when the bloc’s state changes (like displaying user data, status indicators, etc.).

    Use context.read<T>():
      When you only need to perform an action without needing to respond to state changes.
      For one-off actions, like submitting a form or invoking a method in the bloc.
    
     */
    final darkModeBloc = context.watch<DarkModeBloc>();
    return Scaffold(
          appBar: AppBar(
            title: const Text("Settings"),
          ),
          body: Center(
            child: Container(
              constraints: const BoxConstraints(maxWidth: 400),
              child: ListView(
                children: [
                  _SingleSection(
                    title: "General",
                    children: [
                      _CustomListTile(
                          title: "Dark Mode",
                          icon: Icons.dark_mode_outlined,
                          trailing: Switch(
                              value: darkModeBloc.state.isDark,
                              onChanged: (value) {
                                darkModeBloc.add(DarkModeToggle());
                              })),
                    ],
                  ),
                  const Divider(),
                  _SingleSection(
                    children: [
                      _CustomListTile(
                        title: "Help & Feedback",
                        icon: Icons.help_outline_rounded,
                        onTap: () {
                          Fluttertoast.showToast(
                              msg:
                                  'Please email vincenteri321@gmail.com for help and feedback.',
                              toastLength: Toast.LENGTH_LONG,
                              gravity: ToastGravity.CENTER,
                              timeInSecForIosWeb: 1,
                              textColor: Colors.black,
                              backgroundColor: Colors.greenAccent,
                              fontSize: 24.0);
                        },
                      ),
                      _CustomListTile(
                        title: "About",
                        icon: Icons.info_outline_rounded,
                        onTap: () {},
                      ),
                      _CustomListTile(
                        title: "Sign out",
                        icon: Icons.exit_to_app_rounded,
                        onTap: () async {
                          // print("logging out");
                          // Clear any user data or tokens here, for example:
                          await AuthService.clearLogin();
      
                          // Navigate back to login screen and remove all previous routes
                          Navigator.of(context).pushAndRemoveUntil(
                            MaterialPageRoute(builder: (context) => AppHome()),
                            (Route<dynamic> route) => false,
                          );
                        },
                      ),
                    ],
                  ),
                ],
              ),
            ),
          ),
        );
  }
}

class _CustomListTile extends StatelessWidget {
  final String title;
  final IconData icon;
  final Widget? trailing;
  final void Function()? onTap;
  const _CustomListTile(
      {required this.title, required this.icon, this.onTap, this.trailing});

  @override
  Widget build(BuildContext context) {
    return ListTile(
      title: Text(title),
      leading: Icon(icon),
      trailing: trailing,
      onTap: onTap,
    );
  }
}

class _SingleSection extends StatelessWidget {
  final String? title;
  final List<Widget> children;
  const _SingleSection({
    this.title,
    required this.children,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.start,
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        if (title != null)
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: Text(
              title!,
              style: const TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
            ),
          ),
        Column(
          children: children,
        ),
      ],
    );
  }
}
