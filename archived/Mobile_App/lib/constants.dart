import 'package:flutter/material.dart';

const List<BottomNavigationBarItem> ourBottomNavBarLst = [
  BottomNavigationBarItem(
      backgroundColor: Colors.red,
      icon: Icon(Icons.lock, color: Colors.black),
      label: 'Passwords'),
  BottomNavigationBarItem(
      backgroundColor: Colors.green,
      icon: Icon(Icons.settings, color: Colors.black),
      label: 'Settings'),
  BottomNavigationBarItem(
      backgroundColor: Colors.purple,
      icon: Icon(Icons.person, color: Colors.black),
      label: 'Profile'),
  BottomNavigationBarItem(
      backgroundColor: Colors.blue,
      icon: Icon(Icons.logout, color: Colors.black),
      label: 'Logout'),
];
