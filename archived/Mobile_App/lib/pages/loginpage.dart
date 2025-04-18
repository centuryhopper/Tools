import 'dart:async';

import 'package:flutter/material.dart';
import 'package:fluttertoast/fluttertoast.dart';
import 'package:PasswordManager/Services/AuthService.dart';
import 'package:PasswordManager/main.dart';
import 'package:PasswordManager/navigator.dart';
import 'package:url_launcher/url_launcher.dart';

class LoginPage extends StatefulWidget {
  static const String routeID = "/login";
  const LoginPage({
    super.key,
    String appName = "",
    AppHome? appHome,
  });

  @override
  State<LoginPage> createState() => _LoginPageState();
}

class _LoginPageState extends State<LoginPage> {
  bool rememberMe = false;
  bool obscureText = true;
  final TextEditingController? emailController = TextEditingController();
  final TextEditingController? passwordController = TextEditingController();
  final GlobalKey<FormState> formKey = GlobalKey<FormState>();
  int loginAttempts = 0;
  bool showLoginTimeout = false;
  Timer? countdownTimer;
  int remainingSeconds = 60;

  @override
  void initState() {
    super.initState();
    // for testing purposes
    // AuthService.clearLogin();
    // var waitForToken = AuthService.getToken();
    // waitForToken.then((val) {
    //   print('retrieved token: $val');
    // }).catchError((err) => print(err));

    // var waitForRememberMeFlag = AuthService.getRememberMeFlag();
    // waitForRememberMeFlag.then((val) {
    //   print('retrieved rememberMeFlag: $val');
    // }).catchError((err) => print(err));
  }

  @override
  void dispose() {
    // Clean up timer when widget is disposed
    countdownTimer?.cancel();
    emailController?.dispose();
    passwordController?.dispose();
    super.dispose();
  }

  void startCountdown() {
    countdownTimer = Timer.periodic(const Duration(seconds: 1), (timer) {
      if (remainingSeconds <= 0) {
        countdownTimer?.cancel();
        showLoginTimeout = false;
        loginAttempts = 0;
      } else {
        remainingSeconds--;
      }
      setState(() {});
    });
  }

  String? emailValidator(String? value) {
    if (value == null || value.isEmpty) {
      return 'Please enter your email';
    }

    String pattern = r'^[a-zA-Z0-9._]+@[a-zA-Z]+\.[a-zA-Z]+';
    RegExp regex = RegExp(pattern);
    if (!regex.hasMatch(value)) {
      return 'Please enter a valid email address';
    }
    return null;
  }

  String? passwordValidator(String? value) {
    if (value == null || value.isEmpty) {
      return 'Please enter your password';
    }
    return null;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.white,
      appBar: AppBar(
        title: const Text("Login Page"),
      ),
      body: SingleChildScrollView(
        child: Form(
          key: formKey,
          child: Column(
            children: <Widget>[
              Padding(
                padding: const EdgeInsets.only(top: 60.0),
                child: Center(
                  child: SizedBox(
                      width: 200,
                      height: 150,
                      /*decoration: BoxDecoration(
                          color: Colors.red,
                          borderRadius: BorderRadius.circular(50.0)),*/
                      child: Image.asset(
                          'assets/images/password_manager_flaticon.png')),
                ),
              ),
              Padding(
                //padding: const EdgeInsets.only(left:15.0,right: 15.0,top:0,bottom: 0),
                padding: const EdgeInsets.symmetric(horizontal: 15),
                child: TextFormField(
                  controller: emailController,
                  decoration: const InputDecoration(
                      border: OutlineInputBorder(),
                      labelText: 'Email',
                      hintText: 'Enter a valid email such as abc@gmail.com'),
                  validator: emailValidator,
                ),
              ),
              Padding(
                padding: const EdgeInsets.only(
                    left: 15.0, right: 15.0, top: 15, bottom: 0),
                //padding: EdgeInsets.symmetric(horizontal: 15),
                child: TextFormField(
                  controller: passwordController,
                  obscureText: obscureText,
                  decoration: InputDecoration(
                      suffixIcon: IconButton(
                        icon: Icon(
                          obscureText ? Icons.visibility : Icons.visibility_off,
                        ),
                        onPressed: () {
                          obscureText = !obscureText;
                          setState(() {});
                        },
                      ),
                      border: const OutlineInputBorder(),
                      labelText: 'Password',
                      hintText: 'Enter a secure password'),
                  validator: passwordValidator,
                ),
              ),
              // Remember Me checkbox
              CheckboxListTile(
                title: const Text("Remember Me"),
                value: rememberMe,
                onChanged: (bool? newValue) {
                  rememberMe = newValue ?? false;
                  setState(() {});
                },
                controlAffinity: ListTileControlAffinity.leading,
              ),
              // const SizedBox(
              //   height: 30,
              // ),
              // ElevatedButton(
              //   onPressed: () {
              //     //TODO: redirect user to user management system
              //   },
              //   child: const Text(
              //     'Forgot Password',
              //     style: TextStyle(color: Colors.blue, fontSize: 15),
              //   ),
              // ),
              const SizedBox(
                height: 30,
              ),
              Container(
                height: 50,
                width: 250,
                decoration: BoxDecoration(
                    color: Colors.blue,
                    borderRadius: BorderRadius.circular(20)),
                child: ElevatedButton(
                  onPressed: () async {
                    if (!formKey.currentState!.validate()) {
                      // If form is invalid, do not proceed
                      return;
                    }

                    if (loginAttempts > 5) {
                      if (showLoginTimeout) {
                        return;
                      }
                      showLoginTimeout = true;
                      remainingSeconds = 60;
                      startCountdown();
                      return;
                    }

                    var response = await AuthService.login(
                        emailController?.text ?? "",
                        passwordController?.text ?? "",
                        rememberMe);

                    // print("result: ${response.toJson()}");

                    if (!response.flag) {
                      Fluttertoast.showToast(
                          msg: response.message,
                          toastLength: Toast.LENGTH_SHORT,
                          gravity: ToastGravity.CENTER,
                          timeInSecForIosWeb: 1,
                          textColor: Colors.black,
                          backgroundColor: Colors.redAccent,
                          fontSize: 24.0);
                      loginAttempts++;
                      setState(() {});
                      return;
                    }

                    Fluttertoast.showToast(
                        msg: response.message,
                        toastLength: Toast.LENGTH_LONG,
                        gravity: ToastGravity.CENTER,
                        timeInSecForIosWeb: 1,
                        textColor: Colors.black,
                        backgroundColor: Colors.greenAccent,
                        fontSize: 24.0);

                    await AuthService.decodeToken(response.token!);
                    await Future.delayed(const Duration(seconds: 6));
                    Navigator.pushNamed(
                        context, NavigationHelperWidget.routeID);

                    // if token exists then save login info depending on rememberMe state
                    // var value = await AuthService.getToken();
                    // print('value: $value');
                  },
                  child: const Text(
                    'Login',
                    style: TextStyle(color: Colors.black, fontSize: 25),
                  ),
                ),
              ),
              Visibility(
                visible: showLoginTimeout,
                child: Text(
                    'You\'re doing that too much.\nPlease wait ${remainingSeconds}s and try again.',
                    textAlign: TextAlign.center,
                    style: const TextStyle(fontSize: 25, color: Colors.red)),
              ),
              const SizedBox(
                height: 130,
              ),
              GestureDetector(
                onTap: () async {
                  const String url = 'https://dotnetusermanagementsystem-production.up.railway.app/';
                  final Uri uri = Uri.parse(
                      url);
                  if (await canLaunchUrl(uri)) {
                    await launchUrl(uri);
                  } else {
                    throw 'Could not launch $url';
                  }
                },
                child: const Text(
                  'New User? Create Account',
                  style: TextStyle(
                    color: Colors.blue,
                    decoration: TextDecoration.underline,
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
