import 'package:PasswordManager/Models/LoginDTO.dart';
import 'package:PasswordManager/Services/AuthService.dart';
import 'package:PasswordManager/constants.dart';
import 'package:PasswordManager/main.dart';
import 'package:PasswordManager/pages/passwordspage.dart';
import 'package:PasswordManager/pages/profilepage.dart';
import 'package:PasswordManager/pages/settingspage.dart';
import 'package:PasswordManager/statemanagement/bloc/DarkMode/darkmode_bloc.dart';
import 'package:PasswordManager/statemanagement/bloc/DarkMode/darkmode_state.dart';
import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:shared_preferences/shared_preferences.dart';

LoginDTO? _loginDTO;

// this helper widget will help avoid circular dependencies among the other dart files
class NavigationHelperWidget extends StatefulWidget {
  static const String routeID = "/helper";

  NavigationHelperWidget(LoginDTO? loginDTO, {super.key}) {
    _loginDTO = loginDTO;
  }

  @override
  _NavigationHelperWidgetState createState() => _NavigationHelperWidgetState();
}

class _NavigationHelperWidgetState extends State<NavigationHelperWidget>
    with WidgetsBindingObserver {
  int _selectedIndex = 0;

  final List<Widget> pages = [
    PasswordsPage(loginDTO: _loginDTO),
    const SettingsPage(),
    ProfilePage(loginDTO: _loginDTO),
  ];

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addObserver(this); // Add observer
  }

  @override
  void dispose() async {
    super.dispose();

    WidgetsBinding.instance.removeObserver(this); // Remove observer
    if (await AuthService.getToken() != null) {
      if (!await AuthService.getRememberMeFlag()) {
        // print("clearing login");
        await AuthService.clearLogin();
      }
    }
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) async {
    // App is going to the background
    if (state == AppLifecycleState.paused) {
      // print('app is paused');
      // clear local storage data if remember me flag wasnt checked when logging in
      if (await AuthService.getToken() != null) {
        if (!await AuthService.getRememberMeFlag()) {
          await AuthService.clearLogin();
        }
      }
    }
  }

  // Logout method
  void _logout() async {
    // Clear any user data or tokens here, for example:
    await AuthService.clearLogin();

    // Navigate back to login screen and remove all previous routes
    Navigator.of(context).pushAndRemoveUntil(
      MaterialPageRoute(builder: (context) => AppHome()),
      (Route<dynamic> route) => false,
    );
  }

  // navigate to the page based on the index selected
  void _onItemTapped(int bottomNavButtonIndex) {
    _selectedIndex = bottomNavButtonIndex;
    if (ourBottomNavBarLst[bottomNavButtonIndex].label?.toLowerCase() ==
        "logout") {
      // resetting it avoids the error
      _selectedIndex = 0;
      _logout();
    }
    setState(() {});
  }

  // Load dark mode preference
  Future<bool> _loadDarkModePreference() async {
    final prefs = await SharedPreferences.getInstance();
    return prefs.getBool('isDark') ?? false; // Default to false
  }

  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
        future: _loadDarkModePreference(),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return const Center(
                child: CircularProgressIndicator()); // Show loading indicator
          }

          // Once the future is complete, get the dark mode preference
          final isDarkMode = snapshot.data ?? false;

          return Scaffold(
            body: MultiBlocProvider(
              providers: [
                BlocProvider(create: (ctx) => DarkModeBloc(isDarkMode)),
              ],
              child: BlocBuilder<DarkModeBloc, DarkModeState>(
                  builder: (context, state) => Theme(
                        data:
                            state.isDark ? ThemeData.dark() : ThemeData.light(),
                        child: pages[_selectedIndex],
                      )),
            ),
            // https://api.flutter.dev/flutter/material/BottomNavigationBar-class.html
            // https://www.youtube.com/watch?v=elLkVWt7gRM&ab_channel=ProgrammingAddict
            bottomNavigationBar: BottomNavigationBar(
                items: ourBottomNavBarLst,
                currentIndex: _selectedIndex,
                selectedItemColor: Colors.black,
                onTap: _onItemTapped),
          );
        });
  }
}
