import 'package:PasswordManager/statemanagement/bloc/DarkMode/darkmode_event.dart';
import 'package:PasswordManager/statemanagement/bloc/DarkMode/darkmode_state.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:shared_preferences/shared_preferences.dart';

class DarkModeBloc extends Bloc<DarkModeEvent, DarkModeState> {
  DarkModeBloc(bool initialState) : super(DarkModeState(initialState)) {
    on<DarkModeToggle>((event, emit) {
      final newState = !state.isDark;
      emit(DarkModeState(newState));
      _saveDarkModePreference(newState); // Save the new state
    });
  }

  Future<void> _saveDarkModePreference(bool isDark) async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setBool('isDark', isDark);
  }
}
