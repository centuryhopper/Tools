class LoginDTO {
  String email;
  String password;
  bool rememberMe;

  LoginDTO({
    required this.email,
    required this.password,
    this.rememberMe = false,
  });

  // Optional: factory method for creating from JSON
  factory LoginDTO.fromJson(Map<String, dynamic> json) {
    return LoginDTO(
      email: json['email'],
      password: json['password'],
      rememberMe: json['rememberMe'] ?? false,
    );
  }

  // Optional: method to convert to JSON
  Map<String, dynamic> toJson() {
    return {
      'email': email,
      'password': password,
      'rememberMe': rememberMe,
    };
  }
}
