<template>
  <!-- <DataTable class="p-datatable" :value="transactions" tableStyle="min-width: 50rem">
    <Column field="date" header="Date"></Column>
    <Column field="name" header="Name"></Column>
    <Column field="amount" header="Amount"></Column>
    <Column field="category" header="Category"></Column>
    <Column field="note" header="Note"></Column>
  </DataTable> -->
  <div class="m-5 container">
    <DataTable :data="data" class="display" style="width: 100%">
      <thead>
        <tr>
          <th>Date</th>
          <th>Name</th>
          <th>Amount</th>
          <th>Category</th>
          <th>Note</th>
        </tr>
      </thead>
    </DataTable>
  </div>
</template>

<script setup lang="ts">
// import { onMounted, ref } from "vue";
import DataTablesCore from "datatables.net";
import "datatables.net-dt/css/dataTables.dataTables.css";

import DataTable from "datatables.net-vue3";

DataTable.use(DataTablesCore);

const data = [
  ["2023-11-14", "Company Co.", "$2000", "Salary", "Monthly pay"],
  ["2023-11-15", "Netflix", "-$15", "Subscription", "Monthly sub"],
  ["2023-11-16", "Apple", "-$99", "App Store", "One-time app"],
  ["2023-11-17", "Spotify", "-$10", "Entertainment", "Music"],
  ["2023-11-18", "Upwork", "$520", "Freelance", "Side project"],
];

defineProps<{ transactions: Array<any> }>();
</script>

<style scoped></style>
