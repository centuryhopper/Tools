import 'package:PasswordManager/Services/AuthService.dart';
import 'package:flutter/material.dart';
import 'package:PasswordManager/Models/LoginDTO.dart';
import 'package:PasswordManager/navigator.dart';
import 'package:PasswordManager/pages/loginpage.dart';

const String appName = "PasswordManager";

/*
flutter emulators
flutter emulators --launch MyNewPixel6
flutter run
*/

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(AppHome());
}

class AppHome extends StatelessWidget {
  LoginDTO? loginDTO;

  AppHome({super.key});

  void updateAccount(LoginDTO loginDTO) {
    this.loginDTO = loginDTO;
  }

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: appName,
      theme: ThemeData(
        primarySwatch: Colors.teal,
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
      initialRoute: LoginPage.routeID,
      routes: {
        LoginPage.routeID: (ctx) => Theme(
          data: ThemeData.light(),
          child: FutureBuilder<String?>(
                future: AuthService.getToken(), // Replace 'token' with your key
                builder: (context, snapshot) {
                  if (snapshot.connectionState == ConnectionState.waiting) {
                    return const Center(
                        child:
                            CircularProgressIndicator()); // Show loading indicator while checking
                  }
          
          
                  if (snapshot.hasData && snapshot.data != null) {
          
                    // print('snapshot has data');
          
                    // If token exists, navigate to HomePage
                    return NavigationHelperWidget(
                        loginDTO); // Replace with your main app page
                  } else {
                    // If token does not exist, navigate to LoginPage
                    return LoginPage(appName: appName, appHome: this);
                  }
                },
              ),
        ),
        NavigationHelperWidget.routeID: (ctx) {
          return NavigationHelperWidget(loginDTO);
        }
      },
    );
  }
}
