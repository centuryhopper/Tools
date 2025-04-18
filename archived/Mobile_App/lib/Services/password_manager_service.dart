import 'dart:convert';

import 'package:PasswordManager/Services/AuthService.dart';
import 'package:http/http.dart' as http;
import 'package:PasswordManager/Models/PasswordAccountDTO.dart';

class PasswordManagerService {
  static Future<List<PasswordAccountDTO>?> getPasswordAccounts() async {
    // This IP maps localhost on the emulator to your machine's localhost.
    final url = Uri.parse(
        'https://leopasswordmanager-production.up.railway.app/api/PasswordManager/passwords');
    final jwt = await AuthService.getToken();
    // print('jwt: $jwt');

    try {
      final response = await http.get(
        url,
        headers: {
          'Content-Type': 'application/json',
          'accept': '*/*',
          'Authorization': 'Bearer $jwt',
        },
      );

      if (response.statusCode == 200) {
        // Parse the JSON response into a LoginResponse object
        List<dynamic> jsonResponse = jsonDecode(response.body);
        var result = jsonResponse
            .map((json) => PasswordAccountDTO.fromJson(json))
            .toList();
        return result;
      } else {
        // print(
        //     'An error occurred. Status code: ${response.statusCode}. Message: ${response.body}');
        throw Exception(
            'An error occurred. Status code: ${response.statusCode}. Message: ${response.body}');
      }
    } catch (e) {
      return null;
    }
  }
}
