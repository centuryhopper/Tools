import 'dart:convert';

import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:http/http.dart' as http;
import 'package:jwt_decoder/jwt_decoder.dart';
import 'package:PasswordManager/Models/ClaimTypes.dart';
import 'package:PasswordManager/Models/LoginDTO.dart';
import 'package:PasswordManager/Models/ServiceResponses.dart';
import 'package:shared_preferences/shared_preferences.dart';

class AuthService {
  static const _storage = FlutterSecureStorage();
  static const _rememberMeKey = 'remember_me';
  static const _tokenKey = 'auth_token';

  static Future<void> saveRememberMeFlag(bool rememberMe) async {
    SharedPreferences prefs = await SharedPreferences.getInstance();
    await prefs.setBool(_rememberMeKey, rememberMe);
  }

  static Future<bool> getRememberMeFlag() async {
    SharedPreferences prefs = await SharedPreferences.getInstance();
    bool? rememberMe = prefs.getBool(_rememberMeKey) ?? false;
    return rememberMe;
  }

  static Future<void> saveToken(String token) async {
    await _storage.write(key: _tokenKey, value: token);
  }

  // Retrieve token
  static Future<String?> getToken() async {
    return await _storage.read(key: _tokenKey);
  }

  // Clear login token
  static Future<void> clearLogin() async {
    // await _storage.delete(key: _tokenKey);
    await _storage.deleteAll();
    SharedPreferences prefs = await SharedPreferences.getInstance();
    await prefs.remove(_rememberMeKey);
  }

  static Future<Map<String, dynamic>> getClaims() async {
    return {
      ClaimTypes.role: await _storage.read(key: ClaimTypes.role),
      ClaimTypes.nameIdentifier:
          await _storage.read(key: ClaimTypes.nameIdentifier),
      ClaimTypes.name: await _storage.read(key: ClaimTypes.name),
      ClaimTypes.email: await _storage.read(key: ClaimTypes.email),
    };
  }

  static Future<void> decodeToken(String token) async {
    // Decode the token
    Map<String, dynamic> decodedToken = JwtDecoder.decode(token);
    await _storage.write(
        key: ClaimTypes.role, value: decodedToken[ClaimTypes.role]);
    await _storage.write(
        key: ClaimTypes.nameIdentifier,
        value: decodedToken[ClaimTypes.nameIdentifier]);
    await _storage.write(
        key: ClaimTypes.name, value: decodedToken[ClaimTypes.name]);
    await _storage.write(
        key: ClaimTypes.email, value: decodedToken[ClaimTypes.email]);
  }

  static Future<GeneralResponse> checkPassword(String password) async {
    // This IP maps localhost on the emulator to your machine's localhost:
    // http://10.0.2.2:5220
    final url = Uri.parse(
        'https://leopasswordmanager-production.up.railway.app/api/Account/check-password?password=$password');
    final jwt = await getToken();

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
        GeneralResponse jsonResponse =
            GeneralResponse.fromJson(jsonDecode(response.body));
        // print(jsonResponse);
        return jsonResponse;
      } else {
        // print(
        // 'An error occurred. Status code: ${response.statusCode}. Message: ${response.body}');
        GeneralResponse jsonResponse =
            GeneralResponse.fromJson(jsonDecode(response.body));
        // print(jsonResponse.toJson());
        return jsonResponse;
        // throw Exception(
        //     'An error occurred. Status code: ${response.statusCode}. Message: ${response.body}');
      }
    } catch (e) {
      return GeneralResponse(flag: false, message: e.toString());
    }
  }

  static Future<LoginResponse> login(
      String email, String password, bool rememberMe) async {
    // This IP maps localhost on the emulator to your machine's localhost.
    final url = Uri.parse(
        'https://leopasswordmanager-production.up.railway.app/api/Account/login');

    // Prepare the request body
    final body = jsonEncode(
        LoginDTO(email: email, password: password, rememberMe: rememberMe)
            .toJson());

    // print('body: $body');

    try {
      final response = await http.post(
        url,
        headers: {'Content-Type': 'application/json', 'accept': '*/*'},
        body: body,
      );

      // print('here');
      if (response.statusCode == 200) {
        // Parse the JSON response into a LoginResponse object
        final jsonResponse = jsonDecode(response.body);
        var result = LoginResponse.fromJson(jsonResponse);
        if (result.flag) {
          await saveToken(result.token!);
        }
        await saveRememberMeFlag(rememberMe);
        return result;
      } else {
        // print('response.body: ' + response.body);
        final jsonResponse = jsonDecode(response.body);
        // print(jsonResponse);
        var result = LoginResponse.fromJson(jsonResponse);
        return result;
      }
    } catch (e) {
      return LoginResponse(
          flag: false, token: "", message: 'Error during login: $e');
    }
  }
}
