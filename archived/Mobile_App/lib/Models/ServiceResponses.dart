// service_responses.dart
class GeneralResponse {
  final bool flag;
  final String message;

  GeneralResponse({
    required this.flag,
    required this.message,
  });

  // Optional: factory method for creating from JSON if needed
  factory GeneralResponse.fromJson(Map<String, dynamic> json) {
    return GeneralResponse(
      flag: json['flag'],
      message: json['message'],
    );
  }

  // Optional: method to convert to JSON if needed
  Map<String, dynamic> toJson() {
    return {
      'flag': flag,
      'message': message,
    };
  }
}

class LoginResponse {
  final bool flag;
  final String? token;
  final String message;

  LoginResponse({
    required this.flag,
    required this.token,
    required this.message,
  });

  // Optional: factory method for creating from JSON
  factory LoginResponse.fromJson(Map<String, dynamic> json) {
    return LoginResponse(
      flag: json['flag'],
      token: json['token'],
      message: json['message'],
    );
  }

  // Optional: method to convert to JSON
  Map<String, dynamic> toJson() {
    return {
      'flag': flag,
      'token': token,
      'message': message,
    };
  }
}
