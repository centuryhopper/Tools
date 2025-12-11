<template>
  <div class="container m-5 p-5">
    <h1 class="fw-bold fs-3 mt-5 mb-3 text-center">
      Budget for {{ currentMonthString }}
    </h1>
    <div v-if="!spendingsLoaded">
      <div class="d-flex justify-content-center">
        <div class="spinner-border" role="status">
          <span class="visually-hidden">Loading...</span>
        </div>
      </div>
    </div>
    <div v-else-if="spendings.items.length === 0 && spendingsLoaded">
      <div class="m-3">No budget logistics to show</div>
    </div>
    <div v-else>
      <div class="d-flex justify-content-center">
        <div :class="`card ${isDark ? 'bg-dark' : ''} bg-gradient`">
          <div class="card-body">
            <div class="card-text">
              <Budgetsbar
                v-for="spending in spendings.items"
                :key="spending.id"
                :id="spending.id"
                :category="spending.category"
                :spent="spending.spent"
                :budget="spending.categoryBudget"
                @update:budgetLimit="
                  (id, newBudgetLimit) => {
                    updatebudgetLimit(id, newBudgetLimit);
                  }
                "
              />
              <!-- listen for that change -->
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import axios from "axios";
import { onMounted, reactive, ref } from "vue";
import { authStore } from "../stores/auth";
import { useTheme } from "../stores/theme-store";
import type { CurrentMonthlySpending } from "../types/CurrentMonthlySpending";
import { API_BASE_URL } from "../utils/utils";
import Budgetsbar from "./Budgetsbar.vue";

const { isDark } = useTheme();
const selectedBankId = sessionStorage.getItem("selectedBank");

const updatebudgetLimit = async (id: number, newLimit: number) => {
  const item = spendings.items.find((b) => b.id === id);
  if (!item) {
    console.log("budget not found :/");
    return;
  }
  // console.log("updated budget limit for categoryid:", item.categoryId);
  // make api call here to edit budget cap
  // server handles userid
  const response = await axios.patch(API_BASE_URL + "api/Budgets/edit-budgetcap", null, {
    params: {
      categoryId: item.categoryId,
      bankInfoId: selectedBankId,
      budgetCap: newLimit,
    },
    headers: {
      Authorization: `Bearer ${authStre.token}`,
      "Content-Type": "application/json",
    },
  });

  if (response.status === 200) {
    const idx = spendings.items.findIndex((s) => s.id === id);
    spendings.items[idx].categoryBudget = newLimit;
  }

  // console.log(response.data);
};

const monthNames = [
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December",
];

const currentMonthString = monthNames[new Date().getMonth()];
const didSelectBank = ref(!!selectedBankId);
const authStre = authStore();
const spendings = reactive<{ items: CurrentMonthlySpending[] }>({
  items: [],
});
const spendingsLoaded = ref(false);

onMounted(async () => {
  if (!didSelectBank.value) {
    // console.log("didSelectBank.value", didSelectBank.value);
    return;
  }

  const initBudgetsResponse = await axios.get(
    API_BASE_URL + "api/Budgets/init-budgets/" + parseInt(selectedBankId!),
    {
      headers: {
        Authorization: `Bearer ${authStre.token}`,
      },
    }
  );

  // console.log("initBudgetsResponse.data", initBudgetsResponse.data);

  if (initBudgetsResponse.status !== 200) {
    console.log("couldn't initialize budgets properly");
    return;
  }

  const response = await axios.get<{ payload: CurrentMonthlySpending[] }>(
    API_BASE_URL +
      "api/Budgets/current-month-spending-by-category/" +
      parseInt(selectedBankId!),
    {
      headers: {
        Authorization: `Bearer ${authStre.token}`,
      },
    }
  );

  if (response.status !== 200) {
    console.log("couldn't retrieve category spendings for the current month");
    return;
  }

  spendingsLoaded.value = true;
  spendings.items = response.data.payload;

  // console.log(spendings.items);
});

// const budgetInfo = ref([
//   {
//     id: 0,
//     category: "Groceries",
//     budgetCap: 400,
//     spent: 255,
//   },
//   {
//     id: 1,
//     category: "Rent",
//     budgetCap: 1200,
//     spent: 1200,
//   },
//   {
//     id: 2,
//     category: "Entertainment",
//     budgetCap: 400,
//     spent: 300,
//   },
// ]);
</script>

<style scoped>
/* * {
  color: white;
} */
</style>
