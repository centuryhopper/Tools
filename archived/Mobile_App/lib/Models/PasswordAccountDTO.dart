

class PasswordAccountDTO {
  int id;
  int userId;
  String? title;
  String? username;
  String password;
  DateTime? createdAt;
  DateTime? lastUpdatedAt;

  PasswordAccountDTO({
    required this.id,
    required this.userId,
    this.title,
    this.username,
    required this.password,
    this.createdAt,
    this.lastUpdatedAt,
  });

  // Optional: factory method for creating from JSON
  factory PasswordAccountDTO.fromJson(Map<String, dynamic> json) {
    return PasswordAccountDTO(
      id: json['id'],
      userId: json['userId'],
      title: json['title'],
      username: json['username'],
      password: json['password'],
      createdAt: json['createdAt'] != null ? DateTime.parse(json['createdAt']) : null,
      lastUpdatedAt: json['lastUpdatedAt'] != null ? DateTime.parse(json['lastUpdatedAt']) : null,
    );
  }

  // Optional: method to convert to JSON
  Map<String, dynamic> toJson() {
    return {
      'id': id,
      'userId': userId,
      'title': title,
      'username': username,
      'password': password,
      'createdAt': createdAt?.toIso8601String(),
      'lastUpdatedAt': lastUpdatedAt?.toIso8601String(),
    };
  }

  @override
  String toString() {
    return 'Id:$id, Title:$title, Username:$username, Password:$password, UserId:$userId';
  }
}
