<template>
  <div class="m-5 container">
    <!-- Toggle Mode -->
    <button class="btn btn-primary mb-3" @click="toggleMode">
      {{ mode === Mode.Readonly ? "Edit" : "View" }}
    </button>

    <!-- READONLY MODE -->
    <DataTable
      v-if="mode === Mode.Readonly"
      :data="readonlyData"
      class="display"
      style="width: 100%"
    >
      <thead>
        <tr>
          <th>Row #</th>
          <th>Date</th>
          <th>Name</th>
          <th>Amount</th>
          <th>Category</th>
          <th>Note</th>
        </tr>
      </thead>
    </DataTable>

    <!-- EDIT MODE -->
    <div v-else>
      <!-- Filter/Search Controls -->
      <div class="d-flex justify-content-between mb-3">
        <input
          v-model="searchText"
          placeholder="Search..."
          class="form-control w-50 me-2"
        />

        <select v-model="categoryFilter" class="form-select w-25">
          <option value="">All Categories</option>
          <option v-for="category in uniqueCategories" :key="category">
            {{ category }}
          </option>
        </select>
      </div>

      <!-- Editable Table -->
      <table class="table table-bordered" style="width: 100%">
        <thead>
          <tr>
            <th>Row #</th>
            <th>Date</th>
            <th>Name</th>
            <th>Amount</th>
            <th>Category</th>
            <th>Note</th>
            <th>Actions</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="(row, rowIndex) in paginatedData" :key="rowIndex">
            <td>
              <span>{{ rowIdxOffset + rowIndex + 1 }}</span>
            </td>
            <td>
              <input type="date" v-model="row.date" class="form-control" readonly />
            </td>
            <td>
              <input type="text" v-model="row.name" class="form-control" readonly />
            </td>
            <td>
              <input type="text" v-model="row.amount" class="form-control" readonly />
            </td>
            <td>
              <select v-model="row.category" class="form-select">
                <option
                  v-for="category in uniqueCategories"
                  :key="category"
                  :value="category"
                >
                  {{ category }}
                </option>
              </select>
            </td>
            <td><input type="text" v-model="row.note" class="form-control" /></td>
            <td>
              <button
                class="btn btn-success btn-sm"
                @click="saveRow(row, getGlobalIndex(row))"
                :disabled="savingRowIndex === getGlobalIndex(row)"
              >
                <span v-if="savingRowIndex === getGlobalIndex(row)">Saving...</span>
                <span v-else>Save</span>
              </button>
            </td>
          </tr>
        </tbody>
      </table>

      <!-- Pagination Controls -->
      <div class="d-flex justify-content-between align-items-center">
        <div>
          <label>Rows per page:</label>
          <select
            v-model.number="rowsPerPage"
            class="form-select d-inline-block w-auto ms-2"
          >
            <option :value="10">10</option>
            <option :value="25">25</option>
            <option :value="50">50</option>
            <option :value="100">100</option>
          </select>
        </div>

        <div>
          <button
            class="btn btn-secondary me-2"
            :disabled="currentPage === 1"
            @click="currentPage--"
          >
            Prev
          </button>
          <span>Page {{ currentPage }} / {{ totalPages }}</span>
          <button
            class="btn btn-secondary ms-2"
            :disabled="currentPage === totalPages"
            @click="currentPage++"
          >
            Next
          </button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import axios from "axios";
import DataTablesCore from "datatables.net";
import "datatables.net-dt/css/dataTables.dataTables.css";
import DataTable from "datatables.net-vue3";
import { computed, reactive, ref, watch } from "vue";
import useSweetAlertPopups from "../composables/useSweetAlertPopups";
import { authStore } from "../stores/auth";
import type { Transaction } from "../types/Transactions";
import { API_BASE_URL } from "../utils/utils";

DataTable.use(DataTablesCore);

enum Mode {
  Readonly = "readonly",
  Edit = "edit",
}

const mode = ref<Mode>(Mode.Readonly);
function toggleMode() {
  mode.value = mode.value === Mode.Readonly ? Mode.Edit : Mode.Readonly;
}

const { showFeedbackPopup } = useSweetAlertPopups();

const { transactions } = defineProps<{ transactions: Transaction[] }>();
const editableData = reactive<{ items: Transaction[] }>({
  items: [],
});
const authStre = authStore();
const readonlyData = computed(() => {
  // display the most recent dates first
  const result = editableData.items
    // .filter((t) => t.date !== undefined)
    // .sort((a, b) => b.date!.localeCompare(a.date!))
    .map((t, idx) => [idx + 1, t.date, t.name, t.amount, t.category, t.note]);
  // console.log(result);
  return result;
});

watch(
  () => transactions,
  (newTransactions) => {
    // update const data here
    editableData.items = [...newTransactions];
    // console.log(editableData.items);
  },
  { deep: true, immediate: true }
);

// Filters
const searchText = ref("");
const categoryFilter = ref("");

// Pagination
const currentPage = ref(1);
const rowsPerPage = ref(10);

// Filtered & Searched
const filteredData = computed(() =>
  editableData.items.filter((row) => {
    const matchesSearch =
      searchText.value === "" ||
      Object.values(row).some((cell) =>
        cell.toString().toLowerCase?.().includes(searchText.value.toLowerCase())
      );

    const matchesCategory =
      categoryFilter.value === "" || row.category === categoryFilter.value;

    return matchesSearch && matchesCategory;
  })
);

// Pagination logic
const totalPages = computed(() =>
  Math.ceil(filteredData.value.length / rowsPerPage.value)
);

const paginatedData = computed(() => {
  const start = (currentPage.value - 1) * rowsPerPage.value;
  return [...filteredData.value].slice(start, start + rowsPerPage.value);
});

const rowIdxOffset = computed(() => {
  const start = (currentPage.value - 1) * rowsPerPage.value;
  return start;
});

// Extract unique categories
const uniqueCategories = computed(() => {
  const set = new Set<string>();
  editableData.items.forEach((row) => set.add(row.category!));
  return [...set];
});

// Track saving row
const savingRowIndex = ref<number | null>(null);

// Get global row index from paginated row reference
function getGlobalIndex(row: Transaction): number {
  return editableData.items.findIndex((r) => r.id === row.id);
}

// Save row to "API"
async function saveRow(row: Transaction, index: number) {
  savingRowIndex.value = index;
  try {
    // console.log("Saving row to API:", row);
    const response = await axios.patch(
      API_BASE_URL + "api/Dashboard/editTransaction",
      row,
      {
        headers: {
          Authorization: `Bearer ${authStre.token}`,
          "Content-Type": "application/json",
        },
      }
    );
    // console.log(response.data.message);
    if (response.data.flag) {
      showFeedbackPopup({
        successMsg: response.data.message,
      });
    } else {
      showFeedbackPopup({ success: false, failMsg: response.data.message });
    }
  } catch (err) {
    showFeedbackPopup({
      success: false,
      failMsg: err as string,
    });
  } finally {
    savingRowIndex.value = null;
  }
}
</script>

<style scoped>
input.form-control {
  width: 100%;
  padding: 4px;
  box-sizing: border-box;
}
</style>
