import 'package:PasswordManager/Models/LoginDTO.dart';
import 'package:PasswordManager/Models/PasswordAccountDTO.dart';
import 'package:PasswordManager/Services/AuthService.dart';
import 'package:PasswordManager/Services/password_manager_service.dart';
import 'package:PasswordManager/passwordtable.dart';
import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:fluttertoast/fluttertoast.dart';

// show passwords in a grid with futurebuilder and toggle showing the passwords

class PasswordsPage extends StatefulWidget {
  const PasswordsPage({
    super.key,
    LoginDTO? loginDTO,
  });

  @override
  State<PasswordsPage> createState() => _PasswordsPageState();
}

class _PasswordsPageState extends State<PasswordsPage> {
  // Simulating an API call to fetch password accounts
  Future<List<PasswordAccountDTO>?> fetchPasswordAccounts() async {
    var passwordAccounts = await PasswordManagerService.getPasswordAccounts();
    if (passwordAccounts == null) {
      throw Exception("Couldn't retrieve password accounts from the server");
    }
    return passwordAccounts;
  }

  String? passwordValidator(String? value) {
    if (value == null || value.isEmpty) {
      return 'Please enter your password';
    }
    return null;
  }

  bool obscureText = true;
  bool isVisible = false;
  late TextEditingController passwordController;

  @override
  void initState() {
    super.initState();
    passwordController = TextEditingController();
    // print(AuthService.getToken().then((res) => print(res)));
  }

  @override
  void dispose() {
    passwordController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    // final passwordVisibilityBloc = context.read<PasswordVisibilityBloc>();
    Future<String?> openDialog() => showDialog<String?>(
        context: context,
        builder: (context) => StatefulBuilder(
              builder: (context, setState) => AlertDialog(
                title: const Text('Enter your password'),
                content: TextFormField(
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
                actions: [
                  TextButton(
                      onPressed: () {
                        Navigator.of(context).pop(passwordController.text);
                      },
                      child: const Text('Submit'))
                ],
              ),
            ));

    return Scaffold(
      appBar: AppBar(
        title: const Text('Password Manager'),
        automaticallyImplyLeading: false,
      ),
      body: FutureBuilder<List<PasswordAccountDTO>?>(
        future: fetchPasswordAccounts(),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return const Center(child: CircularProgressIndicator());
          } else if (snapshot.hasError) {
            return Center(child: Text('Error: ${snapshot.error}'));
          } else if (!snapshot.hasData || snapshot.data!.isEmpty) {
            return const Center(child: Text('No password accounts available'));
          } else {
            final passwordAccounts = snapshot.data!;
            // statefulbuilder is handy for rerendering itself with its own local setState() rather than the entire stateful widget's setState()
            return StatefulBuilder(
              builder: (context, setState) => SingleChildScrollView(
                child: Column(
                  children: [
                    PaginatedDataTable(
                      header: const Text('Password Accounts'),
                      columns: const [
                        DataColumn(label: Text('ID')),
                        DataColumn(label: Text('Title')),
                        DataColumn(label: Text('Username')),
                        DataColumn(label: Text('Password')),
                        DataColumn(label: Text('Created At')),
                        DataColumn(label: Text('Last Updated At')),
                      ],
                      source: PasswordTableSource(
                          passwordAccounts, isVisible), // Custom data source
                      rowsPerPage: 10, // Set the number of rows per page
                    ),
                    const SizedBox(
                      height: 30.0,
                    ),
                    ElevatedButton(
                        onPressed: () async {
                          if (isVisible) {
                            isVisible = false;
                            setState(() {});
                            return;
                          }
                          var result = await openDialog();
                          if (result == null || result.isEmpty) {
                            Fluttertoast.showToast(
                                msg: 'Please enter a value',
                                toastLength: Toast.LENGTH_SHORT,
                                gravity: ToastGravity.CENTER,
                                timeInSecForIosWeb: 1,
                                textColor: Colors.black,
                                backgroundColor: Colors.redAccent,
                                fontSize: 24.0);
                            return;
                          }
                          passwordController.clear();

                          var response =
                              await AuthService.checkPassword(result);

                          if (!response.flag) {
                            Fluttertoast.showToast(
                                msg: response.message,
                                toastLength: Toast.LENGTH_LONG,
                                gravity: ToastGravity.CENTER,
                                timeInSecForIosWeb: 1,
                                textColor: Colors.black,
                                backgroundColor: Colors.redAccent,
                                fontSize: 24.0);
                            return;
                          }

                          isVisible = true;
                          setState(() {});
                        },
                        child: const Text('Show/Hide Password')),
                    const SizedBox(
                      height: 30.0,
                    ),
                  ],
                ),
              ),
            );
          }
        },
      ),
    );
  }
}
