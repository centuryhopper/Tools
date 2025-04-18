import 'package:PasswordManager/Models/PasswordAccountDTO.dart';
import 'package:flutter/material.dart';
import 'package:intl/intl.dart';

class PasswordTableSource extends DataTableSource {
  final List<PasswordAccountDTO> _passwordAccounts;
  final bool _isVisible;

  PasswordTableSource(this._passwordAccounts, this._isVisible);

  @override
  DataRow getRow(int index) {
    assert(index >= 0);
    // if (index >= _passwordAccounts.length) return null;

    final passwordAccount = _passwordAccounts[index];

    return DataRow.byIndex(
      index: index,
      cells: [
        DataCell(Text('${passwordAccount.id}')),
        DataCell(Text(passwordAccount.title ?? '')),
        DataCell(Text(passwordAccount.username ?? '')),
        DataCell(
          Text(_isVisible ? passwordAccount.password : '●●●●●●●●'),
        ),
        DataCell(Text(passwordAccount.createdAt == null
            ? ''
            : DateFormat('yyyy-MM-dd').format(passwordAccount.createdAt!))),
        DataCell(Text(passwordAccount.lastUpdatedAt == null
            ? ''
            : DateFormat('yyyy-MM-dd').format(passwordAccount.lastUpdatedAt!))),
      ],
    );
  }

  @override
  bool get isRowCountApproximate => false;

  @override
  int get rowCount => _passwordAccounts.length;

  @override
  int get selectedRowCount => 0;
}
