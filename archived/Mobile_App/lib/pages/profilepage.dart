import 'package:flutter/material.dart';
import 'package:PasswordManager/Models/ClaimTypes.dart';
import 'package:PasswordManager/Models/LoginDTO.dart';
import 'package:PasswordManager/Services/AuthService.dart';

class ProfilePage extends StatelessWidget {
  const ProfilePage({
    super.key,
    LoginDTO? loginDTO,
  });

  Future<Map<String, dynamic>> getClaims() async {
    var claims = await AuthService.getClaims();
    return claims;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Your Profile Information'),
        centerTitle: true,
      ),
      body: FutureBuilder<Map<String, dynamic>>(
        future: getClaims(),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return const Center(child: CircularProgressIndicator());
          } else if (snapshot.hasError) {
            return Center(child: Text('Error: ${snapshot.error}'));
          } else if (snapshot.hasData) {
            final claims = snapshot.data!;
            final email = claims[ClaimTypes.email];
            final role = claims[ClaimTypes.role];
            final userId = claims[ClaimTypes.nameIdentifier];
            final name = claims[ClaimTypes.name];

            return Center(
              child: SingleChildScrollView(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  children: [
                    Card(
                      elevation: 4,
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(10),
                      ),
                      child: Padding(
                        padding: const EdgeInsets.all(16.0),
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            _buildProfileInfo('Name', name),
                            const Divider(),
                            _buildProfileInfo('Email', email),
                            const Divider(),
                            _buildProfileInfo('Role', role),
                            const Divider(),
                            _buildProfileInfo('User ID', userId),
                          ],
                        ),
                      ),
                    ),
                    // const SizedBox(height: 20), // Spacing after the card
                    // ElevatedButton(
                    //   onPressed: () {
                    //     // Add any action, like editing profile or logging out
                    //   },
                    //   style: ElevatedButton.styleFrom(
                    //     padding: const EdgeInsets.symmetric(horizontal: 30, vertical: 15),
                    //     shape: RoundedRectangleBorder(
                    //       borderRadius: BorderRadius.circular(10),
                    //     ),
                    //   ),
                    //   child: const Text('Edit Profile'),
                    // ),
                  ],
                ),
              ),
            );
          } else {
            return const Center(child: Text('No data available.'));
          }
        },
      ),
    );
  }

  Widget _buildProfileInfo(String label, String? value) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceBetween,
      children: [
        Text(
          '$label:',
          style: const TextStyle(
            fontSize: 20,
            fontWeight: FontWeight.bold,
          ),
        ),
        Text(
          value ?? 'N/A',
          style: TextStyle(
            fontSize: 20,
            color: Colors.grey[700],
          ),
        ),
      ],
    );
  }
}
